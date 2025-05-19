/*
 * Copyright (c) 2025 HIAST
 *
 * 
 *
 * Author: Rida Takla
 */


 
 #include "cognitive-radio-phy.h"
 
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
 
 using namespace ns3;
 
 namespace ns3
 {
  
 NS_LOG_COMPONENT_DEFINE("CognitiveRadioPhy");
  
 NS_OBJECT_ENSURE_REGISTERED(CognitiveRadioPhy);
  
 CognitiveRadioPhy::CognitiveRadioPhy()
     : m_mobility(nullptr),
       m_netDevice(nullptr),
       m_channel(nullptr),
       m_txPsd(nullptr),
       m_localSpectrum(nullptr),
       m_energyModel(nullptr),
       m_state(IDLE),
       m_ChannelIndex(0),
       m_availableChannels(12),
       m_threshold(5e-15),
       m_resense(MilliSeconds(100))
 {
     m_interference.SetErrorModel(CreateObject<ShannonSpectrumErrorModel>());
     m_channelStatus.assign(NUMBER_OF_CHANNELS,true);
     m_rd = CreateObject<UniformRandomVariable>();
     m_rd->SetAttribute("Min",DoubleValue(0.0));
     m_rd->SetAttribute("Max",DoubleValue(12.0));
 }
  
 CognitiveRadioPhy::~CognitiveRadioPhy()
 {
 }
  
 void
 CognitiveRadioPhy::DoDispose()
 {
     NS_LOG_FUNCTION(this);
     m_mobility = nullptr;
     m_netDevice = nullptr;
     m_channel = nullptr;
     m_txPsd = nullptr;
     m_energyModel = nullptr;
     m_rxPsd = nullptr;
     m_txPacket = nullptr;
     m_rxPacket = nullptr;
     m_localSpectrum = nullptr;
     m_phyMacTxEndCallback = MakeNullCallback<void, Ptr<const Packet>>();
     m_phyMacRxStartCallback = MakeNullCallback<void>();
     m_phyMacRxEndErrorCallback = MakeNullCallback<void>();
     m_phyMacRxEndOkCallback = MakeNullCallback<void, Ptr<Packet>>();
     m_phyEnergyTxEndCallback = MakeNullCallback<void>();
     m_phyEnergyRxStartCallback = MakeNullCallback<void>();
     m_phyEnergyRxEndErrorCallback = MakeNullCallback<void>();
     m_phyEnergyRxEndOkCallback = MakeNullCallback<void>();
     m_phyEnergyTxStartCallback = MakeNullCallback<void>();
     SpectrumPhy::DoDispose();
 }
  
 /**
  * @brief Output stream operator
  * @param os output stream
  * @param s the state to print
  * @return an output stream
  */
 std::ostream&
 operator<<(std::ostream& os, CognitiveRadioPhy::State s)
 {
     switch (s)
     {
     case CognitiveRadioPhy::IDLE:
         os << "IDLE";
         break;
     case CognitiveRadioPhy::RX:
         os << "RX";
         break;
     case CognitiveRadioPhy::TX:
         os << "TX";
         break;
     default:
         os << "UNKNOWN";
         break;
     }
     return os;
 }
  
 TypeId
 CognitiveRadioPhy::GetTypeId()
 {
     static TypeId tid =
         TypeId("ns3::CognitiveRadioPhy")
             .SetParent<SpectrumPhy>()
             .SetGroupName("Spectrum")
             .AddConstructor<CognitiveRadioPhy>()
             .AddAttribute(
                 "Rate",
                 "The PHY rate used by this device",
                 DataRateValue(DataRate("1Mbps")),
                 MakeDataRateAccessor(&CognitiveRadioPhy::SetRate, &CognitiveRadioPhy::GetRate),
                 MakeDataRateChecker())
             .AddTraceSource("TxStart",
                             "Trace fired when a new transmission is started",
                             MakeTraceSourceAccessor(&CognitiveRadioPhy::m_phyTxStartTrace),
                             "ns3::Packet::TracedCallback")
             .AddTraceSource("TxEnd",
                             "Trace fired when a previously started transmission is finished",
                             MakeTraceSourceAccessor(&CognitiveRadioPhy::m_phyTxEndTrace),
                             "ns3::Packet::TracedCallback")
             .AddTraceSource("RxStart",
                             "Trace fired when the start of a signal is detected",
                             MakeTraceSourceAccessor(&CognitiveRadioPhy::m_phyRxStartTrace),
                             "ns3::Packet::TracedCallback")
             .AddTraceSource("RxAbort",
                             "Trace fired when a previously started RX is aborted before time",
                             MakeTraceSourceAccessor(&CognitiveRadioPhy::m_phyRxAbortTrace),
                             "ns3::Packet::TracedCallback")
             .AddTraceSource("RxEndOk",
                             "Trace fired when a previously started RX terminates successfully",
                             MakeTraceSourceAccessor(&CognitiveRadioPhy::m_phyRxEndOkTrace),
                             "ns3::Packet::TracedCallback")
             .AddTraceSource("RxEndError",
                             "Trace fired when a previously started RX terminates with an error "
                             "(packet is corrupted)",
                             MakeTraceSourceAccessor(&CognitiveRadioPhy::m_phyRxEndErrorTrace),
                             "ns3::Packet::TracedCallback");
     return tid;
 }
  
 Ptr<NetDevice>
 CognitiveRadioPhy::GetDevice() const
 {
     NS_LOG_FUNCTION(this);
     return m_netDevice;
 }
  
 Ptr<MobilityModel>
 CognitiveRadioPhy::GetMobility() const
 {
     NS_LOG_FUNCTION(this);
     return m_mobility;
 }
  
 void
 CognitiveRadioPhy::SetDevice(Ptr<NetDevice> d)
 {
     NS_LOG_FUNCTION(this << d);
     m_netDevice = d;
 }
  
 void
 CognitiveRadioPhy::SetMobility(Ptr<MobilityModel> m)
 {
     NS_LOG_FUNCTION(this << m);
     m_mobility = m;
 }
  
 void
 CognitiveRadioPhy::SetChannel(Ptr<SpectrumChannel> c)
 {
     NS_LOG_FUNCTION(this << c);
     m_channel = c;
 }
  
 Ptr<const SpectrumModel>
 CognitiveRadioPhy::GetRxSpectrumModel() const
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
 CognitiveRadioPhy::SetTxPowerSpectralDensity(Ptr<SpectrumValue> txPsd , uint16_t index)
 {
     NS_LOG_FUNCTION(this << txPsd);
     NS_ASSERT(txPsd);
     m_txPsd = txPsd;
     m_ChannelIndex = index ;
     NS_LOG_INFO(*txPsd << *m_txPsd);
 }
  
 void
 CognitiveRadioPhy::SetNoisePowerSpectralDensity(Ptr<const SpectrumValue> noisePsd)
 {
     NS_LOG_FUNCTION(this << noisePsd);
     NS_ASSERT(noisePsd);
     m_interference.SetNoisePowerSpectralDensity(noisePsd);
 }

 void
 CognitiveRadioPhy::SetLocalSpectrum(Ptr<SpectrumValue> spectrum)
 {
    m_localSpectrum = spectrum ;
 }
  
 void
 CognitiveRadioPhy::SetRate(DataRate rate)
 {
     NS_LOG_FUNCTION(this << rate);
     m_rate = rate;
 }
  
 DataRate
 CognitiveRadioPhy::GetRate() const
 {
     NS_LOG_FUNCTION(this);
     return m_rate;
 }
  
 void
 CognitiveRadioPhy::SetGenericPhyTxEndCallback(GenericPhyTxEndCallback c)
 {
     NS_LOG_FUNCTION(this);
     m_phyMacTxEndCallback = c;
 }
 void
 CognitiveRadioPhy::SetGenericPhyEnergyTxEndCallback(GenericEnergyNotification c)
 {
    NS_LOG_FUNCTION(this);
    m_phyEnergyTxEndCallback = c;
 }
 void
 CognitiveRadioPhy::SetGenericPhyRxStartCallback(GenericPhyRxStartCallback c)
 {
     NS_LOG_FUNCTION(this);
     m_phyMacRxStartCallback = c;
 }
 void
 CognitiveRadioPhy::SetGenericPhyEnergyRxStartCallback(GenericEnergyNotification c)
 {
    NS_LOG_FUNCTION(this);
    m_phyEnergyRxStartCallback = c;
 }
 void
 CognitiveRadioPhy::SetGenericPhyRxEndErrorCallback(GenericPhyRxEndErrorCallback c)
 {
     NS_LOG_FUNCTION(this);
     m_phyMacRxEndErrorCallback = c;
 }
 void
 CognitiveRadioPhy::SetGenericPhyEnergyRxEndErrorCallback(GenericEnergyNotification c)
 {
     NS_LOG_FUNCTION(this);
     m_phyEnergyRxEndErrorCallback = c;
 }
 void
 CognitiveRadioPhy::SetGenericPhyRxEndOkCallback(GenericPhyRxEndOkCallback c)
 {
     NS_LOG_FUNCTION(this);
     m_phyMacRxEndOkCallback = c;
 }
 void
 CognitiveRadioPhy::SetGenericPhyEnergyRxEndOkCallback(GenericEnergyNotification c)
 {
     NS_LOG_FUNCTION(this);
     m_phyEnergyRxEndOkCallback = c;
 }
 void
 CognitiveRadioPhy::SetGenericPhyEnergyTxStartCallback(GenericEnergyNotification c)
 {
    NS_LOG_FUNCTION(this);
    m_phyEnergyTxStartCallback = c ;
 }
 Ptr<Object>
 CognitiveRadioPhy::GetAntenna() const
 {
     NS_LOG_FUNCTION(this);
     return m_antenna;
 }
  
 void
 CognitiveRadioPhy::SetAntenna(Ptr<AntennaModel> a)
 {
     NS_LOG_FUNCTION(this << a);
     m_antenna = a;
 }
  
 void
 CognitiveRadioPhy::ChangeState(State newState)
 {
     NS_LOG_LOGIC(this << " state: " << m_state << " -> " << newState);
     m_state = newState;
 }
  
 bool
 CognitiveRadioPhy::StartTx(Ptr<Packet> p)
 {
     NS_LOG_FUNCTION(this << p);
     NS_LOG_LOGIC(this << "state: " << m_state);
  
     m_phyTxStartTrace(p);
  
     switch (m_state)
     {
     case RX:
         AbortRx();
         // fall through
  
     case IDLE: {
         SenseSpectrum();
         uint16_t chosen = ChooseChannel();
         if(chosen != 10000)
         {
            SetChannelIndex(chosen);
         }
         else
         {
            Simulator::Schedule(m_resense, &CognitiveRadioPhy::StartTx,this,p);
            return false;
         }
         if(!m_phyEnergyTxStartCallback.IsNull()){

         }
         if(!m_phyEnergyTxStartCallback.IsNull())
         {
            m_phyEnergyTxStartCallback();
         }
         totpacket++;
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
         Simulator::Schedule(txTimeSeconds, &CognitiveRadioPhy::EndTx, this);
     }
     break;
  
     case TX:
         return true;
     }
     return false;
 }
  
 void
 CognitiveRadioPhy::EndTx()
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
     m_txPacket = nullptr;
     ChangeState(IDLE);
 }
  
 void
 CognitiveRadioPhy::StartRx(Ptr<SpectrumSignalParameters> spectrumParams)
 {
     NS_LOG_FUNCTION(this << spectrumParams);
     NS_LOG_LOGIC(this << " state: " << m_state);
     NS_LOG_LOGIC(this << " rx power: " << 10 * std::log10(Integral(*(spectrumParams->psd))) + 30
                       << " dBm");
  
     // interference will happen regardless of the state of the receiver
     m_interference.AddSignal(spectrumParams->psd, spectrumParams->duration);
  
     // the device might start RX only if the signal is of a type understood by this device
     // this corresponds in real devices to preamble detection
     Ptr<HalfDuplexIdealPhySignalParameters> rxParams =
         DynamicCast<HalfDuplexIdealPhySignalParameters>(spectrumParams);
     if (rxParams)
     {
         // signal is of known type
         switch (m_state)
         {
         case TX:
             // the PHY will not notice this incoming signal
  
         case RX:
             // we should check if we should re-sync on a new incoming signal and discard the old one
             // (somebody calls this the "capture" effect)
             // criteria considered to do might include the following:
             //  1) signal strength (e.g., as returned by rxPsd.Norm ())
             //  2) how much time has passed since previous RX attempt started
             // if re-sync (capture) is done, then we should call AbortRx ()
             break;
  
         case IDLE:
             // preamble detection and synchronization is supposed to be always successful.
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
             m_endRxEventId =
             Simulator::Schedule(rxParams->duration, &CognitiveRadioPhy::EndRx, this);
  
             break;
         }
     }
     else // rxParams == 0
     {
         NS_LOG_LOGIC(this << " signal of unknown type");
     }
  
     NS_LOG_LOGIC(this << " state: " << m_state);
 }
  
 void
 CognitiveRadioPhy::AbortRx()
 {
     NS_LOG_FUNCTION(this);
     NS_LOG_LOGIC(this << "state: " << m_state);
     if(!m_phyEnergyRxEndErrorCallback.IsNull())
     {
        m_phyEnergyRxEndErrorCallback();
     }
     NS_ASSERT(m_state == RX);
     m_interference.AbortRx();
     m_phyRxAbortTrace(m_rxPacket);
     m_endRxEventId.Cancel();
     m_rxPacket = nullptr;
     ChangeState(IDLE);
 }
  
 void
 CognitiveRadioPhy::EndRx()
 {
     NS_LOG_FUNCTION(this);
     NS_LOG_LOGIC(this << " state: " << m_state);
  
     NS_ASSERT(m_state == RX);
     bool rxOk = m_interference.EndRx();
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
         if(!m_phyEnergyRxEndOkCallback.IsNull())
         {
            m_phyEnergyRxEndOkCallback();
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
         if(!m_phyEnergyRxEndErrorCallback.IsNull())
         {
            m_phyEnergyRxEndErrorCallback();
         }
     }
  
     ChangeState(IDLE);
     m_rxPacket = nullptr;
     m_rxPsd = nullptr;
 }
 
 void
 CognitiveRadioPhy::SetChannelIndex(uint16_t index)
 {
     NS_ASSERT(m_txPsd);
     Ptr<SpectrumValue> temPsd = Create<SpectrumValue>(m_txPsd->GetSpectrumModel()) ;
     uint16_t numBins = m_localSpectrum->GetSpectrumModel()->GetNumBands()  ;
     for(uint16_t i = 0 ; i < numBins ;i++)
     {  
         (*temPsd)[index*numBins+i] = (*m_localSpectrum)[i];
     } 
     SetTxPowerSpectralDensity(temPsd,index);
 }  
 
 uint16_t
 CognitiveRadioPhy::ChooseChannel(){
     uint16_t newIndex = m_rd->GetInteger();
     if(m_availableChannels){
            newIndex %= m_availableChannels ;
            uint16_t pos = 10000 ; 
            for(uint i = 0 ;i < NUMBER_OF_CHANNELS ; i++){
                if(m_channelStatus[i])
                {
                    if(newIndex==0){pos = i ; break;}
                    else{newIndex--;}
                }
            }
            return pos;
     }
     else
     {
        return 10000;
     }
 }

 Ptr<const SpectrumValue>
 CognitiveRadioPhy::GetTxSpectrumValue()
 {
    return m_txPsd;
 }

 void
 CognitiveRadioPhy::SenseSpectrum()
 {
    m_availableChannels = 0 ;
    Ptr<const SpectrumValue> spec = m_interference.GetSpectrum();
    Ptr<SpectrumValue> loc = Create<SpectrumValue>(m_localSpectrum->GetSpectrumModel());
    uint16_t numBins = m_localSpectrum->GetSpectrumModel()->GetNumBands() ;
    for(uint16_t i = 0 ; i < NUMBER_OF_CHANNELS ;i++)
    {
        for(uint16_t j = 0 ; j < numBins ;j++)
        {
        (*loc)[j] = (*spec)[j+i*numBins];
        }
        double cur = Integral(*loc);
        bool var = (cur>=m_threshold);
        m_channelStatus[i] = !var;
        m_availableChannels+= (!var); 
    }
 }
 
 void
 CognitiveRadioPhy::SetThreshold(double threshold)
 {
    m_threshold = threshold ;
 }

 uint32_t
 CognitiveRadioPhy::NumPacketsSent()
 {
    return totpacket;
 }

 void
 CognitiveRadioPhy::SetCognitiveRadioEnergyModel(Ptr<CognitiveRadioEnergyModel> model)
 {
    m_energyModel = model;
 }
 } // namespace ns3
 
