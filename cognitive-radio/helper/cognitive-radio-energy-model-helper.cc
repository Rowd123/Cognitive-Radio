/*
 * Copyright (c) 2025 Telecommunications Lab, Higher Institiute for Applied Sciences and Technology , Damascus.
 *
 *
 * Author: Rida Takla <raidotakla@gmail.com>
 */

#include "cognitive-radio-energy-model-helper.h"

#include "ns3/aloha-noack-cognitive-net-device.h"

#include "ns3/cognitive-radio-phy.h"

namespace ns3
{

CognitiveRadioEnergyModelHelper::CognitiveRadioEnergyModelHelper()
{
     m_radioEnergy.SetTypeId("ns3::CognitiveRadioEnergyModel");
}

CognitiveRadioEnergyModelHelper::~CognitiveRadioEnergyModelHelper()
{
}

void

CognitiveRadioEnergyModelHelper::Set(std::string name, const AttributeValue& v)
{
    m_radioEnergy.Set(name, v);
}

/*
 * Private function starts here.
 */

Ptr<energy::DeviceEnergyModel>

CognitiveRadioEnergyModelHelper::DoInstall(Ptr<NetDevice> device, Ptr<energy::EnergySource> source) const
{
    NS_ASSERT(device);
    NS_ASSERT(source);
    // check if device is WifiNetDevice
    std::string deviceName = device->GetInstanceTypeId().GetName();
    if (deviceName != "ns3::AlohaNoackCognitiveNetDevice")
    {
        NS_FATAL_ERROR("NetDevice type is not AlohaNoackCognitiveNetDevice!");
    }
    Ptr<Node> node = device->GetNode();
    Ptr<CognitiveRadioEnergyModel> model = m_radioEnergy.Create()->GetObject<CognitiveRadioEnergyModel>();
    NS_ASSERT(model);

    // set energy depletion callback
    // if none is specified, make a callback to WifiPhy::SetOffMode
    Ptr<AlohaNoackCognitiveNetDevice> alohaDevice = DynamicCast<AlohaNoackCognitiveNetDevice>(device);
    Ptr<CognitiveRadioPhy> alohaPhy = DynamicCast<CognitiveRadioPhy>(alohaDevice->GetPhy());
    alohaPhy->SetCognitiveRadioEnergyModel(model);
    alohaPhy->SetGenericPhyEnergyTxStartCallback(MakeCallback(&CognitiveRadioEnergyModel::ChangeStateToTx,model));
    alohaPhy->SetGenericPhyEnergyTxEndCallback(MakeCallback(&CognitiveRadioEnergyModel::ChangeStateToIdle,model));
    alohaPhy->SetGenericPhyEnergyRxEndOkCallback(MakeCallback(&CognitiveRadioEnergyModel::ChangeStateToIdle,model));
    alohaPhy->SetGenericPhyEnergyRxEndErrorCallback(MakeCallback(&CognitiveRadioEnergyModel::ChangeStateToIdle,model));
    alohaPhy->SetGenericPhyEnergyRxStartCallback(MakeCallback(&CognitiveRadioEnergyModel::ChangeStateToRx,model));

    // add model to device model list in energy source
    source->AppendDeviceEnergyModel(model);
    // set energy source pointer
    model->SetEnergySource(source);
    
    return model;
}

} // namespace ns3
