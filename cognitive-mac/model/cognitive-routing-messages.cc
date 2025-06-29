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

CognitiveRoutingMessage::CognitiveRoutingMessage() :
    TotDelay(0.0)
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

CognitiveRoutingMessage::SetMsgType(RoutingMsgType type)
{
    m_type = type;
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

void

CognitiveRoutingMessage::SetDelay(double d)
{
    TotDelay = d; 
}

double

CognitiveRoutingMessage::GetDelay()
{
    return TotDelay;
}

}