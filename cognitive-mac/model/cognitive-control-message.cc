/*
 * Copyright (c) 2025 Telecommunications Lab, Higher Institiute for Applied Sciences and Technology , Damascus.
 *
 *
 * Author: Rida Takla <raidotakla@gmail.com>
 */
#include "cognitive-control-message.h"

#include <cmath>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("CognitiveControlMessage");
NS_OBJECT_ENSURE_REGISTERED(CognitiveControlMessage);

CognitiveControlMessage::CognitiveControlMessage()
    : m_pkt(nullptr),
      m_channelQuality(nullptr),
      m_neighborNodeConnectivty(nullptr),
      m_neighborClusterReachability(nullptr),
      m_energy(0.0)
{

}

CognitiveControlMessage::~CognitiveControlMessage()
{

}

TypeId

CognitiveControlMessage::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::CognitiveControlMessage")
            .SetParent<Object>()
            .SetGroupName("Network")
            .AddConstructor<CognitiveControlMessage>();
    return tid;
}

void

CognitiveControlMessage::DoDispose()
{
    m_pkt=nullptr;
    m_channelQuality=nullptr;
    m_neighborClusterReachability=nullptr;
    m_neighborNodeConnectivty=nullptr;
    Object::DoDispose();
}

void

CognitiveControlMessage::SetPacket(Ptr<Packet> pkt)
{
    m_pkt = pkt;
}

Ptr<Packet> 

CognitiveControlMessage::GetPacket()
{
    return m_pkt;
}

void

CognitiveControlMessage::SetSourceAddress(const Address address)
{
    m_sourceAddress = address ;
}

const Address

CognitiveControlMessage::GetSourceAddress()
{
    return m_sourceAddress;
}

void 

CognitiveControlMessage::SetDestinationAddress(const Address address)
{
    m_destinAddress = address;
}

const Address

CognitiveControlMessage::GetDestinationAddress()
{
    return m_destinAddress;
}

void

CognitiveControlMessage::SetChannelQualityMap(ChannelQuality mp)
{
    m_channelQuality = mp;
}

ChannelQuality

CognitiveControlMessage::GetChannelQualityMap()
{
    return m_channelQuality;
}

void

CognitiveControlMessage::SetNeighborNodeConnectivityMap(NeighborNodeConnectivity mp)
{
    m_neighborNodeConnectivty = mp;
}

NeighborNodeConnectivity

CognitiveControlMessage::GetNeighborNodeConnectivityMap()
{
    return m_neighborNodeConnectivty;
}

void 

CognitiveControlMessage::SetNeighborClusterReachabilityMap(NeighborClusterReachability mp)
{
    m_neighborClusterReachability = mp;
}

NeighborClusterReachability

CognitiveControlMessage::GetNeighborClusterReachabilityMap()
{
    return m_neighborClusterReachability;
}

void 

CognitiveControlMessage::SetCreationTime(Time time)
{
    m_creationTime = time;
}

Time 

CognitiveControlMessage::GetCreationTime()
{
    return m_creationTime;  
}

void 

CognitiveControlMessage::SetEnergy(double energy)
{
    m_energy = energy; 
}

double

CognitiveControlMessage::GetEnergy()
{
    return m_energy;
}

void

CognitiveControlMessage::SetKind(Kind kind)
{
    m_kind = kind;
}

CognitiveControlMessage::Kind

CognitiveControlMessage::GetKind()
{
    return m_kind;
}

void 

CognitiveControlMessage::SetCADC(uint16_t channel)
{
    m_CADC = channel;
}

uint16_t

CognitiveControlMessage::GetCADC()
{
    return m_CADC;
}

void 

CognitiveControlMessage::SetCBDC(uint16_t channel)
{
    m_CBDC = channel;
}

uint16_t

CognitiveControlMessage::GetCBDC()
{
    return m_CBDC;
}

void

CognitiveControlMessage::SetNodeId(uint32_t ID)
{
    m_nodeId = ID;
}

uint32_t 

CognitiveControlMessage::GetNodeId()
{
    return m_nodeId;
}


}