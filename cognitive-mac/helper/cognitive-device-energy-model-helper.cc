/*
 * Copyright (c) 2025 Telecommunications Lab, Higher Institiute for Applied Sciences and Technology , Damascus.
 *
 *
 * Author: Rida Takla <raidotakla@gmail.com>
 */

#include "cognitive-device-energy-model-helper.h"

#include "ns3/cognitive-general-net-device.h"

#include "ns3/cognitive-phy-device.h"

namespace ns3
{

CognitiveDeviceEnergyModelHelper::CognitiveDeviceEnergyModelHelper()
{
     m_radioEnergy.SetTypeId("ns3::CognitiveRadioEnergyModel");
}

CognitiveDeviceEnergyModelHelper::~CognitiveDeviceEnergyModelHelper()
{
}

void

CognitiveDeviceEnergyModelHelper::Set(std::string name, const AttributeValue& v)
{
    m_radioEnergy.Set(name, v);
}

/*
 * Private function starts here.
 */

Ptr<energy::DeviceEnergyModel>

CognitiveDeviceEnergyModelHelper::DoInstall(Ptr<NetDevice> device, Ptr<energy::EnergySource> source) const
{
    NS_ASSERT(device);
    NS_ASSERT(source);
    // check if device is WifiNetDevice
    std::string deviceName = device->GetInstanceTypeId().GetName();
    if (deviceName != "ns3::CognitiveGeneralNetDevice")
    {
        NS_FATAL_ERROR("NetDevice type is not CognitiveGeneralNetDevice!");
    }
    Ptr<Node> node = device->GetNode();
    Ptr<CognitiveRadioEnergyModel> model = m_radioEnergy.Create()->GetObject<CognitiveRadioEnergyModel>();
    NS_ASSERT(model);

    // set energy depletion callback
    // if none is specified, make a callback to WifiPhy::SetOffMode
    Ptr<CognitiveGeneralNetDevice> cognitiveDevice = DynamicCast<CognitiveGeneralNetDevice>(device);
    Ptr<CognitivePhyDevice> CognitivePhy = DynamicCast<CognitivePhyDevice>(cognitiveDevice->GetPhy());
    CognitivePhy->SetCognitiveDeviceEnergyModel(model);
    CognitivePhy->SetGenericPhyEnergyTxStartCallback(MakeCallback(&CognitiveRadioEnergyModel::ChangeStateToTx,model));
    CognitivePhy->SetGenericPhyEnergyTxEndCallback(MakeCallback(&CognitiveRadioEnergyModel::ChangeStateToIdle,model));
    CognitivePhy->SetGenericPhyEnergyRxEndCallback(MakeCallback(&CognitiveRadioEnergyModel::ChangeStateToIdle,model));
    CognitivePhy->SetGenericPhyEnergyRxStartCallback(MakeCallback(&CognitiveRadioEnergyModel::ChangeStateToRx,model));

    // add model to device model list in energy source
    source->AppendDeviceEnergyModel(model);
    // set energy source pointer
    model->SetEnergySource(source);
    
    return model;
}

} // namespace ns3
