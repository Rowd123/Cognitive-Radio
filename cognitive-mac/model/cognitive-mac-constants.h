/*
 * Copyright (c) 2025 Telecommunications Lab, Higher Institiute for Applied Sciences and Technology , Damascus.
 *
 *
 * Author: Rida Takla <raidotakla@gmail.com>
 */
 #ifndef COGNITIVE_MAC_CONSTANTS
 #define COGNITIVE_MAC_CONSTANTS

 #include <ns3/packet.h>
 #include <ns3/nstime.h>


 #include <iostream>
 namespace ns3
 {
    const Time SLOT = MicroSeconds(9);          //!< SLOT Time
    const Time SIFS = MicroSeconds(16);         //!< SIFS Time
    const Time DIFS = MicroSeconds(34);         //!< DIFS Time
    const Time margin = MicroSeconds(1) ;       //!< the marging for the data transmission duration   
    const Time infinty = Seconds(10);
    const Time CTStimeout = MicroSeconds(55);   //!< Timeout waiting for cts
    const uint32_t MacHeaderSize = 24;          //!< the size of the MAC header
    const uint32_t PhyHeaderSize = 24;          //!< the size of the PHY header
    const uint32_t RTSsize = 44;                //!< the size of the rts packet in bytes
    const uint32_t CTSsize = 38;                //!< the size of the cts packet in bytes
    const uint32_t ACKsize = 38;                //!< the size of the ack packet in bytes
                
    /**
     * the state of the net device
     */
   enum CognitiveState
   {
      IDLE,       //!< Idle state
      TX,         //!< Transmitting State
      RX,         //!< Receiving State
      CCA_BUSY    //!< Busy Channel
   };
   enum FrameType
   {
     DATA,  //!< data
     RTS,   //!< request to sent
     CTS,   //!< clear to send
     ACK    //!< acknowlegement 
   };
}

 #endif