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
#include "ns3/cognitive-control-application.h"
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

CognitiveNetDeviceHelper::CognitiveNetDeviceHelper():
    m_stopTime(Seconds(10.0)),m_startTime(Seconds(0.0)),
    m_threshold(1e-10)
{
    m_phy.SetTypeId("ns3::CognitivePhyDevice");
    m_device.SetTypeId("ns3::CognitiveGeneralNetDevice");
    m_antenna.SetTypeId("ns3::IsotropicAntennaModel");
    m_controlApp.SetTypeId("ns3::CognitiveControlApplication");
    m_spectrumCtrl.SetTypeId("ns3::SpectrumControlModule");
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

CognitiveNetDeviceHelper::SetDataTxPowerSpectralDensity(Ptr<SpectrumValue> dataTxPsd)
{
    NS_LOG_FUNCTION(this << dataTxPsd);
    m_dataTxPsd = dataTxPsd;
}

void

CognitiveNetDeviceHelper::SetCtrlTxPowerSpectralDensity(Ptr<SpectrumValue> ctlrTxPsd)
{
    NS_LOG_FUNCTION(this << ctlrTxPsd);
    m_ctrlTxPsd = ctlrTxPsd;
}

void

CognitiveNetDeviceHelper::SetLocalSpectrumModel(Ptr<SpectrumModel> localModel)
{
    NS_LOG_FUNCTION(this<<localModel);
    m_localModel = localModel;
}

void

CognitiveNetDeviceHelper::SetNoisePowerSpectralDensity(Ptr<SpectrumValue> noisePsd)
{
    NS_LOG_FUNCTION(this << noisePsd);
    m_noisePsd = noisePsd;
}

void

CognitiveNetDeviceHelper::SetStopTime(Time time)
{
    m_stopTime = time;
}

void

CognitiveNetDeviceHelper::SetStartTime(Time time)
{
    m_startTime = time;
}

void

CognitiveNetDeviceHelper::SetThreshold(double threshold)
{
    m_threshold = threshold;
}

void

CognitiveNetDeviceHelper::SetChannelsInfo(uint16_t bgCount, uint16_t bgSize,
                                         uint16_t numOfChannels, uint16_t numBins)
{
    m_bgCount = bgCount;
    m_bgSize = bgSize;
    m_numOfChannels = numOfChannels;
    m_numBins = numBins;
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

        Ptr<CognitiveControlApplication> ctrlApp = (m_controlApp.Create())->GetObject<CognitiveControlApplication>();
        Ptr<SpectrumControlModule> ctrlSpect = (m_spectrumCtrl.Create())->GetObject<SpectrumControlModule>();
        
        ctrlApp->SetStartTime(m_startTime);
        ctrlApp->SetStopTime(m_stopTime);
        
    
        Ptr<CognitiveGeneralNetDevice> datadev = (m_device.Create())->GetObject<CognitiveGeneralNetDevice>();
        Ptr<CognitiveGeneralNetDevice> controldev = (m_device.Create())->GetObject<CognitiveGeneralNetDevice>();


        datadev->SetAddress(Mac48Address::Allocate());
        controldev->SetAddress(datadev->GetAddress());
        std::queue<Ptr<MacDcfFrame>> *q = new std::queue<Ptr<MacDcfFrame>>();
        std::queue<Ptr<MacDcfFrame>> *f = new std::queue<Ptr<MacDcfFrame>>();
        datadev->SetQueue(q);
        controldev->SetQueue(f);   

        ctrlApp->SetAddress(datadev->GetAddress());
        ctrlSpect->SetChannels(m_bgSize,m_bgCount);
        ctrlSpect->SetThreshold(m_threshold);
        ctrlApp->SetSpectrumControlModule(ctrlSpect);
        ctrlApp->SetDataDevice(datadev);
        ctrlApp->SetControlDevice(controldev);

        datadev->SetDeviceKind(true);

        ctrlSpect->SetSenseResultCallback(
            MakeCallback(&CognitiveControlApplication::SelectedChannelResult,ctrlApp));
        ctrlSpect->SetQtableResultCallback(
            MakeCallback(&CognitiveControlApplication::ReceivingQtable,ctrlApp));    
        ctrlApp->SetChannelSensingCallback(
            MakeCallback(&CognitiveGeneralNetDevice::SpectrumControlSense,datadev));
        ctrlApp->SetStartSensingPeriodCallback(
            MakeCallback(&CognitiveGeneralNetDevice::StopWork,datadev));
        ctrlApp->SetGetRemainingEnergyCallback(
            MakeCallback(&CognitiveGeneralNetDevice::GetReamainingEnergy,datadev));
        ctrlApp->SetSetCommonDataChannelsCallback(
            MakeCallback(&CognitiveGeneralNetDevice::SetClusterInfo,datadev));
        controldev->SetReceiveCtrlPacketCallback(
            MakeCallback(&CognitiveControlApplication::ReceiveControlMsg,ctrlApp));
            
        // note that we could have used a SpectrumPhyHelper here, but
        // given that it is straightforward to handle the configuration
        // in this helper here, we avoid asking the user to pass us a
        // SpectrumPhyHelper, so to spare him some typing.

        Ptr<CognitivePhyDevice> dataphy = (m_phy.Create())->GetObject<CognitivePhyDevice>();
        Ptr<CognitivePhyDevice> controlphy = (m_phy.Create())->GetObject<CognitivePhyDevice>();
        NS_ASSERT(dataphy);
        NS_ASSERT(controldev);

        datadev->SetGetRemainingEnergyCallback(
            MakeCallback(&CognitivePhyDevice::GetRemainingEnergy,dataphy));

        datadev->SetPhy(dataphy);
        controldev->SetPhy(controldev);

        dataphy->SetChannelsInfo(m_numOfChannels,m_numBins,0);
        controlphy->SetChannelsInfo(m_numOfChannels,m_numBins,m_numOfChannels-1);

        NS_ASSERT(node);
        dataphy->SetMobility(node->GetObject<MobilityModel>());
        controlphy->SetMobility(node->GetObject<MobilityModel>());

        NS_ASSERT(datadev);
        NS_ASSERT(controldev);
        dataphy->SetDevice(datadev);
        controlphy->SetDevice(controldev);

        NS_ASSERT_MSG(
            m_dataTxPsd,
            "you forgot to call CognitivePhyDeviceHelper::SetDataTxPowerSpectralDensity ()");
        Ptr<SpectrumValue> datatxPsd = m_dataTxPsd->Copy();
        
        dataphy->SetTxPowerSpectralDensity(datatxPsd);

        NS_ASSERT_MSG(
            m_ctrlTxPsd,
            "you forgot to call CognitivePhyDeviceHelper::SetCtrlTxPowerSpectralDensity ()");
        controlphy->SetTxPowerSpectralDensity(m_ctrlTxPsd);

        NS_ASSERT_MSG(
            m_noisePsd,
            "you forgot to call CognitivePhyDeviceHelper::SetNoisePowerSpectralDensity ()");
        dataphy->SetNoisePowerSpectralDensity(m_noisePsd);
        controlphy->SetNoisePowerSpectralDensity(m_noisePsd);
        
        
        NS_ASSERT_MSG(m_channel, "you forgot to call CognitivePhyDeviceHelper::SetChannel ()");
        dataphy->SetChannel(m_channel);
        controlphy->SetChannel(m_channel);
        datadev->SetChannel(m_channel);
        controldev->SetChannel(m_channel);
        datadev->SetPhyRate(dataphy->GetRate());
        controldev->SetPhyRate(controlphy->GetRate());
        datadev->SetThreshold(m_threshold);
        controldev->SetThreshold(m_threshold);
        m_channel->AddRx(dataphy);
        m_channel->AddRx(controlphy);
        dataphy->SetLocalSpectrumModel(m_localModel);
        controlphy->SetLocalSpectrumModel(m_localModel);

        dataphy->SetGenericPhyTxEndCallback(
            MakeCallback(&CognitiveGeneralNetDevice::NotifyPartialTransmissionEnd, datadev));
        dataphy->SetGenericPhyRxStartCallback(
            MakeCallback(&CognitiveGeneralNetDevice::NotifyReceptionStart, datadev));
        dataphy->SetGenericPhyRxEndOkCallback(
            MakeCallback(&CognitiveGeneralNetDevice::NotifyReceptionEndOk, datadev));
        dataphy->SetGenericPhyTxAbortCallback(
            MakeCallback(&CognitiveGeneralNetDevice::NotifyTransmissionAbort,datadev));
        

        controlphy->SetGenericPhyTxEndCallback(
            MakeCallback(&CognitiveGeneralNetDevice::NotifyPartialTransmissionEnd, controldev));
        controlphy->SetGenericPhyRxStartCallback(
            MakeCallback(&CognitiveGeneralNetDevice::NotifyReceptionStart, controldev));
        controlphy->SetGenericPhyRxEndOkCallback(
            MakeCallback(&CognitiveGeneralNetDevice::NotifyReceptionEndOk, controldev));
        controlphy->SetGenericPhyTxAbortCallback(
            MakeCallback(&CognitiveGeneralNetDevice::NotifyTransmissionAbort,controldev));

        datadev->SetPhyStartTxCallback(MakeCallback(&CognitivePhyDevice::StartTx, dataphy));
        datadev->SetCcaResultCallback(MakeCallback(&CognitivePhyDevice::CarrierSense,dataphy));
        datadev->SetStopWorkCallback(MakeCallback(&CognitivePhyDevice::StartSensing,dataphy));
        datadev->SetCurrentDataChannelCallback(MakeCallback(&CognitivePhyDevice::SetChannelIndex,dataphy));
        
        controldev->SetPhyStartTxCallback(MakeCallback(&CognitivePhyDevice::StartTx, controlphy));
        controldev->SetCcaResultCallback(MakeCallback(&CognitivePhyDevice::CarrierSense,controlphy));
        controldev->TransmissionPermission(true);
        
        Ptr<AntennaModel> dataAntenna = (m_antenna.Create())->GetObject<AntennaModel>();
        Ptr<AntennaModel> ctrlAntenna = (m_antenna.Create())->GetObject<AntennaModel>();
        NS_ASSERT_MSG(dataAntenna, "error in creating the Data AntennaModel object");
        NS_ASSERT_MSG(ctrlAntenna, "error in creating the Ctrl AntennaModel object");
        dataphy->SetAntenna(dataAntenna);
        controlphy->SetAntenna(ctrlAntenna);
        node->AddApplication(ctrlApp);
        node->AddDevice(datadev);
        node->AddDevice(controldev);
        devices.Add(datadev);
        devices.Add(controldev);    
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