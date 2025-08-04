/*
 * Copyright (c) 2025 Telecommunications Lab, Higher Institiute for Applied Sciences and Technology , Damascus.
 *
 *
 * Author: Rida Takla <raidotakla@gmail.com>
 */
#ifndef COGNITIVE_GENERAL_NET_DEVICE
#define COGNITIVE_GENERAL_NET_DEVICE

#include "cognitive-mac-constants.h"
#include "mac-frames.h"
#include "cognitive-routing-unite.h"

#include <ns3/drop-tail-queue.h>
#include <ns3/address.h>
#include <ns3/callback.h>
#include <ns3/generic-phy.h>
#include <ns3/mac48-address.h>
#include <ns3/net-device.h>
#include <ns3/node.h>
#include <ns3/nstime.h>
#include <ns3/packet.h>
#include <ns3/ptr.h>
#include <ns3/traced-callback.h>
#include <ns3/random-variable-stream.h>
#include <ns3/event-id.h>
#include <ns3/data-rate.h>
#include <map>
#include <cstring>
#include <queue>

namespace ns3
{
class SpectrumChannel;
class Channel;
class SpectrumErrorModel;

/**
 * @ingroup spectrum
 * 
 * this device implement the following features:
 *  a MAC protocol depending on the general architecture of the CDMA/CA
 *  can support any physical layer compatible with the API defined in generic-phy.h
 */

typedef Callback<bool,Ptr<Packet>> PhyTxStartCallback;
typedef Callback<double,uint16_t> SensingResultCallback;
typedef Callback<void> StopWorkCallback;
typedef Callback<double> GetRemainingEnergyCallback;
typedef Callback<void,uint16_t> CurrentDataChannelCallback;

    class CognitiveGeneralNetDevice : public NetDevice
    {
        public:

        /**
         * @brief Get the Type ID
         * @return the object type ID
         */
        static TypeId GetTypeId();

        CognitiveGeneralNetDevice();                 // Construcotr
        ~CognitiveGeneralNetDevice() override;       // Destructor

        /**
         * set the queue which is going to be used by this device
         *
         * @param queue
         */
        virtual void SetQueue(std::queue <Ptr<MacDcfFrame>> *que);

        /**
         * notify the MAC that the PHY has finished a previously started transmission
         * @param packet the packet which was transmitted 
         */
        void ContinueTransmission();

        /**
         * notify Tx abort because of the receiving something now
         */
        void NotifyTransmissionAbort(Ptr<const Packet>);

        /**
         * notify the RTS,CTS,ACK,DATA transmission end
         * @param packet
         */
        void NotifyPartialTransmissionEnd(Ptr<const Packet>);

        /**
         * notify the MAC that the PHY has started receiving a packet 
         */
        void NotifyReceptionStart();
        
        /**
         * Notify the MAC that the PHY finished a reception with an error
         */
        void NotifyReceptionEndError();

        /**
         * Notify the MAC that the PHY finished a reception successfully
         * @param packet the received packet 
         */
        void NotifyReceptionEndOk(Ptr<Packet> packet);

        /**
         * Set the phy rate to calculate
         */
        void SetPhyRate(const DataRate rate);

        /**
         * This class doesn't talk directly with the underlying channel (a
         * dedicated PHY class is expected to do it), however the NetDevice
         * specification features a GetChannel() method. This method here
         * is therefore provide to allow CognitiveGeneralNetDevice::GetChannel() to have
         * something meaningful to return.
         *
         * @param channel the underlying channel
         */
        void SetChannel(Ptr<Channel> channel);

        /**
         * set the callback used to instruct the lower layer to start TX
         * @param c 
         */
        void SetPhyStartTxCallback(PhyTxStartCallback c);

        /**
         * set the callback used to get the result of medium sensing 
         * @param c  
         */
        void SetCcaResultCallback(SensingResultCallback c);

        /**
         * set the callback for stopping the work of the phy
         * device during sensing period
         * @param c
         */
        void SetStopWorkCallback(StopWorkCallback c);

        /**
         * @brief Set the callback for getting the energy
         * @param c the callback
         */
        void SetGetRemainingEnergyCallback(GetRemainingEnergyCallback c);

        /**
         * @brief Set the current data
         * channel callback
         * @param c the callback
         */
        void SetCurrentDataChannelCallback(CurrentDataChannelCallback c);

        /**
         * @brief Set the Phy object which is attached to this device.
         * This object is needed so that we can set/get attributes and
         * connect to trace sources of the PHY from the net device.
         * @param phy the Phy object attached to the device.
         */
        void SetPhy(Ptr<Object> phy);
        
        /**
         * @return a reference to the PHY object embedded in this NetDevice.
        */
        Ptr<Object> GetPhy() const;
         
        /**
         * @brief sensing the spectrum during DIFS
         * @return the result of DIFS Sensing 
         */
         
        void DIFSPhase();
         
         /**
          * @brief the result of DIFSPhase
          */
         void DIFSDecision();

        /**
         * @brief the sending the rts 
         */
        void SendRTS();

        /**
         * @brief receiving an rts
         */
        void ReceiveRTS();
        /**
         * @brief the backOff Phase
         */
        void BackOffPhase();

        /**
         * @brief the Doer the back off process
         */
        void DoBackOffProcedure();

        /**
         * @brief ended backoff
         */
        void EndBackOff();
        
        /**
         * @brief CTS Received
         */
        void ReceiveCTS();

        /**
         * @brief Sending CTS
         */
        void SendCTS();

        /**
         * @brief Data transmission
         */
        void TransmitData() ;

        /**
         * @brief data reception
         */
        void ReceiveData();

        /**
         * @brief Send the ACK
         */
        void SendAck();

        /**
         * @brief Received an ACK
         */
        void ReceiveAck();

        /**
         * @brief Stop the transmission
         * and reception for the sensing
         * phase 
         */
        void StopWork(Time stopTime);

        /**
         * @brief the total number of generated packets
         * over all class objects
         */

        static uint32_t NumOfGenPackets();

        /**
         * @brief the total number of received packets
         * over all class objects
         */

        static uint32_t NumOfRecPackets();


        /**
         * @brief the total latency sum of all packets
         */
        static double TotalLatency();

        /**
         * @brief the sensing order from the spectrum module
         */
        double SpectrumControlSense(uint16_t Index);

        /**
         * @brief Set the threshold for deciding channel 
         * statur 
         * @param threshold the thresold
         */
        void SetThreshold(double threshold);

        /**
         * @brief change the state
         * @param s 
         */
        void ChangeState(CognitiveState s);

        /**
         * @brief Get the remaining energy
         * @return the energy
         */
        double GetReamainingEnergy();

        /**
         * @brief Get the cluster info
         * to set the CADC and CBDC
         */
        void SetClusterInfo(uint16_t CADC , uint16_t CBDC , Address CHaddress);

        /**
         * @brief allow or forbid
         * transmission
         * @param b true if transmission
         * allowed
         */
        void TransmissionPermission(bool b);

        /**
         * @brief Set the kind of 
         * device to be for data
         * or control
         * @param b true if it's
         * data device and false
         *  for control
         */
        void SetDeviceKind(bool b);

        /**
         * @brief Sending a frame
         * @param frame 
         */
        void SendFrame(Ptr<MacDcfFrame> frame);

        /**
         * @brief Set the routing 
         * unite
         * @param routingUnite 
         */
        void SetRoutingUnite(Ptr<CognitiveRoutingUnite> routingUnite);

        /**
         * @brief drop the current 
         * packet
         */
        void DropPacket();

        /**
         * @brief get PDR info
         */
        static void GetPDRInfo();



        void SetIfIndex(const uint32_t index) override;
        uint32_t GetIfIndex() const override;
        Ptr<Channel> GetChannel() const override;
        bool SetMtu(const uint16_t mtu) override;
        uint16_t GetMtu() const override;
        void SetAddress(Address address) override;
        Address GetAddress() const override;
        bool IsLinkUp() const override;
        void AddLinkChangeCallback(Callback<void> callback) override;
        bool IsBroadcast() const override;
        Address GetBroadcast() const override;
        bool IsMulticast() const override;
        bool IsPointToPoint() const override;
        bool IsBridge() const override;
        bool Send(Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber) override;
        bool SendFrom(Ptr<Packet> packet,const Address& source,const Address& dest,uint16_t protocolNumber) override;
        Ptr<Node> GetNode() const override;
        void SetNode(Ptr<Node> node) override;
        bool NeedsArp() const override;
        void SetReceiveCallback(NetDevice::ReceiveCallback cb) override;
        Address GetMulticast(Ipv4Address addr) const override;
        Address GetMulticast(Ipv6Address addr) const override;
        void SetPromiscReceiveCallback(PromiscReceiveCallback cb) override;
        bool SupportsSendFrom() const override;

        private :
        void DoDispose() override ;

        /**
         * start the transmission of a packet by contacting the PHY
         */
        void StartTransmission(Ptr<Packet> packet);

        std:: queue <Ptr<MacDcfFrame>> *m_queue; // !<the packets' queue

        TracedCallback<Ptr<const Packet>> m_MacTxTrace;
        TracedCallback<Ptr<const Packet>> m_MacTxDropTrace;
        TracedCallback<Ptr<const Packet>> m_MacPromiscRxTrace;
        TracedCallback<Ptr<const Packet>> m_MacRxTrace;
        TracedCallback<Ptr<const Packet>> m_MacCcaBusyTrace;
        Ptr<Node> m_node;              // !< the node owning this net device 
        Ptr<Channel> m_channel;        // !< the channel where the net device associated 
        Mac48Address m_address;        // !< MAC address
        Mac48Address m_paddress;       // !< Peer MAC address
        NetDevice::ReceiveCallback m_rxCallback;                               //!< Rx Callback 
        NetDevice::PromiscReceiveCallback m_promiscRxCallback;            //!< Promiscuous Rx callback
        PhyTxStartCallback m_phyTxStartCallback;      //!< the TX start Callback
        SensingResultCallback m_sensingResult;        //!< the sensing result 
        StopWorkCallback m_stopWork;                 //!< stop work of the phy     
        GetRemainingEnergyCallback m_remainingEnergy; //!< get the remaining energy callback 
        CurrentDataChannelCallback m_currentDataChannel; //!< set the current data channel
        TracedCallback<> m_linkChangeCallbacks;       //!< the link change Callback;
        uint32_t m_ifIndex;                 // !< the interface Index
        mutable uint32_t m_mtu;             // !< NetDevice MTU
        bool m_linkUp;                      // !< true if link is up
        CognitiveState m_state;             // !< state of the net device 
        Ptr<MacDcfFrame> m_data;            // !< the data packet being sent
        Ptr<MacDcfFrame> m_rdata;           // !< the data packet being recerived
        Ptr<CognitiveRoutingUnite> m_routingUnite;//!< the routing unite connect to the device
        Ptr<Object> m_phy;                  // !< the physical layer object
        Ptr<UniformRandomVariable> m_rv ;   // random variable for the backoff process
        bool m_IhaveChannel;                // !< boolean to tell that I have cluster
        uint16_t m_CW;                      //!< the contention window size
        uint16_t m_CADC;                    //!< the common active data channel 
        uint16_t m_CBDC;                    //!< the common active data channel
        double m_threshold;                 //!< threshold for the carrier sense 
        DataRate m_rate;                    //!< phy date rate
        uint32_t m_backOffSlots;            //!< number of back off slots
        inline static std::map<uint32_t,Ptr<MacDcfFrame>> m_map;  //!< the map for the packets sent
        inline static std::set<uint32_t> m_dataSet;     //!< the set containing the packets send from protocol
        double m_senseRes;          
        bool m_currentTX ;                  //!< boolean to know if we have a packet to transmit now 
        bool m_backoff;                     //!< boolean to know that we are in backoff phase 
        bool m_dataDevice;                  //!< boolean to indicate if the net device is for data or control
        inline static uint32_t sentPackets = 0;
        inline static uint32_t recPackets = 0;
        EventId m_sendPhase ;               //!< time to retry the transmission
        EventId m_nav ;                     //!< virtual carrier sense timers
        EventId m_dropPacket;               //!< timer for dropping a packet
        inline static double latency = 0.0; //!< the total latency over all packets
        Address m_CHaddress;                //!< the address of the cluster head     
        Time m_dropTime;                    //!< the time to drop a packet
        
    };

}

#endif // COGNTITVE_GENERAL_NET_DEVICE