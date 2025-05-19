/*
 * Copyright (c) 2025 HIAST
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Nicola Baldo <nbaldo@cttc.es>
 *
 * Edited: Rida Takla <reda.takla@hiast.edu.sy>
 */
#include "adhoc-aloha-noack-cognitive-phy-helper.h"
#include "ns3/cognitive-radio-phy.h"
#include "ns3/aloha-noack-cognitive-net-device.h"
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

NS_LOG_COMPONENT_DEFINE("AdhocAlohaNoackCognitivePhyHelper");

AdhocAlohaNoackCognitivePhyHelper::AdhocAlohaNoackCognitivePhyHelper()
{
    m_phy.SetTypeId("ns3::CognitiveRadioPhy");
    m_device.SetTypeId("ns3::AlohaNoackCognitiveNetDevice");
    m_queue.SetTypeId("ns3::DropTailQueue<Packet>");
    m_antenna.SetTypeId("ns3::IsotropicAntennaModel");
    m_threshold = 0 ;
}

AdhocAlohaNoackCognitivePhyHelper::~AdhocAlohaNoackCognitivePhyHelper()
{
}

void

AdhocAlohaNoackCognitivePhyHelper::SetChannel(Ptr<SpectrumChannel> channel)
{
    m_channel = channel;
}

void

AdhocAlohaNoackCognitivePhyHelper::SetChannel(std::string channelName)
{
    Ptr<SpectrumChannel> channel = Names::Find<SpectrumChannel>(channelName);
    m_channel = channel;
}

void

AdhocAlohaNoackCognitivePhyHelper::SetTxPowerSpectralDensity(Ptr<SpectrumValue> txPsd)
{
    NS_LOG_FUNCTION(this << txPsd);
    m_txPsd = txPsd;
}

void

AdhocAlohaNoackCognitivePhyHelper::SetNoisePowerSpectralDensity(Ptr<SpectrumValue> noisePsd)
{
    NS_LOG_FUNCTION(this << noisePsd);
    m_noisePsd = noisePsd;
}

void

AdhocAlohaNoackCognitivePhyHelper::SetPhyAttribute(std::string name, const AttributeValue& v)
{
    m_phy.Set(name, v);
}

void

AdhocAlohaNoackCognitivePhyHelper::SetDeviceAttribute(std::string name, const AttributeValue& v)
{
    m_device.Set(name, v);
}

NetDeviceContainer

AdhocAlohaNoackCognitivePhyHelper::Install(NodeContainer c) const
{
    NetDeviceContainer devices;
    for (auto i = c.Begin(); i != c.End(); ++i)
    {
        Ptr<Node> node = *i;

        Ptr<AlohaNoackCognitiveNetDevice> dev = (m_device.Create())->GetObject<AlohaNoackCognitiveNetDevice>();
        dev->SetAddress(Mac48Address::Allocate());
        Ptr<Queue<Packet>> q = (m_queue.Create())->GetObject<Queue<Packet>>();
        dev->SetQueue(q);

        // note that we could have used a SpectrumPhyHelper here, but
        // given that it is straightforward to handle the configuration
        // in this helper here, we avoid asking the user to pass us a
        // SpectrumPhyHelper, so to spare him some typing.

        Ptr<CognitiveRadioPhy> phy = (m_phy.Create())->GetObject<CognitiveRadioPhy>();
        NS_ASSERT(phy);

        dev->SetPhy(phy);

        NS_ASSERT(node);
        phy->SetMobility(node->GetObject<MobilityModel>());

        NS_ASSERT(dev);
        phy->SetDevice(dev);

        NS_ASSERT_MSG(
            m_txPsd,
            "you forgot to call AdhocAlohaNoackIdealPhyHelper::SetTxPowerSpectralDensity ()");
        phy->SetTxPowerSpectralDensity(m_txPsd,0);

        NS_ASSERT_MSG(
            m_noisePsd,
            "you forgot to call AdhocAlohaNoackIdealPhyHelper::SetNoisePowerSpectralDensity ()");
        phy->SetNoisePowerSpectralDensity(m_noisePsd);

        NS_ASSERT_MSG(
            m_local,
            "you forgot to call AdhocAlohaNoackIdealPhyHelper::SetLocalSpectrum ()");
        phy->SetLocalSpectrum(m_local);

        NS_ASSERT_MSG(
            m_threshold,
            "you forgot to call AdhocAlohaNoackIdealPhyHelper::SetThreshold ()");
        phy->SetThreshold(m_threshold);

        NS_ASSERT_MSG(m_channel, "you forgot to call AdhocAlohaNoackIdealPhyHelper::SetChannel ()");
        phy->SetChannel(m_channel);
        dev->SetChannel(m_channel);
        m_channel->AddRx(phy);

        phy->SetGenericPhyTxEndCallback(
            MakeCallback(&AlohaNoackCognitiveNetDevice::NotifyTransmissionEnd, dev));
        phy->SetGenericPhyRxStartCallback(
            MakeCallback(&AlohaNoackCognitiveNetDevice::NotifyReceptionStart, dev));
        phy->SetGenericPhyRxEndOkCallback(
            MakeCallback(&AlohaNoackCognitiveNetDevice::NotifyReceptionEndOk, dev));
        dev->SetGenericPhyTxStartCallback(MakeCallback(&CognitiveRadioPhy::StartTx, phy));

        Ptr<AntennaModel> antenna = (m_antenna.Create())->GetObject<AntennaModel>();
        NS_ASSERT_MSG(antenna, "error in creating the AntennaModel object");
        phy->SetAntenna(antenna);

        node->AddDevice(dev);
        devices.Add(dev);
    }
    return devices;
}

NetDeviceContainer

AdhocAlohaNoackCognitivePhyHelper::Install(Ptr<Node> node) const
{
    return Install(NodeContainer(node));
}

NetDeviceContainer

AdhocAlohaNoackCognitivePhyHelper::Install(std::string nodeName) const
{
    Ptr<Node> node = Names::Find<Node>(nodeName);
    return Install(node);
}

void

AdhocAlohaNoackCognitivePhyHelper::SetLocalSpectrum(Ptr<SpectrumValue> local)
{
    m_local = local;
}

void 

AdhocAlohaNoackCognitivePhyHelper::SetThreshold(double threshold)
{
    m_threshold = threshold;
}
} // namespace ns3