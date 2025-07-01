/*
 * Copyright (c) 2025 Telecommunications Lab, Higher Institiute for Applied Sciences and Technology , Damascus.
 *
 *
 * Author: Rida Takla <raidotakla@gmail.com>
 */

 
 #include "cognitive-phy-device.h"
 
 #include "ns3/half-duplex-ideal-phy-signal-parameters.h"
 #include "ns3/spectrum-error-model.h"
 #include <ns3/antenna-model.h>
 #include <ns3/callback.h>
 #include <ns3/log.h>
 #include <ns3/object-factory.h>
 #include <ns3/packet-burst.h>
 #include <ns3/simulator.h>
 #include <ns3/trace-source-accessor.h>
 #include <ns3/double.h>    
 #include <cmath>
 #include <iomanip>
 
 using namespace ns3;
 
 namespace ns3
 {
  
 NS_LOG_COMPONENT_DEFINE("CognitivePhyDevice");
  
 NS_OBJECT_ENSURE_REGISTERED(CognitivePhyDevice);
  
 CognitivePhyDevice::CognitivePhyDevice()
     : m_mobility(nullptr),
       m_netDevice(nullptr),
       m_channel(nullptr),
       m_txPsd(nullptr),
       m_state(IDLE),
       m_channelIndex(0)
 {
     m_interference.SetErrorModel(CreateObject<ShannonSpectrumErrorModel>());
 }
  
 CognitivePhyDevice::~CognitivePhyDevice()
 {
 }
  
 void
 CognitivePhyDevice::DoDispose()
 {
     NS_LOG_FUNCTION(this);
     m_mobility = nullptr;
     m_netDevice = nullptr;
     m_channel = nullptr;
     m_txPsd = nullptr;
     m_rxPsd = nullptr;
     m_txPacket = nullptr;
     m_rxPacket = nullptr;
     m_localModel = nullptr;
     m_phyMacTxEndCallback = MakeNullCallback<void, Ptr<const Packet>>();
     m_phyMacRxStartCallback = MakeNullCallback<void>();
     m_phyMacRxEndErrorCallback = MakeNullCallback<void>();
     m_phyMacRxEndOkCallback = MakeNullCallback<void, Ptr<Packet>>();
     SpectrumPhy::DoDispose();
 }
  
 /**
  * @brief Output stream operator
  * @param os output stream
  * @param s the state to print
  * @return an output stream
  */
 std::ostream&
 operator<<(std::ostream& os, CognitivePhyDevice::State s)
 {
     switch (s)
     {
     case CognitivePhyDevice::IDLE:
         os << "IDLE";
         break;
     case CognitivePhyDevice::RX:
         os << "RX";
         break;
     case CognitivePhyDevice::TX:
         os << "TX";
         break;
     default:
         os << "UNKNOWN";
         break;
     }
     return os;
 }
  
 TypeId
 CognitivePhyDevice::GetTypeId()
 {
     static TypeId tid =
         TypeId("ns3::CognitivePhyDevice")
             .SetParent<SpectrumPhy>()
             .SetGroupName("Spectrum")
             .AddConstructor<CognitivePhyDevice>()
             .AddAttribute(
                 "Rate",
                 "The PHY rate used by this device",
                 DataRateValue(DataRate("1Mbps")),
                 MakeDataRateAccessor(&CognitivePhyDevice::SetRate, &CognitivePhyDevice::GetRate),
                 MakeDataRateChecker())
             .AddTraceSource("TxStart",
                             "Trace fired when a new transmission is started",
                             MakeTraceSourceAccessor(&CognitivePhyDevice::m_phyTxStartTrace),
                             "ns3::Packet::TracedCallback")
             .AddTraceSource("TxEnd",
                             "Trace fired when a previously started transmission is finished",
                             MakeTraceSourceAccessor(&CognitivePhyDevice::m_phyTxEndTrace),
                             "ns3::Packet::TracedCallback")
             .AddTraceSource("RxStart",
                             "Trace fired when the start of a signal is detected",
                             MakeTraceSourceAccessor(&CognitivePhyDevice::m_phyRxStartTrace),
                             "ns3::Packet::TracedCallback")
             .AddTraceSource("RxAbort",
                             "Trace fired when a previously started RX is aborted before time",
                             MakeTraceSourceAccessor(&CognitivePhyDevice::m_phyRxAbortTrace),
                             "ns3::Packet::TracedCallback")
             .AddTraceSource("RxEndOk",
                             "Trace fired when a previously started RX terminates successfully",
                             MakeTraceSourceAccessor(&CognitivePhyDevice::m_phyRxEndOkTrace),
                             "ns3::Packet::TracedCallback")
             .AddTraceSource("RxEndError",
                             "Trace fired when a previously started RX terminates with an error "
                             "(packet is corrupted)",
                             MakeTraceSourceAccessor(&CognitivePhyDevice::m_phyRxEndErrorTrace),
                             "ns3::Packet::TracedCallback");
     return tid;
 }
  
 Ptr<NetDevice>
 CognitivePhyDevice::GetDevice() const
 {
     NS_LOG_FUNCTION(this);
     return m_netDevice;
 }
  
 Ptr<MobilityModel>
 CognitivePhyDevice::GetMobility() const
 {
     NS_LOG_FUNCTION(this);
     return m_mobility;
 }
  
 void
 CognitivePhyDevice::SetDevice(Ptr<NetDevice> d)
 {
     NS_LOG_FUNCTION(this << d);
     m_netDevice = d;
 }
  
 void
 CognitivePhyDevice::SetMobility(Ptr<MobilityModel> m)
 {
     NS_LOG_FUNCTION(this << m);
     m_mobility = m;
 }
  
 void
 CognitivePhyDevice::SetChannel(Ptr<SpectrumChannel> c)
 {
     NS_LOG_FUNCTION(this << c);
     m_channel = c;
 }
  
 Ptr<const SpectrumModel>
 CognitivePhyDevice::GetRxSpectrumModel() const
 {
     if (m_txPsd)
     {
         return m_txPsd->GetSpectrumModel();
     }
     else
     {
         return nullptr;
     }
 }
  
 void
 CognitivePhyDevice::SetTxPowerSpectralDensity(Ptr<SpectrumValue> txPsd)
 {
     NS_LOG_FUNCTION(this << txPsd);
     NS_ASSERT(txPsd);
     m_txPsd = txPsd;
     NS_LOG_INFO(*txPsd << *m_txPsd);
 }
  
 void
 CognitivePhyDevice::SetNoisePowerSpectralDensity(Ptr<const SpectrumValue> noisePsd)
 {
     NS_LOG_FUNCTION(this << noisePsd);
     NS_ASSERT(noisePsd);
     m_interference.SetNoisePowerSpectralDensity(noisePsd);
 }

 void
 CognitivePhyDevice::SetRate(DataRate rate)
 {
     NS_LOG_FUNCTION(this << rate);
     m_rate = rate;
 }
  
 DataRate
 CognitivePhyDevice::GetRate() const
 {
     NS_LOG_FUNCTION(this);
     return m_rate;
 }

 void
 CognitivePhyDevice::SetLocalSpectrumModel(Ptr<SpectrumModel> localModel)
 {
    NS_LOG_FUNCTION(this);
    m_localModel = localModel;
 }

 Ptr<SpectrumModel>
 CognitivePhyDevice::GetLocalSpectrumModel()
 {
    NS_LOG_FUNCTION(this);
    return m_localModel;
 }

  
 void
 CognitivePhyDevice::SetGenericPhyTxEndCallback(GenericPhyTxEndCallback c)
 {
     NS_LOG_FUNCTION(this);
     m_phyMacTxEndCallback = c;
 }
 
 void
 CognitivePhyDevice::SetGenericPhyRxStartCallback(GenericPhyRxStartCallback c)
 {
     NS_LOG_FUNCTION(this);
     m_phyMacRxStartCallback = c;
 }

 void
 CognitivePhyDevice::SetGenericPhyRxEndErrorCallback(GenericPhyRxEndErrorCallback c)
 {
     NS_LOG_FUNCTION(this);
     m_phyMacRxEndErrorCallback = c;
 }

 void
 CognitivePhyDevice::SetGenericPhyRxEndOkCallback(GenericPhyRxEndOkCallback c)
 {
     NS_LOG_FUNCTION(this);
     m_phyMacRxEndOkCallback = c;
 }
 
 void 
 CognitivePhyDevice::SetGenericPhyTxAbortCallback(GenericPhyTxEndCallback c)
 {
    NS_LOG_FUNCTION(this);
    m_phyMacTxAbortCallback = c ;
 }

 Ptr<Object>
 CognitivePhyDevice::GetAntenna() const
 {
     NS_LOG_FUNCTION(this);
     return m_antenna;
 }
  
 void
 CognitivePhyDevice::SetAntenna(Ptr<AntennaModel> a)
 {
     NS_LOG_FUNCTION(this << a);
     m_antenna = a;
 }
  
  
 bool
 CognitivePhyDevice::StartTx(Ptr<Packet> p)
 {
     NS_LOG_FUNCTION(this << p);
     NS_LOG_LOGIC(this << "state: " << m_state);
     m_phyTxStartTrace(p);
     switch (m_state)
     {
     case RX:
        break;
     case SENS:
        break;
     case IDLE: {
         if(!m_phyEnergyTxStartCallback.IsNull())
         {
            m_phyEnergyTxStartCallback();
         }
        m_txPacket = p;
         ChangeState(TX);
         Ptr<HalfDuplexIdealPhySignalParameters> txParams =
             Create<HalfDuplexIdealPhySignalParameters>();
         Time txTimeSeconds = m_rate.CalculateBytesTxTime(p->GetSize());
         txParams->duration = txTimeSeconds;
         txParams->txPhy = GetObject<SpectrumPhy>();
         txParams->txAntenna = m_antenna;
         txParams->psd = m_txPsd;
         txParams->data = m_txPacket;
  
         NS_LOG_LOGIC(this << " tx power: " << 10 * std::log10(Integral(*(txParams->psd))) + 30
                           << " dBm");
         m_channel->StartTx(txParams);
         m_process.Cancel();
         m_process = Simulator::Schedule(txTimeSeconds, &CognitivePhyDevice::EndTx, this);
     }
     break;
  
     case TX:
         return true;
     }
     return false;
 }
  
 void
 CognitivePhyDevice::ChangeState(State s)
 {
    m_state = s ;
 }

 void
 CognitivePhyDevice::EndTx()
 {
     NS_LOG_FUNCTION(this);
     NS_LOG_LOGIC(this << " state: " << m_state);
  
     NS_ASSERT(m_state == TX);
  
     m_phyTxEndTrace(m_txPacket);
  
     if (!m_phyMacTxEndCallback.IsNull())
     {
         m_phyMacTxEndCallback(m_txPacket);
     }
     if(!m_phyEnergyTxEndCallback.IsNull())
     {
        m_phyEnergyTxEndCallback();
     }
     m_process.Cancel();
     m_txPacket = nullptr;
     ChangeState(IDLE);
 }

 void
 CognitivePhyDevice::AbortTx()
 {
    NS_ASSERT(m_state==TX);
    //std::cout << m_netDevice->GetNode()->GetId() << " aborting Tx " << std::endl;
    if(!m_phyMacTxAbortCallback.IsNull())
    {
        m_phyMacTxAbortCallback(m_txPacket);
    }
    if(!m_phyEnergyTxEndCallback.IsNull())
    {
        m_phyEnergyTxEndCallback();
    }
    m_process.Cancel();
    ChangeState(IDLE);
 }
  
 void
 CognitivePhyDevice::StartRx(Ptr<SpectrumSignalParameters> spectrumParams)
 {
     NS_LOG_FUNCTION(this << spectrumParams);
     NS_LOG_LOGIC(this << " state: " << m_state);
     NS_LOG_LOGIC(this << " rx power: " << 10 * std::log10(Integral(*(spectrumParams->psd))) + 30
                       << " dBm");
     // interference will happen regardless of the state of the receiver
     Ptr<SpectrumValue> temp = Create<SpectrumValue>(m_localModel);
    
     m_interference.AddSignal(spectrumParams->psd, spectrumParams->duration);
     
     for(uint16_t i = 0 ; i < m_numBins ; i++)
     {
        (*temp)[i] = (*spectrumParams->psd)[i+m_channelIndex*m_numBins];
     }
     
     if(Integral(*temp)==0.0)
     {
        return;
     }
    // std::cout << CarrierSense(1000) << std::endl;

     
    // std::cout << CarrierSense(1000) << std::endl;

     // the device might start RX only if the signal is of a type understood by this device
     // this corresponds in real devices to preamble detection
     Ptr<HalfDuplexIdealPhySignalParameters> rxParams =
         DynamicCast<HalfDuplexIdealPhySignalParameters>(spectrumParams);
     if (rxParams)
     {
         // signal is of known type
         if(m_state==TX)
         {
            AbortTx();
         }
         if(m_state==IDLE)
         {
            if(!m_phyEnergyRxStartCallback.IsNull())
            {
                m_phyEnergyRxStartCallback();
            }
            Ptr<Packet> p = rxParams->data;
             m_phyRxStartTrace(p);
             m_rxPacket = p;
             m_rxPsd = rxParams->psd;
             ChangeState(RX);
             if (!m_phyMacRxStartCallback.IsNull())
             {
                 NS_LOG_LOGIC(this << " calling m_phyMacRxStartCallback");
                 m_phyMacRxStartCallback();
             }
             else
             {
                 NS_LOG_LOGIC(this << " m_phyMacRxStartCallback is NULL");
             }
             m_interference.StartRx(p, rxParams->psd);
             NS_LOG_LOGIC(this << " scheduling EndRx with delay " << rxParams->duration); 
             m_process.Cancel();
             Simulator::Schedule(rxParams->duration, &CognitivePhyDevice::EndRx, this);
         }
     }
     else // rxParams == 0
     {
         NS_LOG_LOGIC(this << " signal of unknown type");
     }
  
     NS_LOG_LOGIC(this << " state: " << m_state);
}
  
 void
 CognitivePhyDevice::AbortRx()
 {
     NS_LOG_FUNCTION(this);
     NS_LOG_LOGIC(this << "state: " << m_state);
     NS_ASSERT(m_state == RX);
     m_interference.AbortRx();
     m_phyRxAbortTrace(m_rxPacket);
     m_process.Cancel();
     m_rxPacket = nullptr;
    if(!m_phyEnergyRxEndCallback.IsNull())
    {
        m_phyEnergyRxEndCallback();
    }
     ChangeState(IDLE);
 }
  
 void
 CognitivePhyDevice::EndRx()
 {
 //   std::cout << m_netDevice->GetNode()->GetId() <<" Rx has ended " << Simulator::Now() << std::endl;
     NS_LOG_FUNCTION(this);
     NS_LOG_LOGIC(this << " state: " << m_state);
     NS_ASSERT(m_state == RX);
     bool rxOk = m_interference.EndRx();
    if(!m_phyEnergyRxEndCallback.IsNull())
    {
        m_phyEnergyRxEndCallback();
    }

     if (rxOk)
     {
         m_phyRxEndOkTrace(m_rxPacket);
         if (!m_phyMacRxEndOkCallback.IsNull())
         {
             NS_LOG_LOGIC(this << " calling m_phyMacRxEndOkCallback");
             m_phyMacRxEndOkCallback(m_rxPacket);
         }
         else
         {
             NS_LOG_LOGIC(this << " m_phyMacRxEndOkCallback is NULL");
         }
     }
     else
     {
         m_phyRxEndErrorTrace(m_rxPacket);
         if (!m_phyMacRxEndErrorCallback.IsNull())
         {
             NS_LOG_LOGIC(this << " calling m_phyMacRxEndErrorCallback");
             m_phyMacRxEndErrorCallback();
         }
         else
         {
             NS_LOG_LOGIC(this << " m_phyMacRxEndErrorCallback is NULL");
         }
     }
     m_process.Cancel();
     ChangeState(IDLE);
     m_rxPacket = nullptr;
     m_rxPsd = nullptr;
 }

 Ptr<const SpectrumValue>
 CognitivePhyDevice::GetTxSpectrumValue()
 {
    return m_txPsd;
 }

 void
 CognitivePhyDevice::SetChannelsInfo(uint16_t numOfChannels , uint16_t numBins , uint16_t Index)
 {
    m_numOfChannels = numOfChannels;
    m_numBins = numBins;
    m_channelIndex = Index; 
 }

 double
 CognitivePhyDevice::CarrierSense(uint16_t Index)
 {
    if(Index==1000){Index = m_channelIndex;}
  //  std::cout << Index << std::endl;
    Ptr<const SpectrumValue> spec = m_interference.GetSpectrum();
    Ptr<SpectrumValue> model = Create<SpectrumValue>(m_localModel);
    for(uint16_t i = 0 ; i < m_numBins ; i++)
    {
        (*model)[i]= (*spec)[i+Index*m_numBins];
    } 
    return Integral(*model);
 }

 void
 CognitivePhyDevice::SetChannelIndex(uint16_t Index)
 {
    if(Index!=m_channelIndex)
    {
        for(uint16_t i = 0 ; i < m_numBins ; i++)
        {
            std::swap((*m_txPsd)[i+Index*m_numBins],(*m_txPsd)[i+m_channelIndex*m_numBins]);
        }
        m_channelIndex = Index; 
    }
 }
 
 void 
 CognitivePhyDevice::SetGenericPhyEnergyTxStartCallback(GenericEnergyNotification c)
 {
    m_phyEnergyTxStartCallback = c ;
 }

 void 
 CognitivePhyDevice::SetGenericPhyEnergyTxEndCallback(GenericEnergyNotification c)
 {
    m_phyEnergyTxEndCallback = c ;
 }

 void 
 CognitivePhyDevice::SetGenericPhyEnergyRxStartCallback(GenericEnergyNotification c)
 {
    m_phyEnergyRxStartCallback = c ;
 }

 void 
 CognitivePhyDevice::SetGenericPhyEnergyRxEndCallback(GenericEnergyNotification c)
 {
    m_phyEnergyRxEndCallback = c ;
 }

 void
 CognitivePhyDevice::SetCognitiveDeviceEnergyModel(Ptr<CognitiveRadioEnergyModel> model)
 {
    NS_ASSERT(model);
    m_energyModel = model ;
 }

 void
 CognitivePhyDevice::StartSensing()
 {
   // std::cout << m_netDevice->GetNode()->GetId() << " going to start sensing "<< m_state <<" " << Simulator::Now() << std::endl;
    if(m_state==TX)
    {
        AbortTx();
    }
 }

 double
 CognitivePhyDevice::GetRemainingEnergy()
 {
    NS_ASSERT_MSG(m_energyModel,"the energy model isn't set");
    return m_energyModel->GetRemainingEnergy();
 }

 } // namespace ns3
 
