
#include "cognitive-radio-energy-model.h"
#include "ns3/energy-source.h"
#include "ns3/log.h"
#include "ns3/pointer.h"
#include "ns3/simulator.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("CognitiveRadioEnergyModel");

NS_OBJECT_ENSURE_REGISTERED(CognitiveRadioEnergyModel);

TypeId

CognitiveRadioEnergyModel::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::CognitiveRadioEnergyModel")
            .SetParent<DeviceEnergyModel>()
            .SetGroupName("Energy")
            .AddConstructor<CognitiveRadioEnergyModel>()
            .AddAttribute("IdleCurrentA",
                          "The default radio Idle current in Ampere.",
                          DoubleValue(0.273), // idle mode = 273mA
                          MakeDoubleAccessor(&CognitiveRadioEnergyModel::SetIdleCurrentA,
                                             &CognitiveRadioEnergyModel::GetIdleCurrentA),
                          MakeDoubleChecker<ampere_u>())
            .AddAttribute("TxCurrentA",
                          "The radio TX current in Ampere.",
                          DoubleValue(0.380), // transmit at 0dBm = 380mA
                          MakeDoubleAccessor(&CognitiveRadioEnergyModel::SetTxCurrentA,
                                             &CognitiveRadioEnergyModel::GetTxCurrentA),
                          MakeDoubleChecker<ampere_u>())
            .AddAttribute("RxCurrentA",
                          "The radio RX current in Ampere.",
                          DoubleValue(0.313), // receive mode = 313mA
                          MakeDoubleAccessor(&CognitiveRadioEnergyModel::SetRxCurrentA,
                                             &CognitiveRadioEnergyModel::GetRxCurrentA),
                          MakeDoubleChecker<ampere_u>())
            .AddTraceSource(
                "TotalEnergyConsumption",
                "Total energy consumption of the radio device.",
                MakeTraceSourceAccessor(&CognitiveRadioEnergyModel::m_totalEnergyConsumption),
                "ns3::TracedValueCallback::Double");
    return tid;
}

CognitiveRadioEnergyModel::CognitiveRadioEnergyModel()
    : m_source(nullptr),
      m_currentState(State::IDLE),
      m_lastUpdateTime(Seconds(0.0))
{

}

CognitiveRadioEnergyModel::~CognitiveRadioEnergyModel()
{
    NS_LOG_FUNCTION(this);
}

void

CognitiveRadioEnergyModel::SetEnergySource(const Ptr<energy::EnergySource> source)
{
    NS_LOG_FUNCTION(this << source);
    NS_ASSERT(source);
    m_source = source;
    m_switchToOffEvent.Cancel();
    const auto durationToOff = GetMaximumTimeInState(m_currentState);
    m_switchToOffEvent = Simulator::Schedule(durationToOff,
                                             &CognitiveRadioEnergyModel::ChangeStateToOff,
                                             this);
}

Watt_u

CognitiveRadioEnergyModel::GetTotalEnergyConsumption() const
{
    NS_LOG_FUNCTION(this);

    const auto duration = Simulator::Now() - m_lastUpdateTime;
    NS_ASSERT(duration.IsPositive()); // check if duration is valid

    // energy to decrease = current * voltage * time
    const auto supplyVoltage = m_source->GetSupplyVoltage();
    const auto energyToDecrease = duration.GetSeconds() * GetStateA(m_currentState) * supplyVoltage;

    // notify energy source
    m_source->UpdateEnergySource();

    return m_totalEnergyConsumption + energyToDecrease;
}

ampere_u

CognitiveRadioEnergyModel::GetIdleCurrentA() const
{
    NS_LOG_FUNCTION(this);
    return m_idleCurrent;
}

void

CognitiveRadioEnergyModel::SetIdleCurrentA(ampere_u idleCurrent)
{
    NS_LOG_FUNCTION(this << idleCurrent);
    m_idleCurrent = idleCurrent;
}

ampere_u

CognitiveRadioEnergyModel::GetTxCurrentA() const
{
    NS_LOG_FUNCTION(this);
    return m_txCurrent;
}

void

CognitiveRadioEnergyModel::SetTxCurrentA(ampere_u txCurrent)
{
    NS_LOG_FUNCTION(this << txCurrent);
    m_txCurrent = m_idleCurrent + txCurrent;
}

ampere_u

CognitiveRadioEnergyModel::GetRxCurrentA() const
{
    NS_LOG_FUNCTION(this);
    return m_rxCurrent;
}

void

CognitiveRadioEnergyModel::SetRxCurrentA(ampere_u rxCurrent)
{
    NS_LOG_FUNCTION(this << rxCurrent);
    m_rxCurrent = rxCurrent;
}

Time

CognitiveRadioEnergyModel::GetMaximumTimeInState(State state) const
{
    if (state == State::OFF)
    {
        NS_FATAL_ERROR("Requested maximum remaining time for OFF state");
    }
    const auto remainingEnergy = m_source->GetRemainingEnergy();
    const auto supplyVoltage = m_source->GetSupplyVoltage();
    const auto current = GetStateA(state);
    return Seconds(remainingEnergy / (current * supplyVoltage));
}

void

CognitiveRadioEnergyModel::ChangeStateToTx()
{
    m_switchToOffEvent.Cancel();
    const auto durationToOff = GetMaximumTimeInState(State::TX);
    m_switchToOffEvent = Simulator::Schedule(durationToOff,
                                             &CognitiveRadioEnergyModel::ChangeStateToOff,
                                             this);
    const auto duration = Simulator::Now() - m_lastUpdateTime;
    NS_ASSERT(duration.IsPositive()); // check if duration is valid

    // energy to decrease = current * voltage * time
    const auto supplyVoltage = m_source->GetSupplyVoltage();
    const auto energyToDecrease = duration.GetSeconds() * GetStateA(m_currentState) * supplyVoltage;
    // update total energy consumption
    m_totalEnergyConsumption += energyToDecrease;
    NS_ASSERT(m_totalEnergyConsumption <= m_source->GetInitialEnergy());

    // update last update time stamp
    m_lastUpdateTime = Simulator::Now();

    // notify energy source
    m_source->UpdateEnergySource();

    m_currentState = State::TX;

}

void

CognitiveRadioEnergyModel::ChangeStateToRx()
{
    m_switchToOffEvent.Cancel();
    const auto durationToOff = GetMaximumTimeInState(State::RX);
    m_switchToOffEvent = Simulator::Schedule(durationToOff,
                                             &CognitiveRadioEnergyModel::ChangeStateToOff,
                                             this);
    const auto duration = Simulator::Now() - m_lastUpdateTime;
    NS_ASSERT(duration.IsPositive()); // check if duration is valid

    // energy to decrease = current * voltage * time
    const auto supplyVoltage = m_source->GetSupplyVoltage();
    const auto energyToDecrease = duration.GetSeconds() * GetStateA(m_currentState) * supplyVoltage;
    // update total energy consumption
    m_totalEnergyConsumption += energyToDecrease;
    NS_ASSERT(m_totalEnergyConsumption <= m_source->GetInitialEnergy());

    // update last update time stamp
    m_lastUpdateTime = Simulator::Now();

    // notify energy source
    m_source->UpdateEnergySource();

    m_currentState = State::RX;

}

void

CognitiveRadioEnergyModel::ChangeStateToIdle()
{
    m_switchToOffEvent.Cancel();
    const auto durationToOff = GetMaximumTimeInState(State::IDLE);
    m_switchToOffEvent = Simulator::Schedule(durationToOff,
                                             &CognitiveRadioEnergyModel::ChangeStateToOff,
                                             this);
    const auto duration = Simulator::Now() - m_lastUpdateTime;
    NS_ASSERT(duration.IsPositive()); // check if duration is valid

    // energy to decrease = current * voltage * time
    const auto supplyVoltage = m_source->GetSupplyVoltage();
    const auto energyToDecrease = duration.GetSeconds() * GetStateA(m_currentState) * supplyVoltage;
    // update total energy consumption
    m_totalEnergyConsumption += energyToDecrease;
    NS_ASSERT(m_totalEnergyConsumption <= m_source->GetInitialEnergy());

    // update last update time stamp
    m_lastUpdateTime = Simulator::Now();

    // notify energy source
    m_source->UpdateEnergySource();

    m_currentState = State::IDLE;
}

void

CognitiveRadioEnergyModel::ChangeStateToOff()
{
    m_switchToOffEvent.Cancel();
    const auto duration = Simulator::Now() - m_lastUpdateTime;
    NS_ASSERT(duration.IsPositive()); // check if duration is valid

    // energy to decrease = current * voltage * time
    const auto supplyVoltage = m_source->GetSupplyVoltage();
    const auto energyToDecrease = duration.GetSeconds() * GetStateA(m_currentState) * supplyVoltage;
    // update total energy consumption
    m_totalEnergyConsumption += energyToDecrease;
    NS_ASSERT(m_totalEnergyConsumption <= m_source->GetInitialEnergy());

    // update last update time stamp
    m_lastUpdateTime = Simulator::Now();

    // notify energy source
    m_source->UpdateEnergySource();

    m_currentState = State::OFF;
}
void

CognitiveRadioEnergyModel::HandleEnergyChanged()
{
    NS_LOG_FUNCTION(this);
    NS_LOG_DEBUG("CognitiveRadioEnergyModel:Energy is changed!");
    if (m_currentState != State::OFF)
    {
        m_switchToOffEvent.Cancel();
        const auto durationToOff = GetMaximumTimeInState(m_currentState);
        m_switchToOffEvent = Simulator::Schedule(durationToOff,
                                                 &CognitiveRadioEnergyModel::ChangeStateToOff,
                                                 this);
    }
}
/*
 * Private functions start here.
 */

void

CognitiveRadioEnergyModel::DoDispose()
{
    NS_LOG_FUNCTION(this);
    m_source = nullptr;
}

ampere_u

CognitiveRadioEnergyModel::GetStateA(State state) const
{
    switch (state)
    {
    case State::IDLE:
        return m_idleCurrent;
    case State::TX:
        return m_txCurrent;
    case State::RX:
        return m_rxCurrent;
    case State::OFF:
        return 0.0;
    }
    NS_FATAL_ERROR("CognitiveRadioEnergyModel: undefined radio state " << state);
}

ampere_u

CognitiveRadioEnergyModel::DoGetCurrentA() const
{
    return GetStateA(m_currentState);
}

void

CognitiveRadioEnergyModel::ChangeState(int newState)
{
    return;
}

void

CognitiveRadioEnergyModel::HandleEnergyDepletion()
{
    return;
}

void

CognitiveRadioEnergyModel::HandleEnergyRecharged()
{
    return;
}

}