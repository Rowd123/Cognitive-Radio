/*
 * Copyright (c) 2025 Telecommunications Lab, Higher Institiute for Applied Sciences and Technology , Damascus.
 *
 *
 * Author: Rida Takla <raidotakla@gmail.com>
 */
#ifndef COGNITIVE_ROUTING_UNITE
#define COGNITIVE_ROUTING_UNITE


#include "cognitive-routing-messages.h"

#include <ns3/object.h>

#include <map>
#include <vector>

namespace ns3
{

typedef Callback<void,Ptr<MacDcfFrame>> SendFrameCallback;
typedef Callback<void,Ptr<Packet>> SendPacketCallback;
typedef Callback<bool,Address> IsClusterMemberCallback;
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
             * @brief Enable Routing
             * @param b true if routint
             * is enabled
             */
            void EnableRouting(bool b);
            
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

            /**
             * @brief set the callback
             * to check if the node 
             * is cluster member
             * @param c the callback
             */
            void SetIsClusterMemberCallback(IsClusterMemberCallback c);

            /**
             * @brief start the route
             * discovery process
             * @param address the wanted
             * address
             */
            void StartRouteDiscovery(Address address);

            /**
             * @brief send route discovery
             * @param frame received
             * frame
             */
            void ReceiveRouteDiscoveryRequest(Ptr<CognitiveRoutingMessage> frame);

            /**
             * @brief receive route reply
             * message 
             * @param frame the frame 
             */
            void ReceiveRouteReply(Ptr<CognitiveRoutingMessage> frame);
            
            /**
             * @brief end of the route
             * discovery process
             * @param frame the message
             */
            void EndRouteDiscoveryProcess(Ptr<CognitiveRoutingMessage> frame);
            
            /**
             * @brief send link outage msg
             * @param frame the message
             */
            void SendRouteError(Address address);

            /**
             * @brief receive broken link
             * msg
             * @param frame the message
             */
            void ReceiveRouteError(Ptr<CognitiveRoutingMessage> frame);

            /**
             * @brief Calculate the link
             * delay between this node and
             * the next one
             */
            double CalculateLinkDelay();

            /**
             * @brief set the node 
             * as cluster head
             * @param b boolean true
             * if the node is cluster
             * head
             */
            void SetClusterHeadStatus(bool b);

            /**
             * @brief set the node as
             * gateway node
             * @param b boolean true 
             * if the node is gateway
             * node
             */
            void SetGatewayStatus(bool b);
            
            /**
             * @brief send the packets
             * to the destination after
             * finding the address
             */
            void SendPendingPackets();
            
        protected:
            void DoDispose() override;

        private:
            Address m_CHaddress;        //!< the cluster head address
            Address m_address;          //!< the node address
            
            bool m_IhaveCluster;        //!< boolean to indicate having a cluster now
            bool m_routingEnabled;      //!< boolean to indicate routing enabled or not
            bool m_ImClusterHead;       //!< boolean to know if the node is a cluster head
            bool m_ImGateway;           //!< boolean to know if the node is a gateway

            inline static uint32_t ReceivedPackets = 0; //!< number of packets reaching their final destination
            inline static uint32_t SentPackets = 0;     //!< number of packets created 

            DataRate m_dataRate;        //!< the data rate of the PHY
            
            SendFrameCallback m_dataFrameCallback;  //!< sending packet via the data device
            SendFrameCallback m_ctrlFrameCallback;  //!< sending packet via the control device
            SendPacketCallback m_ctrlAppSendPacketCallback; //!< sending the packet to contorl app
            IsClusterMemberCallback m_IsClusterMemberCallback;  //!< used to know if the node is CM

            std::map<Address,Address> m_routingTable;       //!< the routing table and the total delay
            std::map<Address,EventId> m_timers;             //!< the timers for the validity of addresses  
            std::map<Address,double> m_minDelay;            //!< the minimum delay of the path
            
            static std::map<uint32_t,Ptr<CognitiveRoutingMessage>> msgs; //!< the messages sent by routing layer 


            std::vector<Ptr<MacDcfFrame>> *m_vector ;         //!< the vector of the routing unite

            std::set<Address> m_pendingReq ;                 //!< set containing the pending route requests

            Time m_ExpiracyTime = Seconds(600);             //!< the expiracy date of the 


    };
}


#endif // COGNITIVE_MAC_ROUTING_UNITE