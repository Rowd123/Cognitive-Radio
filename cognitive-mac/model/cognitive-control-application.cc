/*
 * Copyright (c) 2025 Telecommunications Lab, Higher Institiute for Applied Sciences and Technology , Damascus.
 *
 *
 * Author: Rida Takla <raidotakla@gmail.com>
 */

#include "cognitive-control-application.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("CognitiveControlApplication");
  
NS_OBJECT_ENSURE_REGISTERED(CognitiveControlApplication);

CognitiveControlApplication::CognitiveControlApplication():
      m_spectrumControlModule(nullptr),
      m_resenseTime(Seconds(1))
{

}

CognitiveControlApplication::~CognitiveControlApplication()
{
      m_spectrumControlModule = nullptr; 
}

TypeId

CognitiveControlApplication::GetTypeId()
{
      static TypeId tid =
        TypeId("ns3::CognitiveControlApplication")
            .SetParent<Application>()
            .SetGroupName("Network")
            .AddConstructor<CognitiveControlApplication>();
      return tid;
}

void

CognitiveControlApplication::SetSpectrumControlModule(Ptr<SpectrumControlModule> spectrumModule)
{
      m_spectrumControlModule = spectrumModule;
}

void

CognitiveControlApplication::SetDataDevice(Ptr<NetDevice> dataDevice)
{
      m_dataDevice = dataDevice;
}

void

CognitiveControlApplication::SetControlDevice(Ptr<NetDevice> controlDevice)
{
      m_controlDevice = controlDevice;
}

void

CognitiveControlApplication::SetChannelSensingCallback(ChannelSensingCallback c)
{
      m_channelSensingCallback = c ;
}

void

CognitiveControlApplication::SetStartSensingPeriodCallback(StartSensingPeriodCallback c)
{
      m_startSensingPeriodCallback = c;
}

double

CognitiveControlApplication::SelectedChannelResult(uint16_t Index)
{
      NS_ASSERT_MSG(m_dataDevice,"there is no data device assigned to the control application");
      NS_ASSERT_MSG(!m_channelSensingCallback.IsNull(),"Set the channel sensing callback");
      return m_channelSensingCallback(Index);
}

void

CognitiveControlApplication::StartApplication()
{
      NS_ASSERT_MSG(m_spectrumControlModule,"you haven't linked the spectrum module yet");
      Simulator::ScheduleNow(&CognitiveControlApplication::StartSensingPeriod,this);
}


void

CognitiveControlApplication::StopApplication()
{
      m_curAction.Cancel();
      m_resense.Cancel();

}

void

CognitiveControlApplication::ReceivingQtable(std::vector<double> Qtable)
{
      m_resense = Simulator::Schedule(m_resenseTime,&CognitiveControlApplication::StartSensingPeriod,this);
}

void

CognitiveControlApplication::StartSensingPeriod()
{
     NS_ASSERT_MSG(!m_startSensingPeriodCallback.IsNull(),"the start sensing period isn't connected to the net device");
     m_startSensingPeriodCallback(m_spectrumControlModule->GetBandGroupSensingTime());
     Simulator::ScheduleNow(&SpectrumControlModule::StartWorking,m_spectrumControlModule); 
}

}