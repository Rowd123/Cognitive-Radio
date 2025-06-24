/*
 * Copyright (c) 2025 Telecommunications Lab, Higher Institiute for Applied Sciences and Technology , Damascus.
 *
 *
 * Author: Rida Takla <raidotakla@gmail.com>
 */

#include "cognitive-routing-messages.h"

#include <ns3/simulator.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("CognitiveRoutingMessage");
  
NS_OBJECT_ENSURE_REGISTERED(CognitiveRoutingMessage);

CognitiveRoutingMessage::CognitiveRoutingMessage()
{

}

CognitiveRoutingMessage::~CognitiveRoutingMessage()
{

}

TypeId

CognitiveRoutingMessage::GetTypeId()
{
    TypeId tid = 
        TypeId("ns3::CognitiveRoutingMessage")
                .SetParent<Object>()
                .SetGroupName("Network")
                .AddConstructor<CognitiveRoutingMessage>();
    return tid;
}

void 

CognitiveRoutingMessage::DoDispose()
{
    Object::DoDispose();
}

void 

CognitiveRoutingMessage::SetSourceAddress(Address address)
{
    m_fromAddress = address;
}

void 

CognitiveRoutingMessage::SetDestinationAddress(Address address)
{
    m_toAddress = address;
}

void 

CognitiveRoutingMessage::SetMsgType(RoutingMsgType type)
{
    m_type = type;
}

void 

CognitiveRoutingMessage::SetPacket(Ptr<Packet> pkt)
{
    m_pkt = pkt;
}

void 

CognitiveRoutingMessage::SetErrorLink(std::pair<Address,Address> link)
{
    m_errorLink = link;
}

void

CognitiveRoutingMessage::SetRequiredAddress(Address address)
{
    m_requiredAddress = address;
}



Address

CognitiveRoutingMessage::GetSourceAddress()
{
    return m_fromAddress;
}

Address

CognitiveRoutingMessage::GetDestinationAddress()
{
    return m_toAddress;
}

Address 

CognitiveRoutingMessage::GetRequiredAddress()
{
    return m_requiredAddress;
}

RoutingMsgType

CognitiveRoutingMessage::GetMsgType()
{
    return m_type;
}

std::pair<Address,Address>

CognitiveRoutingMessage::GetErrorLink()
{
    return m_errorLink;
}



}