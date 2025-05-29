/*
 * Copyright (c) 2025 Telecommunications Lab, Higher Institiute for Applied Sciences and Technology , Damascus.
 *
 *
 * Author: Rida Takla <raidotakla@gmail.com>
 */

 #ifndef COGNITIVE_PHY_DEVICE
 #define COGNITIVE_PHY_DEVICE

 #include "ns3/cognitive-radio-energy-model.h"
 #include "ns3/cognitive-spectrum-interference.h"
 #include "ns3/spectrum-channel.h"
 #include "ns3/spectrum-interference.h"
 #include "ns3/spectrum-phy.h"
 #include "ns3/spectrum-signal-parameters.h"
 #include "ns3/spectrum-value.h"
 #include <ns3/data-rate.h>
 #include <ns3/event-id.h>
 #include <ns3/generic-phy.h>
 #include <ns3/mobility-model.h>
 #include <ns3/net-device.h>
 #include <ns3/nstime.h>
 #include <ns3/packet.h>
 #include <iostream>
 #include <string>
 #include <vector>
 
 using namespace ns3;
 
 namespace ns3
 {
  typedef Callback<void> GenericEnergyNotification ;
  
 class CognitivePhyDevice : public SpectrumPhy
 {
   public:
     CognitivePhyDevice();
     ~CognitivePhyDevice() override;
  
     /**
      *  PHY states
      */
     enum State
     {
         IDLE, //!< Idle state
         TX,   //!< Transmitting state
         RX,   //!< Receiving state
         SENS  //!< sensing the meduim
     };
  
     /**
      * @brief Get the type ID.
      * @return the object TypeId
      */
     static TypeId GetTypeId();
  
     // inherited from SpectrumPhy
     void SetChannel(Ptr<SpectrumChannel> c) override;
     void SetMobility(Ptr<MobilityModel> m) override;
     void SetDevice(Ptr<NetDevice> d) override;
     Ptr<MobilityModel> GetMobility() const override;
     Ptr<NetDevice> GetDevice() const override;
     Ptr<const SpectrumModel> GetRxSpectrumModel() const override;
     Ptr<Object> GetAntenna() const override;
     void StartRx(Ptr<SpectrumSignalParameters> params) override;
  
     /**
      * @return the transmission power spectral density 
      */
     Ptr<const SpectrumValue> GetTxSpectrumValue();
  
     /**
      * @brief Set the Power Spectral Density of outgoing signals in power units
      * (Watt, Pascal...) per Hz.
      *
      * @param txPsd Tx Power Spectral Density
      */
     void SetTxPowerSpectralDensity(Ptr<SpectrumValue> txPsd);
  
     /**
      * @brief Set the Noise Power Spectral Density in power units
      * (Watt, Pascal...) per Hz.
      * @param noisePsd the Noise Power Spectral Density
      */
     void SetNoisePowerSpectralDensity(Ptr<const SpectrumValue> noisePsd);

     Ptr<const SpectrumValue> GetLocalSpectrum();
  
     /**
      * Start a transmission
      *
      *
      * @param p the packet to be transmitted
      *
      * @return true if an error occurred and the transmission was not
      * started, false otherwise.
      */
     bool StartTx(Ptr<Packet> p);
  
     /**
      * Set the PHY rate to be used by this PHY.
      *
      * @param rate DataRate
      */
     void SetRate(DataRate rate);
  
     /**
      * Get the PHY rate to be used by this PHY.
      *
      * @return the PHY rate used by this PHY.
      */
     DataRate GetRate() const;

     /**
      * set the local model for a single 
      * channel 
      * @param loacalModel the model
      */
     void SetLocalSpectrumModel(Ptr<SpectrumModel> localModel);

     /**
      * get the spectrum local model
      * for a single channel
      * @return the model
      */
     Ptr<SpectrumModel> GetLocalSpectrumModel();
  
     /**
      * Set the callback for the end of a TX, as part of the
      * interconnections between the PHY and the MAC
      *
      * @param c the callback
      */
     void SetGenericPhyTxEndCallback(GenericPhyTxEndCallback c);
     /**
      * Set the callback for the start of RX, as part of the
      * interconnections between the PHY and the MAC
      *
      * @param c the callback
      */
     void SetGenericPhyRxStartCallback(GenericPhyRxStartCallback c);
    
     /**
      * set the callback for the end of a RX in error, as part of the
      * interconnections between the PHY and the MAC
      *
      * @param c the callback
      */
     void SetGenericPhyRxEndErrorCallback(GenericPhyRxEndErrorCallback c);
  
     /**
      * set the callback for the successful end of a RX, as part of the
      * interconnections between the PHY and the MAC
      *
      * @param c the callback
      */
     void SetGenericPhyRxEndOkCallback(GenericPhyRxEndOkCallback c);

     /**
      * set the callback for the aborting TX or failure
      * interconnections between the PHY and the MAC
      * @param c the callback 
      */
     void SetGenericPhyTxAbortCallback(GenericPhyTxEndCallback c);

     /**
      * set the callback for the successful start of TX, as part of the
      * interconnections between the PHY and the energy model
      *
      * @param c the callback
      */
     
     void SetGenericPhyEnergyTxStartCallback(GenericEnergyNotification c);
     
     /**
      * Set the callback for the end of a TX, as part of the
      * interconnections between the PHY and the energy model
      *
      * @param c the callback
      */
     void SetGenericPhyEnergyTxEndCallback(GenericEnergyNotification c);
     
     /**
      * Set the callback for the start of RX, as part of the
      * interconnections between the PHY and the energy
      *
      * @param c the callback
      */
     void SetGenericPhyEnergyRxStartCallback(GenericEnergyNotification c);
     
     /**
      * set the callback for the successful end of a RX, as part of the
      * interconnections between the PHY and the energy model
      *
      * @param c the callback
      */

     void SetGenericPhyEnergyRxEndCallback(GenericEnergyNotification c);
     

     /**
      * set the AntennaModel to be used
      *
      * @param a the Antenna Model
      */
      
     void SetAntenna(Ptr<AntennaModel> a);

     /**
      * @brief Set the energy model for this phy
      */
     void SetCognitiveDeviceEnergyModel(Ptr<CognitiveRadioEnergyModel> model);
     
     /**
      * carrier sensing to know if the spectrum is busy
      * @return double the value of the power on the spectrum
      */
     double CarrierSense(uint16_t index);

     /**
      * @brief setting the channel index
      */
     void SetChannelIndex(uint16_t Index); 

     /**
      * @brief set the channel information
      * the number of channels and the number
      * of bins for each channel and the channel 
      * Index
      * @param numOfChannels
      * @param numBins
      * @param 
      */
     void SetChannelsInfo(uint16_t numOfChannel,uint16_t numBins , uint16_t Index);

     /**
      * @brief starting the sensing procedure
      */
     void StartSensing();
  
   private:
     void DoDispose() override; 
     /**
      * End the current Tx
      */
     void EndTx();

     /**
      * aborting TX
      */
     void AbortTx();
     /**
      * About current Rx
      */
     void AbortRx();
     /**
      * End current Rx
      */
     void EndRx();
     
     void ChangeState(State s);

     
     EventId m_endRxEventId; //!< End Rx event
     Ptr<MobilityModel> m_mobility;  //!< Mobility model
     Ptr<AntennaModel> m_antenna;    //!< Antenna model
     Ptr<NetDevice> m_netDevice;     //!< NetDevice connected to this phy
     Ptr<SpectrumChannel> m_channel; //!< Channel
     Ptr<SpectrumValue> m_txPsd;       //!< Tx power spectral density
     Ptr<const SpectrumValue> m_rxPsd; //!< Rx power spectral density
     Ptr<Packet> m_txPacket;           //!< Tx packet
     Ptr<Packet> m_rxPacket;           //!< Rx packet
     Ptr<CognitiveRadioEnergyModel> m_energyModel; // the energy model of this Phy
     Ptr<SpectrumModel> m_localModel; //!< local model for a single channel

     DataRate m_rate; //!< Datarate
     State m_state;   //!< PHY state
  
     TracedCallback<Ptr<const Packet>> m_phyTxStartTrace;    //!< Trace - Tx start
     TracedCallback<Ptr<const Packet>> m_phyTxEndTrace;      //!< Trace - Tx end
     TracedCallback<Ptr<const Packet>> m_phyRxStartTrace;    //!< Trace - Rx start
     TracedCallback<Ptr<const Packet>> m_phyRxAbortTrace;    //!< Trace - Rx abort
     TracedCallback<Ptr<const Packet>> m_phyRxEndOkTrace;    //!< Trace - Tx end (ok)
     TracedCallback<Ptr<const Packet>> m_phyRxEndErrorTrace; //!< Trace - Rx end (error)
  
     GenericPhyTxEndCallback m_phyMacTxEndCallback;           //!< Callback - Tx end
     GenericPhyTxEndCallback m_phyMacTxAbortCallback;         //!< Callback - Tx abort
     GenericPhyRxStartCallback m_phyMacRxStartCallback;       //!< Callback - Rx start
     GenericPhyRxEndErrorCallback m_phyMacRxEndErrorCallback; //!< Callback - Rx error
     GenericPhyRxEndOkCallback m_phyMacRxEndOkCallback;       //!< Callback - Rx end

     GenericEnergyNotification m_phyEnergyTxStartCallback;    //!< Callback - Tx start for energy model
     GenericEnergyNotification m_phyEnergyTxEndCallback;      //!< Callback - Tx end for energy model
     GenericEnergyNotification m_phyEnergyRxStartCallback;    //!< Callback - Rx start for energy model 
     GenericEnergyNotification m_phyEnergyRxEndCallback;      //!< Callback - Rx end for energy model


     uint16_t m_channelIndex;                                 //!< the index of working channel
     uint16_t m_numBins;                                        //!< the number of bins in each channel
     uint16_t m_numOfChannels;                                  //!< the number of channels

     CognitiveSpectrumInterference m_interference; //!< Received interference

     EventId m_process ;
 };
 } // namespace ns3
 #endif
