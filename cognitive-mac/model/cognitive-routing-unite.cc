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
    m_routingEnabled(false)
{

}

CognitiveRoutingUnite::~CognitiveRoutingUnite()
{

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
    Object::DoDispose();
}

void

CognitiveRoutingUnite::SendPacket(Ptr<Packet> packet, const Address source, const Address dest, uint16_t protocolNumber)
{

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
            NS_ASSERT_MSG(m_ctrlAppSendPacketCallback.IsNull(),"the send packet for the ctrl app isn't set");
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
                    return;
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

}