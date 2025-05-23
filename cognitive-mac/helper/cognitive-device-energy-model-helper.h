/*
 * Copyright (c) 2025 Telecommunications Lab, Higher Institiute for Applied Sciences and Technology , Damascus.
 *
 *
 * Author: Rida Takla <raidotakla@gmail.com>
 */

#ifndef COGNITIVE_DEVICE_ENERGY_MODEL_HELPER_H
#define COGNITIVE_DEVICE_ENERGY_MODEL_HELPER_H

#include "ns3/energy-model-helper.h"

#include "ns3/cognitive-radio-energy-model.h"

namespace ns3
{

/**
 * \ingroup energy
 * \brief Assign WifiRadioEnergyModel to cognitive radio devices.
 *
 * This installer installs CognitiveRadioEnergyModel for only CognitiveNetDevice objects.
 *
 */

class CognitiveDeviceEnergyModelHelper : public DeviceEnergyModelHelper
{
  public:
    /**
     * Construct a helper which is used to add a radio energy model to a node
     */
    CognitiveDeviceEnergyModelHelper();

    /**
     * Destroy a RadioEnergy Helper
     */
    ~CognitiveDeviceEnergyModelHelper() override;

    /**
     * \param name the name of the attribute to set
     * \param v the value of the attribute
     *
     * Sets an attribute of the underlying PHY object.
     */
    void Set(std::string name, const AttributeValue& v) override;

  private:
    /**
     * \param device Pointer to the NetDevice to install DeviceEnergyModel.
     * \param source Pointer to EnergySource to install.
     * \returns Ptr<DeviceEnergyModel>
     *
     * Implements DeviceEnergyModel::Install.
     */
    Ptr<energy::DeviceEnergyModel> DoInstall(Ptr<NetDevice> device,
                                             Ptr<energy::EnergySource> source) const override;
                                             
    ObjectFactory m_radioEnergy; ///< radio energy
};
} // namespace ns3

#endif /* COGNITIVE_RADIO_ENERGY_MODEL_HELPER_H */
