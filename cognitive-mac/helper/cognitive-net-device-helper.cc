/*
 * Copyright (c) 2025 HIAST
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Nicola Baldo <nbaldo@cttc.es>
 *
 * Edited: Rida Takla <reda.takla@hiast.edu.sy>
 */
#include "cognitive-net-device-helper.h"
#include "ns3/cognitive-phy-device.h"
#include "ns3/cognitive-general-net-device.h"
#include "ns3/antenna-model.h"
#include "ns3/config.h"
#include "ns3/log.h"
#include "ns3/mac48-address.h"
#include "ns3/mobility-model.h"
#include "ns3/names.h"
#include "ns3/propagation-delay-model.h"
#include "ns3/simulator.h"
#include "ns3/spectrum-channel.h"
#include "ns3/spectrum-propagation-loss-model.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("CognitiveNetDeviceHelper");

CognitiveNetDeviceHelper::CognitiveNetDeviceHelper()
{
    m_phy.SetTypeId("ns3::CognitivePhyDevice");
    m_device.SetTypeId("ns3::CognitiveGeneralNetDevice");
    m_antenna.SetTypeId("ns3::IsotropicAntennaModel");
}

CognitiveNetDeviceHelper::~CognitiveNetDeviceHelper()
{
}

void

CognitiveNetDeviceHelper::SetChannel(Ptr<SpectrumChannel> channel)
{
    m_channel = channel;
}

void

CognitiveNetDeviceHelper::SetChannel(std::string channelName)
{
    Ptr<SpectrumChannel> channel = Names::Find<SpectrumChannel>(channelName);
    m_channel = channel;
}

void

CognitiveNetDeviceHelper::SetTxPowerSpectralDensity(Ptr<SpectrumValue> txPsd)
{
    NS_LOG_FUNCTION(this << txPsd);
    m_txPsd = txPsd;
}

void

CognitiveNetDeviceHelper::SetNoisePowerSpectralDensity(Ptr<SpectrumValue> noisePsd)
{
    NS_LOG_FUNCTION(this << noisePsd);
    m_noisePsd = noisePsd;
}

void

CognitiveNetDeviceHelper::SetPhyAttribute(std::string name, const AttributeValue& v)
{
    m_phy.Set(name, v);
}

void

CognitiveNetDeviceHelper::SetDeviceAttribute(std::string name, const AttributeValue& v)
{
    m_device.Set(name, v);
}

NetDeviceContainer

CognitiveNetDeviceHelper::Install(NodeContainer c) const
{
    NetDeviceContainer devices;
    for (auto i = c.Begin(); i != c.End(); ++i)
    {
        Ptr<Node> node = *i;

        Ptr<CognitiveGeneralNetDevice> dev = (m_device.Create())->GetObject<CognitiveGeneralNetDevice>();
        dev->SetAddress(Mac48Address::Allocate());
        std::queue<Ptr<MacDcfFrame>> *q = new std::queue<Ptr<MacDcfFrame>>();
        dev->SetQueue(q);

        // note that we could have used a SpectrumPhyHelper here, but
        // given that it is straightforward to handle the configuration
        // in this helper here, we avoid asking the user to pass us a
        // SpectrumPhyHelper, so to spare him some typing.

        Ptr<CognitivePhyDevice> phy = (m_phy.Create())->GetObject<CognitivePhyDevice>();
        NS_ASSERT(phy);

        dev->SetPhy(phy);

        NS_ASSERT(node);
        phy->SetMobility(node->GetObject<MobilityModel>());

        NS_ASSERT(dev);
        phy->SetDevice(dev);

        NS_ASSERT_MSG(
            m_txPsd,
            "you forgot to call CognitivePhyDeviceHelper::SetTxPowerSpectralDensity ()");
        phy->SetTxPowerSpectralDensity(m_txPsd);

        NS_ASSERT_MSG(
            m_noisePsd,
            "you forgot to call CognitivePhyDeviceHelper::SetNoisePowerSpectralDensity ()");
        phy->SetNoisePowerSpectralDensity(m_noisePsd);
        NS_ASSERT_MSG(m_channel, "you forgot to call CognitivePhyDeviceHelper::SetChannel ()");
        phy->SetChannel(m_channel);
        dev->SetChannel(m_channel);
        dev->SetPhyRate(phy->GetRate());
        m_channel->AddRx(phy);

        phy->SetGenericPhyTxEndCallback(
            MakeCallback(&CognitiveGeneralNetDevice::NotifyPartialTransmissionEnd, dev));
        phy->SetGenericPhyRxStartCallback(
            MakeCallback(&CognitiveGeneralNetDevice::NotifyReceptionStart, dev));
        phy->SetGenericPhyRxEndOkCallback(
            MakeCallback(&CognitiveGeneralNetDevice::NotifyReceptionEndOk, dev));
        phy->SetGenericPhyTxAbortCallback(
            MakeCallback(&CognitiveGeneralNetDevice::NotifyTransmissionAbort,dev));
            
        dev->SetPhyStartTxCallback(MakeCallback(&CognitivePhyDevice::StartTx, phy));
        dev->SetCcaResultCallback(MakeCallback(&CognitivePhyDevice::CarrierSense,phy));
        Ptr<AntennaModel> antenna = (m_antenna.Create())->GetObject<AntennaModel>();
        NS_ASSERT_MSG(antenna, "error in creating the AntennaModel object");
        phy->SetAntenna(antenna);
        
        node->AddDevice(dev);
        devices.Add(dev);
    }
    return devices;
}

NetDeviceContainer

CognitiveNetDeviceHelper::Install(Ptr<Node> node) const
{
    return Install(NodeContainer(node));
}

NetDeviceContainer

CognitiveNetDeviceHelper::Install(std::string nodeName) const
{
    Ptr<Node> node = Names::Find<Node>(nodeName);
    return Install(node);
}

} // namespace ns3