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
    : m_packet(nullptr),m_totDelay(0.0)
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

MacDcfFrame::SetOriginalSender(const Mac48Address sender)
{
    m_TxAddress = sender;
}

void 

MacDcfFrame::SetOriginalReceiver(const Mac48Address receiver)
{
    m_RxAddress = receiver;
}

void

MacDcfFrame::SetCurrentSender(const Mac48Address curSender)
{
    m_curTxAddress = curSender;
}

void

MacDcfFrame::SetCurrentReceiver(const Mac48Address curReceiver)
{
    m_curRxAddress = curReceiver;
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

MacDcfFrame::GetOriginalSender()
{
    return m_TxAddress;
}

Mac48Address

MacDcfFrame::GetOriginalReceiver()
{
    return m_RxAddress;
}

Mac48Address

MacDcfFrame::GetCurrentSender()
{
    return m_curTxAddress;
}

Mac48Address

MacDcfFrame::GetCurrentReceiver()
{
    return m_curRxAddress;
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

uint32_t 

MacDcfFrame::GetOriginalPacketUid()
{
    return m_originalPkt;
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

void 

MacDcfFrame::SetOriginalPacketUid(uint32_t uid)
{
    m_originalPkt = uid;
}

void

MacDcfFrame::SetProtocolNumber(uint16_t protocolNumber)
{
    m_protocolNum = protocolNumber;
}

uint16_t

MacDcfFrame::GetProtocolNumber()
{
    return m_protocolNum;
}

void 

MacDcfFrame::SetMsgType(RoutingMsgType type)
{
    m_rtType = type;
}

void

MacDcfFrame::SetRequiredAddress(Address address)
{
    m_requiredAddress = address;
}

Address 

MacDcfFrame::GetRequiredAddress()
{
    return m_requiredAddress;
}

RoutingMsgType

MacDcfFrame::GetMsgType()
{
    return m_rtType;
}

void

MacDcfFrame::SetDelay(double d)
{
    m_totDelay = d; 
}

double

MacDcfFrame::GetDelay()
{
    return m_totDelay;
}

}