/*
 * Copyright (c) 2025 HIAST
 *
 * 
 *
 * Author: Rida Takla
 */


 #ifndef COGNITIVE_RADIO_PHY
 #define COGNITIVE_RADIO_PHY

 #include "cognitive-spectrum-interference.h"
 #include "cognitive-radio-energy-model.h"
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
  
 class CognitiveRadioPhy : public SpectrumPhy
 {
   public:
     CognitiveRadioPhy();
     ~CognitiveRadioPhy() override;
  
     /**
      *  PHY states
      */
     enum State
     {
         IDLE, //!< Idle state
         TX,   //!< Transmitting state
         RX    //!< Receiving state
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
      * @brief number of channels
      * @param num the number of channels;
      */
     void SetNumberOfChannel(uint16_t num);
  
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
     void SetTxPowerSpectralDensity(Ptr<SpectrumValue> txPsd , uint16_t index);
  
     /**
      * @brief Set the Noise Power Spectral Density in power units
      * (Watt, Pascal...) per Hz.
      * @param noisePsd the Noise Power Spectral Density
      */
     void SetNoisePowerSpectralDensity(Ptr<const SpectrumValue> noisePsd);
  
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
      * Set the callback for the end of a TX, as part of the
      * interconnections between the PHY and the MAC
      *
      * @param c the callback
      */
     void SetGenericPhyTxEndCallback(GenericPhyTxEndCallback c);

     /**
      * Set the callback for the end of a TX, as part of the
      * interconnections between the PHY and the energy model
      *
      * @param c the callback
      */
     void SetGenericPhyEnergyTxEndCallback(GenericEnergyNotification c);
  
     /**
      * Set the callback for the start of RX, as part of the
      * interconnections between the PHY and the MAC
      *
      * @param c the callback
      */
     void SetGenericPhyRxStartCallback(GenericPhyRxStartCallback c);

     /**
      * Set the callback for the start of RX, as part of the
      * interconnections between the PHY and the energy
      *
      * @param c the callback
      */
     void SetGenericPhyEnergyRxStartCallback(GenericEnergyNotification c);
     /**
      * set the callback for the end of a RX in error, as part of the
      * interconnections between the PHY and the MAC
      *
      * @param c the callback
      */
     void SetGenericPhyRxEndErrorCallback(GenericPhyRxEndErrorCallback c);

     /**
      * set the callback for the end of a RX in error, as part of the
      * interconnections between the PHY and the energy model
      *
      * @param c the callback
      */
     
     void SetGenericPhyEnergyRxEndErrorCallback(GenericEnergyNotification c);
  
     /**
      * set the callback for the successful end of a RX, as part of the
      * interconnections between the PHY and the MAC
      *
      * @param c the callback
      */
     void SetGenericPhyRxEndOkCallback(GenericPhyRxEndOkCallback c);

     /**
      * set the callback for the successful end of a RX, as part of the
      * interconnections between the PHY and the energy model
      *
      * @param c the callback
      */
     void SetGenericPhyEnergyRxEndOkCallback(GenericEnergyNotification c);
     
     /**
      * set the callback for the successful start of TX, as part of the
      * interconnections between the PHY and the energy model
      *
      * @param c the callback
      */
     void SetGenericPhyEnergyTxStartCallback(GenericEnergyNotification c);
     /**
      * set the AntennaModel to be used
      *
      * @param a the Antenna Model
      */
     void SetAntenna(Ptr<AntennaModel> a);
 
     /**
      * set the channel to be used
      *
      * @param index the Channel Index
      */
     void SetChannelIndex(uint16_t index);

     /**
     * @brief set the threshold
     * @param thresould the threshold wanted
     */
     void SetThreshold(double threshold);
     /**
     * @brief set local channel psd 
     * @param spectrum the spectrum to be assigned 
     */
     void SetLocalSpectrum(Ptr<SpectrumValue> spectrum);
     /**
     * @return num the number of packets sent
     */
     static uint32_t NumPacketsSent();
     /**
      * @brief Set the energy model for this phy
      */
     void SetCognitiveRadioEnergyModel(Ptr<CognitiveRadioEnergyModel> model);
     
  
   private:
     void DoDispose() override;
  
     /**
      * Change the PHY state
      * @param newState new state
      */
     void ChangeState(State newState);
     /**
      * End the current Tx
      */
     void EndTx();
     /**
      * About current Rx
      */
     void AbortRx();
     /**
      * End current Rx
      */
     void EndRx();
     /**
      * Chossing an Avaialble channel randomly
      * @return the index of the chosen channel
      */
     uint16_t ChooseChannel();
     
      /**
      * @brief sensing the spectrum 
      */
      void SenseSpectrum();
     EventId m_endRxEventId; //!< End Rx event
     const uint16_t NUMBER_OF_CHANNELS = 12;
     Ptr<MobilityModel> m_mobility;  //!< Mobility model
     Ptr<AntennaModel> m_antenna;    //!< Antenna model
     Ptr<NetDevice> m_netDevice;     //!< NetDevice connected to this phy
     Ptr<SpectrumChannel> m_channel; //!< Channel
     Ptr<SpectrumValue> m_txPsd;       //!< Tx power spectral density
     Ptr<SpectrumValue> m_localSpectrum; // The SpectrumValue for the Channel
     Ptr<CognitiveRadioEnergyModel> m_energyModel; // the energy model of this Phy
     Ptr<const SpectrumValue> m_rxPsd; //!< Rx power spectral density
     Ptr<Packet> m_txPacket;           //!< Tx packet
     Ptr<Packet> m_rxPacket;           //!< Rx packet

  
     DataRate m_rate; //!< Datarate
     State m_state;   //!< PHY state
  
     TracedCallback<Ptr<const Packet>> m_phyTxStartTrace;    //!< Trace - Tx start
     TracedCallback<Ptr<const Packet>> m_phyTxEndTrace;      //!< Trace - Tx end
     TracedCallback<Ptr<const Packet>> m_phyRxStartTrace;    //!< Trace - Rx start
     TracedCallback<Ptr<const Packet>> m_phyRxAbortTrace;    //!< Trace - Rx abort
     TracedCallback<Ptr<const Packet>> m_phyRxEndOkTrace;    //!< Trace - Tx end (ok)
     TracedCallback<Ptr<const Packet>> m_phyRxEndErrorTrace; //!< Trace - Rx end (error)
  
     GenericPhyTxEndCallback m_phyMacTxEndCallback;           //!< Callback - Tx end
     GenericEnergyNotification m_phyEnergyTxEndCallback;        //!< Callback - Tx end for energy model
     GenericPhyRxStartCallback m_phyMacRxStartCallback;       //!< Callback - Rx start
     GenericEnergyNotification m_phyEnergyRxStartCallback;        //!< Callback - Tx start for energy model
     GenericPhyRxEndErrorCallback m_phyMacRxEndErrorCallback; //!< Callback - Rx error
     GenericEnergyNotification m_phyEnergyRxEndErrorCallback;        //!< Callback - Rx end error for energy model
     GenericPhyRxEndOkCallback m_phyMacRxEndOkCallback;       //!< Callback - Rx end
     GenericEnergyNotification m_phyEnergyRxEndOkCallback;        //!< Callback - Rx end okay for energy model
     GenericEnergyNotification m_phyEnergyTxStartCallback;       // !< Callback for the energy start   
     std::vector<bool> m_channelStatus ; // Vector for the available channels
     uint16_t m_ChannelIndex ;           // current channel index
     uint16_t m_availableChannels ;      // number of available channels
     Ptr<UniformRandomVariable> m_rd ;   // random variable for choosing the channel
     CognitiveSpectrumInterference m_interference; //!< Received interference
     double m_threshold ;                   // threshold for interfernce 
     Time m_resense ;                    // time to resense the spectrum
     inline static uint32_t totpacket = 0 ; // number of packets sentzz
 };
 } // namespace ns3
 #endif
