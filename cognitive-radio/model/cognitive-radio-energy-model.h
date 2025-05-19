/*
 * Copyright (c) 2010 Network Security Lab, University of Washington, Seattle.
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Authors: Sidharth Nabar <snabar@uw.edu>
 *          He Wu <mdzz@u.washington.edu>
 */

#ifndef COGNITIVE_RADIO_ENERGY_MODEL_H
#define COGNITIVE_RADIO_ENERGY_MODEL_H

#include "ns3/wifi-units.h"
#include "ns3/device-energy-model.h"
#include "ns3/event-id.h"
#include "ns3/nstime.h"
#include "ns3/traced-value.h"

namespace ns3
{

class CognitiveRadioEnergyModel : public energy::DeviceEnergyModel
{
  public:
  enum State
  {
      TX,
      RX,
      IDLE,
      OFF
  };
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();
    CognitiveRadioEnergyModel();
    ~CognitiveRadioEnergyModel() override;

    /**
     * \brief Sets pointer to EnergySource installed on node.
     *
     * \param source Pointer to EnergySource installed on node.
     *
     * Implements DeviceEnergyModel::SetEnergySource.
     */
    void SetEnergySource(const Ptr<energy::EnergySource> source) override;

    /**
     * \returns Total energy consumption of the wifi device.
     *
     * Implements DeviceEnergyModel::GetTotalEnergyConsumption.
     */
    Watt_u GetTotalEnergyConsumption() const override;

    // Setter & getters for state power consumption.
    /**
     * \brief Gets idle current.
     *
     * \returns idle current of the wifi device.
     */
    ampere_u GetIdleCurrentA() const;
    /**
     * \brief Sets idle current.
     *
     * \param idleCurrentA the idle current
     */
    void SetIdleCurrentA(ampere_u idleCurrentA);
    /**
     * \brief Gets CCA busy current.
     *
     * \returns CCA Busy current of the wifi device.
     */
    ampere_u GetTxCurrentA() const;
    /**
     * \brief Sets transmit current.
     *
     * \param txCurrentA the transmit current
     */
    void SetTxCurrentA(ampere_u txCurrentA);
    /**
     * \brief Gets receive current.
     *
     * \returns receive current of the wifi device.
     */
    ampere_u GetRxCurrentA() const;
    /**
     * \brief Sets receive current.
     *
     * \param rxCurrentA the receive current
     */
    void SetRxCurrentA(ampere_u rxCurrentA);
    /**
     * \param txCurrent the model used to compute the wifi TX current.
     */
    void SetTxCurrentModel(const ampere_u txCurrentA);
    /**
     * @brief changing the state to TX
     */
    void ChangeStateToTx();

    /**
     * @brief changing the state to RX
     */
    
    void ChangeStateToRx();
    
     /**
     * @brief changing the state to IDLE
     */
    void ChangeStateToIdle();

    /**
     * @brief changing the state to OFF
     */
    void ChangeStateToOff();
    /**
     * \param state the wifi state
     *
     * \returns the time the radio can stay in that state based on the remaining energy.
     */
    /**
     * @brief handling the change in source energy
     */
    void HandleEnergyChanged(); 
    
    void ChangeState(int newState) override;
    void HandleEnergyDepletion() override;
    void HandleEnergyRecharged() override;

    Time GetMaximumTimeInState(State state) const;

  private:
    void DoDispose() override;

    /**
     * \param state the wifi state
     * \returns draw of device at given state.
     */
    ampere_u GetStateA(State state) const;

    /**
     * \returns Current draw of device at current state.
     *
     * Implements DeviceEnergyModel::GetCurrentA.
     */
    ampere_u DoGetCurrentA() const override;

    Ptr<energy::EnergySource> m_source; ///< energy source

    // Member variables for current draw in different radio modes.
    ampere_u m_txCurrent;                     ///< transmit current
    ampere_u m_rxCurrent;                     ///< receive current
    ampere_u m_idleCurrent;                   ///< idle current
    ampere_u m_ccaBusyCurrent;                ///< CCA busy current
    ampere_u m_switchingCurrent;              ///< switching current
    ampere_u m_sleepCurrent;                  ///< sleep current
    /// This variable keeps track of the total energy consumed by this model in watts.
    TracedValue<double> m_totalEnergyConsumption;

    // State variables.
    State m_currentState; ///< current state the radio is in
    Time m_lastUpdateTime;       ///< time stamp of previous energy update

    EventId m_switchToOffEvent; ///< switch to off event
};

} // namespace ns3

#endif /* COGNITIVE_RADIO_ENERGY_MODEL_H */
