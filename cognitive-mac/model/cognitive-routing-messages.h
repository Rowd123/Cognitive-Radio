/*
 * Copyright (c) 2025 Telecommunications Lab, Higher Institiute for Applied Sciences and Technology , Damascus.
 *
 *
 * Author: Rida Takla <raidotakla@gmail.com>
 */
#ifndef COGNITIVE_ROUTING_MESSAGES
#define COGNITIVE_ROUTING_MESSAGES

#include "cognitive-mac-constants.h"


#include <ns3/object.h>
#include <ns3/address.h>
#include <ns3/packet.h>

#include <iostream>
#include <map>
namespace ns3
{

    class CognitiveRoutingMessage : public Object
    {
        public:
            CognitiveRoutingMessage();              //!< the constructor
            ~CognitiveRoutingMessage() override;    //!< the destructor

            /**
             * @brief Get the Type ID
             * @return the Object Type ID
             */
            static TypeId GetTypeId();
            
            /**
             * @brief Set the source address
             * @param address of the source
             */
            void SetSourceAddress(Address address);

            /**
             * @brief Set the destination address
             * @param address of the destination 
             */
            void SetDestinationAddress(Address address);

            /**
             * @brief Set the required address
             * @param address of the required
             * node
             */
            void SetRequiredAddress(Address address);

            /**
             * @brief Set the type of msg
             * @param type the type
             */
            void SetMsgType(RoutingMsgType type);

            /**
             * @brief Set the packet
             * @param pkt the packet
             */
            void SetPacket(Ptr<Packet> pkt);
            
            /**
             * @brief Set the link 
             * with the error
             * @param link the pair
             * representing the link
             */
            void SetErrorLink(std::pair<Address,Address> link);

            /**
             * @brief Get the Source
             * Address
             * @return address
             */
            Address GetSourceAddress();

            /**
             * @brief Get the destination 
             * address
             * @return address
             */
            Address GetDestinationAddress();

            /**
             * @brief Get the address
             * of the required node
             * @return address
             */
            Address GetRequiredAddress();

            /**
             * @brief Get the msg 
             * type
             * @return the type
             */
            RoutingMsgType GetMsgType();
            
            /**
             * @brief Get the packet
             * @return the packet
             */
            Ptr<Packet> GetPacket();

            /**
             * @brief Get the link with 
             * the error
             */
            std::pair<Address,Address> GetErrorLink();


            

        protected:
            void DoDispose() override;

        private:

        double TotDelay; //!< the total delay of the link

        Address m_fromAddress;      //!< the destination address
        Address m_toAddress;        //!< the source address
        Address m_requiredAddress;  //!< the address of the requested node

        RoutingMsgType m_type;      //!< the message type

        Ptr<Packet> m_pkt;          //!< the packet holding the msg

        std::pair<Address,Address> m_errorLink; //!< the link containing an error
        

    };
}


#endif // COGNITIVE_MAC_ROUTING_MESSAGES