/*
 * Copyright (c) 2025 Telecommunications Lab, Higher Institiute for Applied Sciences and Technology , Damascus.
 *
 *
 * Author: Rida Takla <raidotakla@gmail.com>
 */

#include "cognitive-routing-unite.h"

#include <ns3/simulator.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("CognitiveRoutingUnite");
  
NS_OBJECT_ENSURE_REGISTERED(CognitiveRoutingUnite);

CognitiveRoutingUnite::CognitiveRoutingUnite():
    m_IhaveCluster(false),
    m_routingEnabled(false),
    m_ImClusterHead(false),
    m_ImGateway(false),
    m_ExpiracyTime(Seconds(2))
{
    m_vector = new std::vector<Ptr<MacDcfFrame>>();
}

CognitiveRoutingUnite::~CognitiveRoutingUnite()
{
    m_vector->clear();
    m_vector = nullptr;
}

TypeId

CognitiveRoutingUnite::GetTypeId()
{
    static TypeId tid = 
        TypeId("ns3::CognitiveRoutingUnite")
            .SetParent<Object>()
            .SetGroupName("Network")
            .AddConstructor<CognitiveRoutingUnite>();
    return tid;
}

void 

CognitiveRoutingUnite::GetSimulationResults()
{
    std::cout << "Total Number of Generated Packets " << SentPackets << '\n';
    std::cout << "Total Number of Received Packets " << ReceivedPackets << '\n';
    std::cout << "Total Delay " << TotDelay << '\n';
    std::cout << "Total Number of Sent Routing Bytes " << RoutingBytes << '\n';
    std::cout << "Total Number of Sent Control Bytes " << ControlBytes << '\n';
}

void

CognitiveRoutingUnite::DoDispose()
{
    m_vector = nullptr; 
    Object::DoDispose();
}

void

CognitiveRoutingUnite::SendPacket(Ptr<Packet> packet, const Address source, const Address dest, uint16_t protocolNumber)
{
  //  std::cout << source << " " << dest << '\n';
    Ptr<MacDcfFrame> frame = CreateObject<MacDcfFrame>();
    frame->SetPacket(packet);
    frame->SetOriginalSender(Mac48Address::ConvertFrom(source));
    frame->SetOriginalReceiver(Mac48Address::ConvertFrom(dest));
    frame->SetCurrentSender(Mac48Address::ConvertFrom(source));
    frame->SetProtocolNumber(protocolNumber);
    frame->SetCreationTime(Simulator::Now());
    frame->SetOriginalPacketUid(packet->GetUid());
    frame->SetDuration(m_dataRate.CalculateBytesTxTime(packet->GetUid()));

    if(protocolNumber==1000)
    {
        frame->SetCurrentReceiver(Mac48Address::ConvertFrom(dest));
        ControlBytes+=packet->GetSize();
        this->SendFrame(frame);
    }
    else
    {
        SentPackets++;
        if(m_routingTable.count(dest))
        {
            frame->SetCurrentReceiver(Mac48Address::ConvertFrom(m_routingTable[dest]));
            this->SendFrame(frame);
        }
        else
        {
            m_vector->push_back(frame);
            if(!m_requiredAddresses.count(dest))
            {
                Simulator::ScheduleNow(&CognitiveRoutingUnite::StartRouteDiscovery,this,dest);\
            }
        }
    }
}

void

CognitiveRoutingUnite::ReceiveFrame(Ptr<MacDcfFrame> frame)
{
    Address orignalReceiver = Mac48Address::ConvertFrom(frame->GetOriginalReceiver());
    Address currentReceiver = Mac48Address::ConvertFrom(frame->GetCurrentReceiver());
    
    if(frame->GetProtocolNumber()==1000)
    {
        if(currentReceiver==m_address || currentReceiver==Broadcast)
        {
            NS_ASSERT_MSG(!m_ctrlAppSendPacketCallback.IsNull(),"the send packet for the ctrl app isn't set");
            m_ctrlAppSendPacketCallback(frame->GetPacket());
        }
        else
        {
            return;
        }
    }
    else if(frame->GetProtocolNumber()==989)
    {   
        Ptr<MacDcfFrame> routFrame = msgs[frame->GetPacket()->GetUid()];
        if(currentReceiver==m_address || currentReceiver==Broadcast)
        {
            if(routFrame->GetMsgType()==RReq)
            {
                Simulator::ScheduleNow(&CognitiveRoutingUnite::ReceiveRouteDiscoveryRequest,this,routFrame);
            }
            else if(routFrame->GetMsgType()==RRep)
            {
                Simulator::ScheduleNow(&CognitiveRoutingUnite::ReceiveRouteReply,this,routFrame);
            }
        }
        else
        {
            return;
        }
    }
    else
    {
        //std::cout << m_address << " I have received a packet " << Simulator::Now() << '\n';;
        //std::cout << orignalReceiver << ' ' << frame->GetOriginalSender() << '\n'; 
        if(orignalReceiver==m_address)
        {
            ReceivedPackets++;
            TotDelay+= frame->CalculateLatency();
            return;
        }
        else
        {
            if(currentReceiver!=m_address)
            {
                return;
            }
            else
            {
                if(m_routingEnabled)
                {
                    if(m_routingTable.count(orignalReceiver))
                    {
                        frame->SetCurrentReceiver(Mac48Address::ConvertFrom(m_routingTable[orignalReceiver]));
                        frame->SetCurrentSender(Mac48Address::ConvertFrom(m_address));
                        //std::cout << frame->GetCurrentReceiver() << ' ' << frame->GetCurrentSender() << '\n';
                        m_dataFrameCallback(frame);
                    }
                    else
                    {
                        m_vector->push_back(frame);
                        if(!m_requiredAddresses.count(orignalReceiver))
                        {
                            StartRouteDiscovery(orignalReceiver);
                        }
                    }
                }
                else
                {
                    return;
                }
                
            }
        }
    }   

}

void

CognitiveRoutingUnite::SendFrame(Ptr<MacDcfFrame> frame)
{
    uint16_t protocolNumber = frame->GetProtocolNumber();
    if(protocolNumber==1000 || protocolNumber==989)
    {
        NS_ASSERT_MSG(!m_ctrlFrameCallback.IsNull(),"the control send frame callback isn't set");
        if(protocolNumber==1000)
        {
            ControlBytes+=frame->GetPacket()->GetSize();
        }
        else
        {
            RoutingBytes+=frame->GetPacket()->GetSize();
        }
        m_ctrlFrameCallback(frame);
    }
    else
    {
        NS_ASSERT_MSG(!m_dataFrameCallback.IsNull(),"the data send frame callback isn't set");
        m_dataFrameCallback(frame);
    }
}

void

CognitiveRoutingUnite::SetCluster(Address address)
{
    m_CHaddress = address;
    m_IhaveCluster = true;
}

void

CognitiveRoutingUnite::EnableRouting(bool b)
{
    m_routingEnabled = b ; 
}

void

CognitiveRoutingUnite::UnSetCluster()
{
    m_IhaveCluster = false;
    m_ImClusterHead = false;
    m_ImGateway = false;
}

void

CognitiveRoutingUnite::SetDataRate(const DataRate dataRate)
{
    m_dataRate = dataRate;
}

uint32_t 

CognitiveRoutingUnite::NumSendPackets()
{
    return SentPackets;
}

uint32_t

CognitiveRoutingUnite::NumReceivedPackets()
{
    return ReceivedPackets;
}

void

CognitiveRoutingUnite::SetAddress(const Address address)
{
    m_address = address;
}

void

CognitiveRoutingUnite::SetSendDataFrameCallback(SendFrameCallback c)
{
    m_dataFrameCallback = c;
}

void

CognitiveRoutingUnite::SetSendCtrlFrameCallback(SendFrameCallback c)
{
    m_ctrlFrameCallback = c;
}

void

CognitiveRoutingUnite::SetCtrlAppSendPacketCallback(SendPacketCallback c)
{
    m_ctrlAppSendPacketCallback = c;
}

void

CognitiveRoutingUnite::SetIsClusterMemberCallback(IsClusterMemberCallback c)
{
    m_IsClusterMemberCallback = c;
}

void

CognitiveRoutingUnite::SetClusterHeadStatus(bool b)
{
    m_ImClusterHead = b;
}

void 

CognitiveRoutingUnite::SetGatewayStatus(bool b)
{
    m_ImGateway = b ;
}

void 

CognitiveRoutingUnite::StartRouteDiscovery(Address address)
{
    if(!m_routingEnabled)
    {
        return ;
    }
    std::cout << m_address << " I'm going to start route discovery \n";  
    
    Ptr<Packet> pkt = Create<Packet>(RReqSize);
    Ptr<MacDcfFrame> msg = CreateObject<MacDcfFrame>();
    msgs[pkt->GetUid()] = msg; 
    msg->SetRequiredAddress(address);
    m_requiredAddresses.insert(address);
    msg->SetMsgType(RoutingMsgType::RReq);
    msg->SetCurrentSender(Mac48Address::ConvertFrom(m_address));
    msg->SetOriginalSender(Mac48Address::ConvertFrom(m_address));
    msg->SetPacket(pkt);
    msg->SetProtocolNumber(routingProtocol);
    
    std::cout << " going to send ";
    if(m_CHaddress!=m_address)
    {
        std::cout << " to my cluster head \n";
        msg->SetOriginalReceiver(Mac48Address::ConvertFrom(m_CHaddress));
        msg->SetCurrentReceiver(Mac48Address::ConvertFrom(m_CHaddress));
    }
    else
    {
        std::cout << " Broadcast \n";
        msg->SetOriginalReceiver(Mac48Address::ConvertFrom(Broadcast));
        msg->SetCurrentReceiver(Mac48Address::ConvertFrom(Broadcast));
    }
    this->SendFrame(msg);
}

void

CognitiveRoutingUnite::ReceiveRouteDiscoveryRequest(Ptr<MacDcfFrame> frame)
{  
    Address des = frame->GetRequiredAddress();
    Address src = Mac48Address::ConvertFrom(frame->GetOriginalSender());
    
    if(m_address==des)
    {
        std::cout << m_address << " I have received a route discovery request sent from " << src << " to " << des << '\n';
        std::cout << " I'm in the node \n";
        Ptr<Packet> pkt = Create<Packet>(RRepSize);
        Ptr<MacDcfFrame> msg = CreateObject<MacDcfFrame>();
        msgs[pkt->GetUid()] = msg ;
        msg->SetOriginalSender(Mac48Address::ConvertFrom(m_address));
        msg->SetCurrentSender(Mac48Address::ConvertFrom(m_address));
        msg->SetOriginalReceiver(frame->GetOriginalSender());
        msg->SetCurrentReceiver(frame->GetOriginalSender());
        msg->SetRequiredAddress(des);
        msg->SetPacket(pkt);
        msg->SetMsgType(RRep);
        msg->SetDelay(0.0);
        msg->SetProtocolNumber(routingProtocol);
        this->SendFrame(msg);

        return;
    }
    if(m_pendingReq.count(std::make_pair(src,des)))
    {
        //std::cout << "Hshh\n";
        return;
    }
   // std::cout << m_address << ' ' << m_ImClusterHead << '\n';
    if(m_ImClusterHead || m_ImGateway)
    {
       m_pendingReq.insert(std::make_pair(src,des));
        //std::cout << m_requiredAddresses.size() << '\n';
        if(!m_requiredAddresses.count(des))
        {
            std::cout << m_address << " I have received a route discovery request sent from " << src << " to " << des << '\n';
            std::cout << "I'm not in the node " << Simulator::Now() << '\n';
            m_requiredAddresses.insert(des);
            Ptr<Packet> pkt = Create<Packet>(RReqSize);
            Ptr<MacDcfFrame> msg = CreateObject<MacDcfFrame>();
            msgs[pkt->GetUid()] = msg;
            msg->SetRequiredAddress(des);
            msg->SetMsgType(RoutingMsgType::RReq);
            msg->SetPacket(pkt);
            msg->SetOriginalSender(Mac48Address::ConvertFrom(m_address));
            msg->SetCurrentSender(Mac48Address::ConvertFrom(m_address));
            msg->SetOriginalReceiver(Mac48Address::ConvertFrom(Broadcast));
            msg->SetCurrentReceiver(Mac48Address::ConvertFrom(Broadcast));
            msg->SetProtocolNumber(routingProtocol);
            this->SendFrame(msg);
            Simulator::Schedule(m_ExpiracyTime,&CognitiveRoutingUnite::DeleteRequest,this,src,des);
        }
        else
        {
            return;
        }
    }

    else
    {
        return;
    }
}

void

CognitiveRoutingUnite::ReceiveRouteReply(Ptr<MacDcfFrame> frame)
{
    
    Address des = frame->GetRequiredAddress();
    double delay = CalculateLinkDelay() + frame->GetDelay();
    if(!m_minDelay.count(des))
    {
        m_minDelay[des] = delay ;
        m_routingTable[des] = Mac48Address::ConvertFrom(frame->GetCurrentSender());
        m_timers[des].Cancel();
        m_timers[des] = Simulator::Schedule(m_ExpiracyTime*5,&CognitiveRoutingUnite::DeleteNode,this,des);
    }
    else
    {
        if(m_minDelay[des] > delay)
        {
            m_minDelay[des] = delay;
            m_routingTable[des] = Mac48Address::ConvertFrom(frame->GetCurrentSender());
            m_timers[des].Cancel();
            m_timers[des] = Simulator::Schedule(m_ExpiracyTime*5,&CognitiveRoutingUnite::DeleteNode,this,des);
        }
    }
    std::cout << m_address << ' ' << " I have received a route reply " << Simulator::Now() << '\n';
    SendPendingPackets();
    for(auto& [i,j] : m_pendingReq)
    {
        if(j==des)
        {
            Ptr<Packet> pkt = Create<Packet> (RRepSize);
            Ptr<MacDcfFrame> msg = CreateObject<MacDcfFrame>();
            msgs[pkt->GetUid()] = msg;
            msg->SetPacket(pkt);
            msg->SetMsgType(RoutingMsgType::RRep);
            msg->SetRequiredAddress(des);
            msg->SetOriginalSender(Mac48Address::ConvertFrom(m_address));
            msg->SetCurrentSender(Mac48Address::ConvertFrom(m_address));
            msg->SetOriginalReceiver(Mac48Address::ConvertFrom(i)); 
            msg->SetCurrentReceiver(Mac48Address::ConvertFrom(i));
            msg->SetDelay(delay);
            msg->SetProtocolNumber(routingProtocol);
            this->SendFrame(msg);   
        }
    }
}

void

CognitiveRoutingUnite::SendRouteError(Address address)
{
    Ptr<Packet> pkt = Create<Packet>(RErrSize);
    Ptr<MacDcfFrame> msg = CreateObject<MacDcfFrame>();
    msgs[pkt->GetUid()] = msg ;
    msg->SetPacket(pkt);
    msg->SetOriginalSender(Mac48Address::ConvertFrom(m_address));
    msg->SetCurrentSender(Mac48Address::ConvertFrom(m_address));
    msg->SetOriginalReceiver(Mac48Address::ConvertFrom(address));
    msg->SetCurrentReceiver(Mac48Address::ConvertFrom(address));
    msg->SetMsgType(RoutingMsgType::RErr);
    this->SendFrame(msg);
}

void

CognitiveRoutingUnite::ReceiveRouteError(Ptr<MacDcfFrame> fra)
{
    Ptr<MacDcfFrame> frame = msgs[fra->GetPacket()->GetUid()];
    if(m_address==frame->GetRequiredAddress())
    {
        StartRouteDiscovery(frame->GetRequiredAddress());
    }
    else
    {
        Address prevHop = m_routingTable[frame->GetRequiredAddress()];
        this->SendRouteError(prevHop);
    }
}

void 

CognitiveRoutingUnite::SendPendingPackets()
{
    int j = 0 ; 
    std::vector<Ptr<MacDcfFrame>> vect;
    for(auto& frame : *m_vector)
    {
        Address des = Mac48Address::ConvertFrom(frame->GetOriginalReceiver());
        if(m_routingTable.count(des))
        {
            j++;
            frame->SetCurrentReceiver(Mac48Address::ConvertFrom(m_routingTable[des]));
            m_dataFrameCallback(frame);
        }
        else
        {
            vect.push_back(frame);
        }
    }
    *m_vector = vect ;
    std::cout << m_address << " this is my routing table " << '\n'; 
    for(auto& i : m_routingTable)
    {
        std::cout << i .first << ' ' << i.second << '\n';
    }
}

double

CognitiveRoutingUnite::CalculateLinkDelay()
{
    return 0.0;
}

void 

CognitiveRoutingUnite::DeleteRequest(Address a , Address b)
{
    m_pendingReq.erase(std::make_pair(a,b));
    m_requiredAddresses.erase(b);
}

void 

CognitiveRoutingUnite::DeleteNode(Address address)
{
    m_routingTable.erase(address);
}

}