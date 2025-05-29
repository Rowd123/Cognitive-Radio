/*
 * Copyright (c) 2025 Telecommunications Lab, Higher Institiute for Applied Sciences and Technology , Damascus.
 *
 *
 * Author: Rida Takla <raidotakla@gmail.com>
 */
#ifndef COGNITIVE_CONTROL_APPLICATION
#define COGNITIVE_CONTROL_APPLICATION

#include "cognitive-general-net-device.h"
#include "spectrum-control-module.h"
#include "ns3/application.h"
#include "ns3/address.h"
#include "ns3/simulator.h"
#include "ns3/net-device.h"

namespace ns3
{

typedef Callback<double , uint16_t> ChannelSensingCallback ;
typedef Callback<void , Time> StartSensingPeriodCallback ; 

    class CognitiveControlApplication : public Application
    {
        public:
          CognitiveControlApplication();
          ~CognitiveControlApplication();

          static TypeId GetTypeId();

          /**
           * @brief Set the spectrum control
           * module
           * @param specturmModule
           */
          void SetSpectrumControlModule(Ptr<SpectrumControlModule> spectrumModule);

          /**
           * @brief Set the data net device
           * @param dataDevice
           */
          void SetDataDevice(Ptr<NetDevice> dataDevice);

          /**
           * @brief Set the control net device
           * @param controlDevice 
           */
          void SetControlDevice(Ptr<NetDevice> controlDevice);

          /**
           * @brief set the channel sensing callback
           * @param c the callback
           */
          void SetChannelSensingCallback(ChannelSensingCallback c);

          /**
           * @brief StartSensingPeriodCallback
           * @param c the callback
           */
          void SetStartSensingPeriodCallback(StartSensingPeriodCallback c);
          /**
           * @brief get the sensing result of a specified channel
           * @param Index of the channel wanted 
           */
          double SelectedChannelResult(uint16_t Index);

          /**
           * @brief receiving the result of the 
           * Q-learning
           * @param Qtable resulting vector
           */
          void ReceivingQtable(std::vector<double> Qtable);

          /**
           * @brief starting the sensing period
           */
          void StartSensingPeriod();
        
          private:

          void StartApplication() override;
          void StopApplication() override;
          
          Ptr<SpectrumControlModule> m_spectrumControlModule;  //!< the spectrum control module 
          Ptr<NetDevice> m_dataDevice;                         //!< the net device 
          Ptr<NetDevice> m_controlDevice;                      //!< the control device

          ChannelSensingCallback m_channelSensingCallback; 
          StartSensingPeriodCallback m_startSensingPeriodCallback;   

          EventId m_curAction ;                                //!< the current action to be done
          EventId m_resense;                                   //!< event for resensing and updating Q-table

          Time m_resenseTime;                                  //!< period for resensing 

    };
}

#endif  // COGNITIVE_CONTROL_APPLICATION