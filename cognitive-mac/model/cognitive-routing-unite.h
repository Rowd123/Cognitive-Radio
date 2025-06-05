/*
 * Copyright (c) 2025 Telecommunications Lab, Higher Institiute for Applied Sciences and Technology , Damascus.
 *
 *
 * Author: Rida Takla <raidotakla@gmail.com>
 */
#ifndef COGNITIVE_ROUTING_UNITE
#define COGNITIVE_ROUTING_UNITE


#include "mac-frames.h"

#include <ns3/object.h>
#include <ns3/data-rate.h>

namespace ns3
{

typedef Callback<void,Ptr<MacDcfFrame>> SendFrameCallback;
typedef Callback<void,Ptr<Packet>> SendPacketCallback;

    class CognitiveRoutingUnite : public Object
    {
        public:
            CognitiveRoutingUnite();              //!< the constructor
            ~CognitiveRoutingUnite() override;    //!< the destructor

            /**
             * @brief Get the Type ID
             * @return the Object Type ID
             */
            static TypeId GetTypeId();

            /**
             * @brief receiving a packet from the application
             * layer
             * @param packet the packet need to be sent
             * @param source the original source address
             * @param dest the original destination address
             * @param protocolNumber the protocol number
             */
            void SendPacket(Ptr<Packet> packet, const Address source, const Address dest, uint16_t protocolNumber);

            /**
             * @brief send the frame via
             * the MAC layer
             * @param frame to be sent
             */
            void SendFrame(Ptr<MacDcfFrame> frame); 

            /**
             * @brief receive a frame from the MAC
             * layer
             * @param frame the received frame
             */
            void ReceiveFrame(Ptr<MacDcfFrame> frame);

            /**
             * @brief set the cluster as the 
             * address 
             * @param address the cluster head
             * address
             */

            void SetCluster(Address address);

            /**
             * @brief unset the cluster 
             */

            void UnSetCluster();

            /**
             * @brief Set the data rate 
             * of the PHY
             */
            void SetDataRate(const DataRate dataRate);

            /**
             * @return number of created
             * packets 
             */
            static uint32_t NumSendPackets();

            /**
             * @return number of packets
             * reaching the final destination
             */
            static uint32_t NumReceivedPackets();

            /**
             * @brief set the address 
             * of the node
             */
            void SetAddress(const Address address);

            /**
             * @brief set the callback
             * for sending a frame
             * via data net device
             * @param c the callback
             */
            void SetSendDataFrameCallback(SendFrameCallback c);

            /**
             * @brief set the callback
             * for sending a frame
             * via ctrl net device
             * @param c the callback
             */
            void SetSendCtrlFrameCallback(SendFrameCallback c);

            /**
             * @brief set the callback
             * for sending a packet
             * for the control app
             * @param c the callback
             */
            void SetCtrlAppSendPacketCallback(SendPacketCallback c);

        
        protected:
            void DoDispose() override;

        private:
            Address m_CHaddress;        //!< the cluster head address
            Address m_address;          //!< the node address
            
            bool m_IhaveCluster;        //!< boolean to indicate having a cluster now
            bool m_routingEnabled;      //!< boolean to indicate routing enabled or not

            inline static uint32_t ReceivedPackets = 0; //!< number of packets reaching their final destination
            inline static uint32_t SentPackets = 0;     //!< number of packets created 

            DataRate m_dataRate;        //!< the data rate of the PHY
            
            SendFrameCallback m_dataFrameCallback;
            SendFrameCallback m_ctrlFrameCallback;
            SendPacketCallback m_ctrlAppSendPacketCallback;


            

    };
}


#endif // COGNITIVE_MAC_ROUTING_UNITE