/*
 * Copyright (c) 2025 Telecommunications Lab, Higher Institiute for Applied Sciences and Technology , Damascus.
 *
 *
 * Author: Rida Takla <raidotakla@gmail.com>
 */
#include "mac-frames.h"

#include <cmath>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("MacDcfFrame");
NS_OBJECT_ENSURE_REGISTERED(MacDcfFrame);

MacDcfFrame::MacDcfFrame()
    : m_packet(nullptr)
{

}

MacDcfFrame::~MacDcfFrame()
{

}

TypeId

MacDcfFrame::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::MacDcfFrame")
            .SetParent<Object>()
            .SetGroupName("Network")
            .AddConstructor<MacDcfFrame>();
    return tid;
}

void

MacDcfFrame::DoDispose()
{
    m_packet=nullptr;
    Object::DoDispose();
}

void

MacDcfFrame::SetPacket(Ptr<Packet> packet)
{
    m_packet = packet;
}

void

MacDcfFrame::SetSender(const Mac48Address sender)
{
    m_TxAddress = sender;
}

void 

MacDcfFrame::SetReceiver(const Mac48Address receiver)
{
    m_RxAddress = receiver;
}

void

MacDcfFrame::SetDuration(const Time duration)
{
    m_duration = duration;
}

void

MacDcfFrame::SetKind(FrameType typ)
{
    m_kind = typ;
}

Ptr<Packet>

MacDcfFrame::GetPacket()
{
    return m_packet;
}

Mac48Address

MacDcfFrame::GetSender()
{
    return m_TxAddress;
}

Mac48Address

MacDcfFrame::GetReceiver()
{
    return m_RxAddress;
}

Time

MacDcfFrame::GetDuration()
{
    return m_duration;
}

FrameType

MacDcfFrame::GetKind()
{
    return m_kind;
}

Time

MacDcfFrame::CalculateLatency()
{
    return m_arrivalTime - m_creationTime; 
}

void

MacDcfFrame::SetCreationTime(const Time time)
{
    m_creationTime = time;   
}

void 

MacDcfFrame::SetArrivalTime(const Time time)
{
    m_arrivalTime = time;
}

}