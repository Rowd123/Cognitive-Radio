/*
 * Copyright (c) 2025 Telecommunications Lab, Higher Institiute for Applied Sciences and Technology , Damascus.
 *
 *
 * Author: Rida Takla <raidotakla@gmail.com>
 */
#include "cognitive-general-net-device.h"
#include <ns3/boolean.h>
#include <ns3/channel.h>
#include <ns3/enum.h>
#include <ns3/llc-snap-header.h>
#include <ns3/log.h>
#include <ns3/pointer.h>
#include <ns3/queue.h>
#include <ns3/simulator.h>
#include <ns3/trace-source-accessor.h>
#include <ns3/uinteger.h>



namespace ns3
{

NS_LOG_COMPONENT_DEFINE("CognitiveGeneralNetDevice");
  
NS_OBJECT_ENSURE_REGISTERED(CognitiveGeneralNetDevice);

 std::ostream&
 operator<<(std::ostream& os, CognitiveState s)
 {
     switch (s)
     {
     case IDLE:
         os << "IDLE";
         break;
     case RX:
         os << "RX";
         break;
     case TX:
         os << "TX";
         break;
     case SENSING:
         os << "SENSING";
         break;
     default:
         os << "UNKNOWN";
         break;
     }
     return os;
 }

TypeId

CognitiveGeneralNetDevice::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::CognitiveGeneralNetDevice")
            .SetParent<NetDevice>()
            .SetGroupName("Spectrum")
            .AddConstructor<CognitiveGeneralNetDevice>()
            .AddAttribute("Address",
                          "The MAC address of this device.",
                          Mac48AddressValue(Mac48Address("12:34:56:78:90:12")),
                          MakeMac48AddressAccessor(&CognitiveGeneralNetDevice::m_address),
                          MakeMac48AddressChecker())
            .AddAttribute(
                "Mtu",
                "The Maximum Transmission Unit",
                UintegerValue(1500),
                MakeUintegerAccessor(&CognitiveGeneralNetDevice::SetMtu, &CognitiveGeneralNetDevice::GetMtu),
                MakeUintegerChecker<uint16_t>(1, 65535))
            .AddAttribute(
                "Phy",
                "The PHY layer attached to this device.",
                PointerValue(),
                MakePointerAccessor(&CognitiveGeneralNetDevice::GetPhy, &CognitiveGeneralNetDevice::SetPhy),
                MakePointerChecker<Object>())
            .AddTraceSource("MacTx",
                            "Trace source indicating a packet has arrived "
                            "for transmission by this device",
                            MakeTraceSourceAccessor(&CognitiveGeneralNetDevice::m_MacTxTrace),
                            "ns3::Packet::TracedCallback")
            .AddTraceSource("MacTxDrop",
                            "Trace source indicating a packet has been dropped "
                            "by the device before transmission",
                            MakeTraceSourceAccessor(&CognitiveGeneralNetDevice::m_MacTxDropTrace),
                            "ns3::Packet::TracedCallback")
            .AddTraceSource("MacPromiscRx",
                            "A packet has been received by this device, has been "
                            "passed up from the physical layer "
                            "and is being forwarded up the local protocol stack.  "
                            "This is a promiscuous trace,",
                            MakeTraceSourceAccessor(&CognitiveGeneralNetDevice::m_MacPromiscRxTrace),
                            "ns3::Packet::TracedCallback")
            .AddTraceSource("MacRx",
                            "A packet has been received by this device, "
                            "has been passed up from the physical layer "
                            "and is being forwarded up the local protocol stack.  "
                            "This is a non-promiscuous trace,",
                            MakeTraceSourceAccessor(&CognitiveGeneralNetDevice::m_MacRxTrace),
                            "ns3::Packet::TracedCallback");
    return tid;
}

CognitiveGeneralNetDevice::CognitiveGeneralNetDevice()
    : m_state(IDLE),
      m_IhaveChannel(false),
      m_CW(4),
      m_threshold(0.0),
      m_currentTX(false),
      m_backoff(false),
      m_dataDevice(false)
{
    m_rv = CreateObject<UniformRandomVariable>();
    NS_LOG_FUNCTION(this);
}

CognitiveGeneralNetDevice::~CognitiveGeneralNetDevice()
{
    NS_LOG_FUNCTION(this);
    m_queue = nullptr;
}

void

CognitiveGeneralNetDevice::DoDispose()
{
    NS_LOG_FUNCTION(this);
    m_node = nullptr;
    m_channel = nullptr;
    m_data = nullptr;
    m_queue=nullptr;
    m_phy = nullptr;
    m_phyTxStartCallback = MakeNullCallback<bool, Ptr<Packet>>();
    m_sensingResult = MakeNullCallback<double,uint16_t>();
    NetDevice::DoDispose();
}

void

CognitiveGeneralNetDevice::SetIfIndex(const uint32_t index)
{
    NS_LOG_FUNCTION(index);
    m_ifIndex = index;
}

uint32_t

CognitiveGeneralNetDevice::GetIfIndex() const
{
    NS_LOG_FUNCTION(this);
    return m_ifIndex;
}

bool

CognitiveGeneralNetDevice::SetMtu(uint16_t mtu)
{
    NS_LOG_FUNCTION(mtu);
    m_mtu = mtu;
    return true;
}

uint16_t

CognitiveGeneralNetDevice::GetMtu() const
{
    NS_LOG_FUNCTION(this);
    return m_mtu;
}

void

CognitiveGeneralNetDevice::SetQueue(std::queue<Ptr<MacDcfFrame>> *que)
{
    m_queue = que;
}

void

CognitiveGeneralNetDevice::SetAddress(Address address)
{
    NS_LOG_FUNCTION(this);
    m_address = Mac48Address::ConvertFrom(address);
}

Address

CognitiveGeneralNetDevice::GetAddress() const
{
    NS_LOG_FUNCTION(this);
    return m_address;
}

bool

CognitiveGeneralNetDevice::IsBroadcast() const
{
    NS_LOG_FUNCTION(this);
    return true;
}

Address

CognitiveGeneralNetDevice::GetBroadcast() const
{
    NS_LOG_FUNCTION(this);
    return Mac48Address::GetBroadcast();
}

bool

CognitiveGeneralNetDevice::IsMulticast() const
{
    NS_LOG_FUNCTION(this);
    return true;
}

Address

CognitiveGeneralNetDevice::GetMulticast(Ipv4Address addr) const
{
    NS_LOG_FUNCTION(addr);
    Mac48Address ad = Mac48Address::GetMulticast(addr);
    return ad;
}

Address

CognitiveGeneralNetDevice::GetMulticast(Ipv6Address addr) const
{
    NS_LOG_FUNCTION(addr);
    Mac48Address ad = Mac48Address::GetMulticast(addr);
    return ad;
}

bool

CognitiveGeneralNetDevice::IsPointToPoint() const
{
    NS_LOG_FUNCTION(this);
    return false;
}

bool

CognitiveGeneralNetDevice::IsBridge() const
{
    NS_LOG_FUNCTION(this);
    return false;
}

Ptr<Node>

CognitiveGeneralNetDevice::GetNode() const
{
    NS_LOG_FUNCTION(this);
    return m_node;
}

void

CognitiveGeneralNetDevice::SetNode(Ptr<Node> node)
{
    NS_LOG_FUNCTION(node);

    m_node = node;
}

void

CognitiveGeneralNetDevice::SetPhy(Ptr<Object> phy)
{
    NS_LOG_FUNCTION(this << phy);
    m_phy = phy;
}

Ptr<Object>

CognitiveGeneralNetDevice::GetPhy() const
{
    NS_LOG_FUNCTION(this);
    return m_phy;
}

void

CognitiveGeneralNetDevice::SetChannel(Ptr<Channel> c)
{
    NS_LOG_FUNCTION(this << c);
    m_channel = c;
}

Ptr<Channel>

CognitiveGeneralNetDevice::GetChannel() const
{
    NS_LOG_FUNCTION(this);
    return m_channel;
}

bool

CognitiveGeneralNetDevice::NeedsArp() const
{
    NS_LOG_FUNCTION(this);
    return true;
}

bool

CognitiveGeneralNetDevice::IsLinkUp() const
{
    NS_LOG_FUNCTION(this);
    return m_linkUp;
}

void

CognitiveGeneralNetDevice::AddLinkChangeCallback(Callback<void> callback)
{
    NS_LOG_FUNCTION(&callback);
    m_linkChangeCallbacks.ConnectWithoutContext(callback);
}

void

CognitiveGeneralNetDevice::SetReceiveCallback(NetDevice::ReceiveCallback cb)
{
    NS_LOG_FUNCTION(&cb);
    m_rxCallback = cb;
}

void

CognitiveGeneralNetDevice::SetPromiscReceiveCallback(NetDevice::PromiscReceiveCallback cb)
{
    NS_LOG_FUNCTION(&cb);
    m_promiscRxCallback = cb;
}

bool

CognitiveGeneralNetDevice::SupportsSendFrom() const
{
    NS_LOG_FUNCTION(this);
    return true;
}

void

CognitiveGeneralNetDevice::SetDeviceKind(bool b)
{
    m_dataDevice = b ;
}

void

CognitiveGeneralNetDevice::DIFSPhase()
{ 
    m_senseRes = 0.0 ;
    NS_ASSERT(!m_sensingResult.IsNull());
    double tem = m_sensingResult(1000);
    m_senseRes += tem * DIFS.GetSeconds();
    m_sendPhase.Cancel();   
    m_sendPhase = Simulator::Schedule(DIFS,&CognitiveGeneralNetDevice::DIFSDecision,this);
}

void 

CognitiveGeneralNetDevice::DIFSDecision()
{
    if(m_senseRes<=m_threshold*DIFS.GetSeconds())
    {
        m_sendPhase.Cancel();
        Time addRandom = Seconds(m_rv->GetValue(1e-6,1e-5));
        if(m_data->GetCurrentReceiver()==Mac48Address::ConvertFrom(Broadcast))
        {

            m_sendPhase = Simulator::Schedule(SIFS+addRandom,&CognitiveGeneralNetDevice::TransmitData,this);
        }
        else
        {
            m_sendPhase = Simulator::Schedule(SIFS+addRandom,&CognitiveGeneralNetDevice::SendRTS,this);
        }
    }
    else
    {
        m_rv->SetAttribute("Min",DoubleValue(0.0));
        m_rv->SetAttribute("Max",DoubleValue((1<<m_CW)-1));
        m_backOffSlots = m_rv->GetInteger() + 1;
        m_sendPhase.Cancel();
        m_sendPhase = Simulator::Schedule(DIFS,&CognitiveGeneralNetDevice::BackOffPhase,this);
        m_backoff = true ;
        //std::cout << m_node->GetId() <<" going to BackOff " << m_backOffSlots << std::endl;
    }
}

void 

CognitiveGeneralNetDevice::SendRTS()
{
    NS_ASSERT_MSG(m_data,"This Net Device doesn't have a data packet");
    Ptr<Packet> rtsPacket = Create<Packet>(RTSsize);
    Time duration = m_rate.CalculateBytesTxTime(ACKsize + CTSsize) + 3*SIFS + m_data->GetDuration() + margin ;
    Ptr<MacDcfFrame> rts = CreateObject<MacDcfFrame>();  
    rts->SetPacket(rtsPacket);
    rts->SetCurrentSender(m_data->GetCurrentSender());
    rts->SetCurrentReceiver(m_data->GetCurrentReceiver());
    rts->SetDuration(duration);
    rts->SetKind(FrameType::RTS);
    rts->SetOriginalPacketUid(m_data->GetPacket()->GetUid());
    m_map[rtsPacket->GetUid()] = rts; 
    m_sendPhase.Cancel(); 
    m_sendPhase = Simulator::ScheduleNow(&CognitiveGeneralNetDevice::StartTransmission,this,rtsPacket);
}

void

CognitiveGeneralNetDevice::ReceiveRTS()
{
    m_sendPhase.Cancel();
    m_sendPhase = Simulator::Schedule(SIFS,&CognitiveGeneralNetDevice::SendCTS,this);
}

void

CognitiveGeneralNetDevice::SendCTS()
{
    Ptr<Packet> ctsPacket = Create<Packet>(CTSsize);
    Time duration = m_rate.CalculateBytesTxTime(ACKsize) + 2*SIFS + m_rdata->GetDuration() ;
    Ptr<MacDcfFrame> cts = CreateObject<MacDcfFrame>();
    cts->SetPacket(ctsPacket);
    cts->SetCurrentReceiver(m_rdata->GetCurrentSender());
    cts->SetCurrentSender(m_rdata->GetCurrentReceiver());
    cts->SetOriginalPacketUid(m_rdata->GetOriginalPacketUid());
    cts->SetDuration(duration + margin);
    cts->SetKind(FrameType::CTS);
    m_map[ctsPacket->GetUid()] = cts;
    //std::cout << m_node->GetId() << " going to send CTS frame " << Simulator::Now() << std::endl;
    m_sendPhase.Cancel();
    m_sendPhase = Simulator::ScheduleNow(&CognitiveGeneralNetDevice::StartTransmission,this,ctsPacket);
}

void 

CognitiveGeneralNetDevice::ReceiveCTS()
{
    m_sendPhase.Cancel();
    m_sendPhase = Simulator::Schedule(SIFS,&CognitiveGeneralNetDevice::TransmitData,this);
}

void 

CognitiveGeneralNetDevice::TransmitData()
{
    NS_ASSERT_MSG(m_data,"This Net Device doesn't have an address");
    //std::cout << m_node->GetId() << std::endl;
    Time duration = m_rate.CalculateBytesTxTime(ACKsize) + SIFS;
    m_data->SetDuration(duration + margin);
    m_data->SetKind(FrameType::DATA);
    m_map[m_data->GetPacket()->GetUid()] = m_data;
    m_sendPhase.Cancel();
    m_sendPhase = Simulator::ScheduleNow(&CognitiveGeneralNetDevice::StartTransmission,this,m_data->GetPacket());
}

void 

CognitiveGeneralNetDevice::ReceiveData()

{
    m_rdata->SetArrivalTime(Simulator::Now());
    latency+=m_rdata->CalculateLatency().GetSeconds();
    m_sendPhase.Cancel();
    if(m_rdata->GetCurrentReceiver()==Mac48Address::ConvertFrom(Broadcast))
    {
        NS_ASSERT_MSG(m_routingUnite,"the Network layer unit isn't set");
        m_routingUnite->ReceiveFrame(m_rdata);
        m_sendPhase = Simulator::Schedule(SIFS,&CognitiveGeneralNetDevice::ContinueTransmission,this);
    }
    else
    {
      /*  if(!m_receiveCtrlPacket.IsNull())
        {
            m_receiveCtrlPacket(m_rdata->GetPacket());
        }
     `*/
        m_sendPhase = Simulator::Schedule(SIFS,&CognitiveGeneralNetDevice::SendAck,this);
    }
}

void 

CognitiveGeneralNetDevice::SendAck()
{
    Ptr<Packet> ackPacket = Create<Packet>(ACKsize);
    Ptr<MacDcfFrame> ack = CreateObject<MacDcfFrame>();
    ack->SetPacket(ackPacket);
    ack->SetCurrentReceiver(m_rdata->GetCurrentSender());
    ack->SetCurrentSender(m_rdata->GetCurrentReceiver());
    ack->SetDuration(Seconds(0.0));
    ack->SetKind(FrameType::ACK);
    ack->SetOriginalPacketUid(m_rdata->GetOriginalPacketUid());
    m_map[ackPacket->GetUid()] = ack; 
    m_sendPhase.Cancel();
    m_sendPhase = Simulator::ScheduleNow(&CognitiveGeneralNetDevice::StartTransmission,this,ackPacket);
    NS_ASSERT_MSG(m_routingUnite,"the Network layer unit isn't set");
    m_routingUnite->ReceiveFrame(m_rdata);
}

void 

CognitiveGeneralNetDevice::ReceiveAck()
{
   // std::cout << "I have received an ack " << m_data->GetPacket()->GetUid() << '\n'; 
    m_currentTX = false;
    recPackets+= m_dataSet.count(m_rdata->GetOriginalPacketUid());
    m_sendPhase.Cancel();
    m_sendPhase = Simulator::ScheduleNow(&CognitiveGeneralNetDevice::ContinueTransmission,this);
    
}

void

CognitiveGeneralNetDevice::BackOffPhase()
{
    //std::cout << m_node->GetId() << " going to backoff " << m_backOffSlots << std::endl; 
    if(m_backOffSlots)
    {
        m_senseRes = 0.0 ;
        NS_ASSERT(!m_sensingResult.IsNull());
        double tem = m_sensingResult(1000);
        m_senseRes += tem * SLOT.GetSeconds();
        m_sendPhase.Cancel();
        m_sendPhase = Simulator::Schedule(SLOT,&CognitiveGeneralNetDevice::DoBackOffProcedure,this);
    }
    else
    {
        m_backoff = false;
        m_sendPhase.Cancel();
        m_sendPhase = Simulator::Schedule(SIFS,&CognitiveGeneralNetDevice::SendRTS,this);
    }
}

void

CognitiveGeneralNetDevice::DoBackOffProcedure()

{
    if(m_senseRes<=m_threshold*SLOT.GetSeconds())
    {
        m_backOffSlots--;
    }
    m_sendPhase.Cancel();
    m_sendPhase = Simulator::Schedule(SLOT,&CognitiveGeneralNetDevice::BackOffPhase,this);
}

bool

CognitiveGeneralNetDevice::Send(Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber)
{
    NS_LOG_FUNCTION(packet << dest << protocolNumber);
    if(protocolNumber!=1000)
    {
        m_dataSet.insert(packet->GetUid());
    }
    // std::cout << "going to send  " << m_node->GetId() << " " << dest << '\n';
    return SendFrom(packet, m_address, dest, protocolNumber);
}

bool

CognitiveGeneralNetDevice::SendFrom(Ptr<Packet> packet,
                           const Address& src,
                           const Address& dest,
                           uint16_t protocolNumber)
{
    NS_LOG_FUNCTION(packet << src << dest << protocolNumber);
    m_routingUnite->SendPacket(packet,src,dest,protocolNumber);
    //std::cout<< m_node->GetId() << " " << Simulator::Now() << " " << packet->GetUid() << std::endl;
    return true;
}

void 

CognitiveGeneralNetDevice::SendFrame(Ptr<MacDcfFrame> frame)
{
    // If the device is idle, transmission starts immediately. Otherwise,
    // the transmission will be started by NotifyTransmissionEnd
    //
    NS_LOG_LOGIC(this << " state=" << m_state);
    if (m_queue->empty() && !m_sendPhase.IsPending() && !m_currentTX && m_state!=RX && m_IhaveChannel)
    {
        NS_LOG_LOGIC("new packet is head of queue, starting TX immediately");
        m_currentTX = true;
        m_data = frame;
        m_sendPhase.Cancel();
        m_sendPhase = Simulator::ScheduleNow(&CognitiveGeneralNetDevice::DIFSPhase,this);
    }
    else
    {
       // std::cout << m_node->GetId()<< " enqueueing new packet " << packet->GetUid() << '\n';
        NS_LOG_LOGIC("enqueueing new packet");
        if (!m_queue->emplace(frame))
        {
                m_MacTxDropTrace(frame->GetPacket());
        }
    }
}

void

CognitiveGeneralNetDevice::SetPhyStartTxCallback(PhyTxStartCallback c)
{
    NS_LOG_FUNCTION(this);
    m_phyTxStartCallback = c;
}

void

CognitiveGeneralNetDevice::SetCcaResultCallback(SensingResultCallback c)
{
    NS_LOG_FUNCTION(this);
    m_sensingResult = c;
}

void

CognitiveGeneralNetDevice::SetGetRemainingEnergyCallback(GetRemainingEnergyCallback c)
{
    NS_LOG_FUNCTION(this);
    m_remainingEnergy = c; 
}

void

CognitiveGeneralNetDevice::SetCurrentDataChannelCallback(CurrentDataChannelCallback c)
{
    NS_LOG_FUNCTION(this);
    m_currentDataChannel = c;
}

void

CognitiveGeneralNetDevice::SetRoutingUnite(Ptr<CognitiveRoutingUnite> routingUnite)
{
    m_routingUnite = routingUnite;
}

void

CognitiveGeneralNetDevice::StartTransmission(Ptr<Packet> packet)
{  
    NS_LOG_FUNCTION(this);

    NS_ASSERT(packet);
    NS_ASSERT_MSG(m_state == IDLE,m_node->GetId());
    if(!m_IhaveChannel)
    {
        return ;
    }
    if (m_phyTxStartCallback(packet))
    {
        NS_LOG_WARN("PHY refused to start TX");
    }
    else
    {
        ChangeState(TX);
    }
}

void

CognitiveGeneralNetDevice::NotifyPartialTransmissionEnd(Ptr<const Packet>)
{
    NS_LOG_FUNCTION(this);
    NS_ASSERT_MSG(m_state == TX, "TX end notified while state != TX");
    ChangeState(IDLE);
    if(m_data)
    {
        if(m_data->GetCurrentReceiver()==Mac48Address::ConvertFrom(Broadcast))
        {
            m_currentTX = false;
            m_sendPhase.Cancel();
            m_sendPhase = Simulator::ScheduleNow(&CognitiveGeneralNetDevice::ContinueTransmission,this);
        }
    }
}

void 

CognitiveGeneralNetDevice::ContinueTransmission()
{
  //  std::cout<< m_node->GetId() <<" We are going to continue transmission " << Simulator::Now() ;
    NS_LOG_FUNCTION(this);
    ChangeState(IDLE);
    NS_ASSERT(m_queue);
    if(!m_IhaveChannel){return;}
   // std::cout << m_node->GetId() << " I'm going to continue transmission ";
    if(m_currentTX)
    {
       // std::cout << " I have a packet currently " << '\n';
        m_sendPhase.Cancel();
        if(m_backoff)
        {
            m_sendPhase = Simulator::ScheduleNow(&CognitiveGeneralNetDevice::DoBackOffProcedure,this);
        }
        else
        {
            m_sendPhase = Simulator::ScheduleNow(&CognitiveGeneralNetDevice::DIFSPhase,this);
        }
    }
    else
    {
      //  std::cout << " I don't have a packet currently \n";
        if (!m_queue->empty())
        {
            Ptr<MacDcfFrame> data = m_queue->front();
            m_queue->pop();
            NS_ASSERT(data);
            m_data = data;
            m_currentTX = true;
            NS_LOG_LOGIC("scheduling transmission now");
            m_sendPhase.Cancel();
            m_sendPhase = Simulator::ScheduleNow(&CognitiveGeneralNetDevice::DIFSPhase,this);
        }
        else
        {
        }
    }
}

void

CognitiveGeneralNetDevice::NotifyReceptionStart()
{
    NS_LOG_FUNCTION(this);
    NS_ASSERT_MSG(m_state!=TX,"the reception started while while transmitting");
    //std::cout << m_node->GetId() << " We have started reception " << Simulator::Now() << std::endl; 
    m_sendPhase.Cancel();
    ChangeState(RX);
}

void 

CognitiveGeneralNetDevice::NotifyTransmissionAbort(Ptr<const Packet>)
{
    NS_LOG_FUNCTION(this);
    if(m_state==SENSING){return;}
    NS_ASSERT_MSG(m_state==TX,"called to abort transmitting while not transmitting");
   // std::cout << m_node->GetId() << "has stopped transmission " << Simulator::Now() << std::endl;
    m_sendPhase.Cancel();
    ChangeState(IDLE);
}

void

CognitiveGeneralNetDevice::NotifyReceptionEndError()
{
    //std::cout << m_node->GetId() << " Reception has finished in Error " << Simulator::Now() << std::endl;
    NS_LOG_FUNCTION(this);
    m_sendPhase.Cancel();
    m_sendPhase = Simulator::ScheduleNow(&CognitiveGeneralNetDevice::ContinueTransmission,this);
}

void

CognitiveGeneralNetDevice::NotifyReceptionEndOk(Ptr<Packet> packet)
{
    NS_LOG_FUNCTION(this << packet);
    if(!m_map.count(packet->GetUid())){return;}
    ChangeState(IDLE);
    
    m_rdata = m_map[packet->GetUid()];

    m_paddress = m_rdata->GetCurrentSender();
  /*  std::cout << m_node->GetId() << " I have received packet "  << m_rdata->GetPacket()->GetUid() << " sent from "
     << m_rdata->GetSender() << " to " << m_rdata->GetReceiver() << "\n"; 
    */
    if(m_rdata->GetCurrentReceiver()!=m_address && m_rdata->GetCurrentReceiver()!=Mac48Address::ConvertFrom(Broadcast))
    { 
        m_sendPhase.Cancel();
        
        m_sendPhase = Simulator::Schedule(m_rdata->GetDuration(),&CognitiveGeneralNetDevice::ContinueTransmission,this);
        
        return ;
    }

    FrameType typ = m_rdata->GetKind();
    if (typ==FrameType::RTS)
    {
        //std::cout << "I have received an RTS" << std::endl;
        m_sendPhase = Simulator::ScheduleNow(&CognitiveGeneralNetDevice::ReceiveRTS,this); 
    }
    else if (typ==FrameType::CTS)
    {
        //std::cout << "I have received a CTS" << std::endl;
        m_sendPhase = Simulator::ScheduleNow(&CognitiveGeneralNetDevice::ReceiveCTS,this);
    }
    else if (typ==FrameType::ACK)
    {
        //std::cout << "I have received an ACK" << std::endl;
        m_sendPhase = Simulator::ScheduleNow(&CognitiveGeneralNetDevice::ReceiveAck,this);
    }
    else if (typ==FrameType::DATA)
    {
        //std::cout << "I have received a DATA" << std::endl;
        m_sendPhase = Simulator::ScheduleNow(&CognitiveGeneralNetDevice::ReceiveData,this);
    }
    
    m_MacRxTrace(packet);
     
}

void

CognitiveGeneralNetDevice::SetPhyRate(const DataRate rate)
{
    m_rate = rate;
}

uint32_t

CognitiveGeneralNetDevice::NumOfGenPackets()
{
    return genPackets;
}

uint32_t

CognitiveGeneralNetDevice::NumOfRecPackets()
{
    return recPackets;
}

double

CognitiveGeneralNetDevice::TotalLatency()
{
    return latency;
}

double 

CognitiveGeneralNetDevice::SpectrumControlSense(uint16_t Index)
{
    NS_ASSERT_MSG(!m_sensingResult.IsNull(),"sensing isn't linked between phy and mac");
    return m_sensingResult(Index);
}

void

CognitiveGeneralNetDevice::SetThreshold(double threshold)
{
    m_threshold = threshold;
}

void

CognitiveGeneralNetDevice::StopWork(Time stopTime)
{
    ChangeState(SENSING);
    m_sendPhase.Cancel();
    m_sendPhase = Simulator::Schedule(stopTime,&CognitiveGeneralNetDevice::ContinueTransmission,this);
    NS_ASSERT_MSG(!m_stopWork.IsNull(),"you haven't set the StopWork Callback for the PHY");
    m_stopWork();
}

void

CognitiveGeneralNetDevice::SetStopWorkCallback(StopWorkCallback c)
{
    m_stopWork = c;
}

void

CognitiveGeneralNetDevice::ChangeState(CognitiveState s)
{
    m_state = s;  
}

double

CognitiveGeneralNetDevice::GetReamainingEnergy()
{
    NS_ASSERT_MSG(!m_remainingEnergy.IsNull(),"you haven't set the get energy callback between phy and mac");
    return m_remainingEnergy();
}

void

CognitiveGeneralNetDevice::SetClusterInfo(uint16_t CADC , uint16_t CBDC, Address CHaddress)
{
    NS_LOG_FUNCTION(this);
    m_CADC = CADC ;
    m_CBDC = CBDC ;
    m_IhaveChannel = true;
    m_CHaddress = CHaddress;
    if(!(*m_queue).empty())
    {
        std::queue<Ptr<MacDcfFrame>> *temQueue = new std::queue<Ptr<MacDcfFrame>>();
        while(!(*m_queue).empty())
        {
            Ptr<MacDcfFrame> tem = (*m_queue).front();
            (*m_queue).pop();
            tem->SetCurrentReceiver(Mac48Address::ConvertFrom(m_CHaddress)); 
            (*temQueue).push(tem);
        }
        m_queue = temQueue;
    }
    if(!m_currentDataChannel.IsNull())
    {
        m_currentDataChannel(m_CADC);
        m_sendPhase.Cancel();
        m_sendPhase = Simulator::ScheduleNow(&CognitiveGeneralNetDevice::ContinueTransmission,this);
    }
}

void

CognitiveGeneralNetDevice::TransmissionPermission(bool b)
{
    m_IhaveChannel = b ;
}


}