/*
 * Copyright (c) 2025 Telecommunications Lab, Higher Institiute for Applied Sciences and Technology , Damascus.
 *
 *
 * Author: Rida Takla <raidotakla@gmail.com>
 */
#include "spectrum-control-module.h"
#include "ns3/simulator.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("SpectrumControlModule");
  
NS_OBJECT_ENSURE_REGISTERED(SpectrumControlModule);



SpectrumControlModule::SpectrumControlModule() :
    m_SingleChannelSensingPeriod(MicroSeconds(10)),
    m_ChannelMeasuringTime(MicroSeconds(1)),
    m_Nsensing(5),m_threshold(0.0),
    m_w1(0.5),m_w2(0.5),m_learningRate(0.5),
    m_discountFactor(0.5)
{
    m_Qtable = new std::vector<double>();
    m_bgQtable = new std::vector<double>();
}

SpectrumControlModule::~SpectrumControlModule()
{

}

TypeId

SpectrumControlModule::GetTypeId()
{
    static TypeId tid =
    TypeId("ns3::SpectrumControlModule")
            .SetParent<Object>()
            .SetGroupName("Spectrum")
            .AddConstructor<SpectrumControlModule>();
    return tid;
}

void

SpectrumControlModule::SetThreshold(double threshold)
{
    m_threshold = threshold;
}

void 

SpectrumControlModule::SetSenseResultCallback(SenseResultCallback c)
{
    m_senseResultCallback = c ;
}

void

SpectrumControlModule::SetQtableResultCallback(QtableResultCallback c)
{
    m_QtableResultCallback = c ;
}

void

SpectrumControlModule::SetChannels(uint16_t bgSize , uint16_t bgCount)
{
    NS_ASSERT_MSG((bgSize > 0 && bgCount > 0) , "number of channels must be larger than zero");
    m_bgSize = bgSize;
    m_bgCount = bgCount;
    (*m_Qtable).assign(bgSize*bgCount,0.0);
    (*m_bgQtable).assign(bgSize,0.0);
}

void

SpectrumControlModule::SetSingleChannelSensingPeriod(Time Ts)
{
    m_SingleChannelSensingPeriod = Ts; 
}

void

SpectrumControlModule::SetChannelMeasuringTime(Time Ts)
{
    m_ChannelMeasuringTime = Ts;
}

void

SpectrumControlModule::SetLearningRate(double learningRate)
{
    m_learningRate = learningRate ; 
}

void

SpectrumControlModule::SenseSpectrum()
{
    
    NS_ASSERT_MSG((*m_bgQtable).size() > 0 , "the number of channels must be greater than 0");
    NS_ASSERT_MSG(m_threshold > 0 , "you haven't set the threshold");
    NS_ASSERT_MSG(!m_senseResultCallback.IsNull(),"you haven't linked the phy device and the spectrum module");
    double maxQbg = -1.0 ;
    uint16_t bgIndex = -1 ;
    Temp.assign(m_bgSize,std::vector<bool>(m_Nsensing,false));
    for(uint16_t i = 0 ; i < m_bgCount ; i++)
    {
        if(maxQbg<(*m_bgQtable)[i])
        {
            maxQbg = (*m_bgQtable)[i] ;
            bgIndex = i ; 
        }
    }
    for(uint16_t i = 0 ; i < m_bgSize ; i++)
    {
        for(uint16_t j = 0 ;j < m_Nsensing ; j++)
        {
            Simulator::Schedule(m_SingleChannelSensingPeriod*j+i*m_ChannelMeasuringTime,
                                &SpectrumControlModule::DoSenseChannel,this,bgIndex,i,j);
        }
    }
    Simulator::Schedule(m_Nsensing*m_SingleChannelSensingPeriod,
                        &SpectrumControlModule::UpdateQtable,this,bgIndex);
}

void

SpectrumControlModule::DoSenseChannel(uint16_t bgIndex, uint16_t Index, uint16_t fois)
{
    double res = m_senseResultCallback(Index + bgIndex*m_bgSize);
   // std::cout << res << " " << m_threshold << std::endl;
    if(res>=m_threshold)
    {
        Temp[Index][fois] = true;
    }
    else
    {
        Temp[Index][fois] = false;
    } 
}

void

SpectrumControlModule::UpdateQtable(uint16_t Index)
{
    
    int maxQvalue = *std::max_element((*m_Qtable).begin(),(*m_Qtable).end());
    for(uint16_t i = 0 ; i < m_bgSize ; i++)
    {
        double T = 0.0 ;
        double P = 0.0 ; 
        for(uint16_t j = 0 ; j < m_Nsensing ; j++)
        {
            //std::cout << Temp[i][j] << " ";
            if(!Temp[i][j])
            {
                if(j==0 || Temp[i][j-1]){T++;}
                P++;
            }
        }
     //   std::cout << std::endl;
        if(T > 0.0)
        {
            P = P / m_Nsensing ;
            T = P / T ;
        }
        double reward = m_w1*T + m_w2*P;
        (*m_Qtable)[Index*m_bgSize + i] = (1-m_learningRate)* (*m_Qtable)[Index*m_bgSize + i] 
                                            + m_learningRate*(reward + m_discountFactor*maxQvalue); 
    }
    double sum = 0.0 ;
    for(uint16_t i  = 0 ; i < m_bgSize ; i++)
    {
        sum+=(*m_Qtable)[Index*m_bgSize + i] ;
    }
    sum /= m_bgSize;
    Simulator::ScheduleNow(&SpectrumControlModule::SendSensingResult,this,Index);
}

void

SpectrumControlModule::SendSensingResult(uint16_t bgIndex)
{
    NS_ASSERT_MSG(!m_QtableResultCallback.IsNull(),"you haven't connected to the control application");
    std::map<uint16_t,double> Qtable;
    for(int i = 0 ; i < m_bgSize ; i++)
    {
        if(!Temp[i].back())
        {
            Qtable[bgIndex*m_bgSize + i] = (*m_Qtable)[bgIndex*m_bgSize + i ];
        }
    }
    m_QtableResultCallback(Qtable);
}

void 

SpectrumControlModule::StartWorking()
{
    Simulator::ScheduleNow(&SpectrumControlModule::SenseSpectrum,this);
}

Time

SpectrumControlModule::GetBandGroupSensingTime()
{
    return m_Nsensing*m_SingleChannelSensingPeriod;
}

}