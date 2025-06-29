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
    m_ImGateway(false)
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
    }
    else
    {
        SentPackets++;
        if(m_IhaveCluster)
        {
            frame->SetCurrentReceiver(Mac48Address::ConvertFrom(m_CHaddress));
        }
    }
    this->SendFrame(frame);
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
    else
    {
        if(orignalReceiver==m_address)
        {
            ReceivedPackets++;
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
                        m_dataFrameCallback(frame);
                    }
                    else
                    {
                        m_vector->push_back(frame);
                        StartRouteDiscovery(orignalReceiver);
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
    if(frame->GetProtocolNumber()==1000)
    {
        NS_ASSERT_MSG(!m_ctrlFrameCallback.IsNull(),"the control send frame callback isn't set");
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
    Ptr<Packet> pkt = Create<Packet>(RReqSize);
    Ptr<CognitiveRoutingMessage> msg = CreateObject<CognitiveRoutingMessage>();
    msgs[pkt->GetUid()] = msg; 
    msg->SetRequiredAddress(address);
    msg->SetMsgType(RoutingMsgType::RReq);
    msg->SetCurrentSender(Mac48Address::ConvertFrom(m_address));
    msg->SetOriginalSender(Mac48Address::ConvertFrom(m_address));
    msg->SetPacket(pkt);
    m_pendingReq.insert(address); 
    if(m_CHaddress!=m_address)
    {
        msg->SetOriginalReceiver(Mac48Address::ConvertFrom(m_CHaddress));
        msg->SetCurrentReceiver(Mac48Address::ConvertFrom(m_CHaddress));
    }
    else
    {
        msg->SetOriginalReceiver(Mac48Address::ConvertFrom(Broadcast));
        msg->SetCurrentReceiver(Mac48Address::ConvertFrom(Broadcast));
    }
    m_ctrlFrameCallback(msg);
}

void

CognitiveRoutingUnite::ReceiveRouteDiscoveryRequest(Ptr<CognitiveRoutingMessage> frame)
{
    if(m_ImClusterHead || m_ImGateway)
    {
        Address des = frame->GetRequiredAddress();
        bool IsCM = m_IsClusterMemberCallback(des);
        if(m_ImClusterHead && IsCM)
        {
            double delay = CalculateLinkDelay();
            Ptr<Packet> pkt = Create<Packet> (RRepSize);
            Ptr<CognitiveRoutingMessage> msg = CreateObject<CognitiveRoutingMessage>();
            msgs[pkt->GetSize()] = msg;
            msg->SetOriginalSender(Mac48Address::ConvertFrom(m_address));
            msg->SetCurrentSender(Mac48Address::ConvertFrom(m_address));
            msg->SetOriginalReceiver(Mac48Address::ConvertFrom(Broadcast)); 
            msg->SetCurrentReceiver(Mac48Address::ConvertFrom(Broadcast));
            msg->SetRequiredAddress(des);
            msg->SetMsgType(RoutingMsgType::RRep);
            msg->SetDelay(delay);
            m_pendingReq.erase(des);
            m_ctrlFrameCallback(msg);
        }
        else
        {
            Ptr<Packet> pkt = Create<Packet>(RReqSize);
            Ptr<CognitiveRoutingMessage> msg = CreateObject<CognitiveRoutingMessage>();
            msgs[pkt->GetUid()] = msg;
            msg->SetRequiredAddress(des);
            msg->SetMsgType(RoutingMsgType::RReq);
            msg->SetPacket(pkt);
            msg->SetOriginalSender(Mac48Address::ConvertFrom(m_address));
            msg->SetCurrentSender(Mac48Address::ConvertFrom(m_address));
            msg->SetOriginalReceiver(Mac48Address::ConvertFrom(Broadcast));
            msg->SetCurrentReceiver(Mac48Address::ConvertFrom(Broadcast));
            m_pendingReq.insert(des);
            m_ctrlFrameCallback(msg);
        }
    }
    else
    {
        return;
    }
}

void

CognitiveRoutingUnite::ReceiveRouteReply(Ptr<CognitiveRoutingMessage> frame)
{
    if(!m_pendingReq.count(frame->GetRequiredAddress()))
    {
        return ;
    }
    Ptr<Packet> pkt = Create<Packet>(RRepSize);
    Ptr<CognitiveRoutingMessage> msg = CreateObject<CognitiveRoutingMessage>();
    double delay = CalculateLinkDelay();
    Address des = frame->GetRequiredAddress();
    double delay = CalculateLinkDelay() + frame->GetDelay();
    if(!m_minDelay.count(des))
    {
        m_minDelay[des] = delay ;
        m_routingTable[des] = Mac48Address::ConvertFrom(frame->GetCurrentSender());
    }
    else
    {
        if(m_minDelay[des] > delay)
        {
            m_minDelay[des] = delay;
            m_routingTable[des] = Mac48Address::ConvertFrom(frame->GetCurrentSender());
        }
    }
    if(des==m_address)
    {
        SendPendingPackets();
        return ;
    }
    else
    {
        Ptr<Packet> pkt = Create<Packet> (RRepSize);
        Ptr<CognitiveRoutingMessage> msg = CreateObject<CognitiveRoutingMessage>();
        msgs[pkt->GetSize()] = msg;
        msg->SetOriginalSender(Mac48Address::ConvertFrom(m_address));
        msg->SetCurrentSender(Mac48Address::ConvertFrom(m_address));
        msg->SetOriginalReceiver(Mac48Address::ConvertFrom(Broadcast)); 
        msg->SetCurrentReceiver(Mac48Address::ConvertFrom(Broadcast));
        msg->SetRequiredAddress(des);
        msg->SetMsgType(RoutingMsgType::RRep);
        msg->SetDelay(delay);
        m_pendingReq.erase(des);
        m_ctrlFrameCallback(msg);   
    }
}

void

CognitiveRoutingUnite::SendRouteError(Address address)
{
    Ptr<Packet> pkt = Create<Packet>(RErrSize);
    Ptr<CognitiveRoutingMessage> msg = CreateObject<CognitiveRoutingMessage>();
    msgs[pkt->GetUid()] = msg ;
    msg->SetPacket(pkt);
    msg->SetOriginalSender(Mac48Address::ConvertFrom(m_address));
    msg->SetCurrentSender(Mac48Address::ConvertFrom(m_address));
    msg->SetOriginalReceiver(Mac48Address::ConvertFrom(address));
    msg->SetCurrentReceiver(Mac48Address::ConvertFrom(address));
    msg->SetMsgType(RoutingMsgType::RErr);
    m_ctrlFrameCallback(msg);
}

void

CognitiveRoutingUnite::ReceiveRouteError(Ptr<CognitiveRoutingMessage> frame)
{
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
    std::vector<Ptr<MacDcfFrame>> vect;
    for(auto& frame : *m_vector)
    {
        Address des = Mac48Address::ConvertFrom(frame->GetOriginalReceiver());
        if(m_routingTable.count(des))
        {
            frame->SetCurrentReceiver(Mac48Address::ConvertFrom(m_routingTable[des]));
            m_dataFrameCallback(frame);
        }
        else
        {
            vect.push_back(frame);
        }
    }
    *m_vector = vect ;
}

}