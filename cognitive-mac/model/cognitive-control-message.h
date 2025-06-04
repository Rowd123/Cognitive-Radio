/*
 * Copyright (c) 2025 Telecommunications Lab, Higher Institiute for Applied Sciences and Technology , Damascus.
 *
 *
 * Author: Rida Takla <raidotakla@gmail.com>
 */
#ifndef COGNITIVE_CONTROL_FRAME
#define COGNITIVE_CONTROL_FRAME

#include "cognitive-mac-constants.h"

#include <ns3/object.h>
#include <ns3/packet.h>
#include <ns3/address.h>
#include <ns3/nstime.h>
#include <ns3/data-rate.h>
#include <iostream>
#include <map>

namespace ns3
{

/**
 * this class is for the implementation of the 
 * cognitive general control message
 * NCCI,CH_ANM,GH_ANM,CH_REQ,JOIN_REQ 
 */


typedef std::map<uint16_t,double>* ChannelQuality;
typedef std::map<Address,std::set<uint16_t>>* NeighborNodeConnectivity;
typedef std::map<Address,std::pair<uint16_t,uint16_t>>* NeighborClusterReachability;

class CognitiveControlMessage : public Object
{
    public:
      CognitiveControlMessage();
      
      ~CognitiveControlMessage();

      enum Kind
      {
        NCCI,
        CH_ANM,
        CH_REQ,
        JOIN_REQ,
        GH_ANM
      };


      /**
       * Register this type
       * @return the type ID 
       */
      static TypeId GetTypeId();

      /**
       * @brief set the packet 
       * of the msg
       * @param pkt the packet
       */
      void SetPacket(Ptr<Packet> pkt);

      /**
       * @brief Get the packet
       * of the msg
       */
      Ptr<Packet> GetPacket();

      /**
       * @brief set the address
       * of the source
       * @param address the address
       */
      void SetSourceAddress(Address address);

      /**
       * @brief get the address
       * of the source
       * @return address
       */
      const Address GetSourceAddress();

      /**
       * @brief set the destination
       * address
       * @param address of the receiver
       */
      void SetDestinationAddress(Address address);

      /**
       * @brief get the address of
       * the destination
       * @return address
       */
      const Address GetDestinationAddress();

      /**
       * @brief Set the channel
       * quality map
       * @param mp the map
       */
      void SetChannelQualityMap(ChannelQuality mp);

      
      /**
       * @brief get the channel
       * quality map
       * @return the map
       */
      ChannelQuality GetChannelQualityMap();

      /**
       * @brief Set the neighboring
       * channel connectivity map
       * @param mp the map
       */
      void SetNeighborNodeConnectivityMap(NeighborNodeConnectivity mp);

      /**
       * @brief Set the neighboring
       * channel connectivity map
       * @return the map
       */
      NeighborNodeConnectivity GetNeighborNodeConnectivityMap();


      /**
       * @brief Set the neighboring 
       * cluster reachability map
       * @param mp the map
       */

       void SetNeighborClusterReachabilityMap(NeighborClusterReachability mp);

      /**
       * @brief Set the neighboring 
       * cluster reachability map
       * @return the map
       */

       NeighborClusterReachability GetNeighborClusterReachabilityMap();

       /**
        * @brief set the creation
        * time of the msg
        * @param time the time
        */
       void SetCreationTime(Time time);

       /**
        * @brief get the
        * creation time of the msg
        * @return the time 
        */
       Time GetCreationTime();

      /**
       * @brief set the intial energy
       * required for the NCCI msg
       * @param energy double
       */
      void SetEnergy(double energy);

      /**
       * @brief Get the inital energy
       * required for the processing 
       * of the NCCI msg
       * @return current energy
       */
      double GetEnergy();

      /**
       * @brief Set the kind
       * of the message
       * @param kind 
       */
      void SetKind(Kind kind);

      /** 
       * @brief Get the
       * kind of the message
       * @return the kind
      */
      Kind GetKind();

      /**
       * @brief  Set CADC
       * @param channel the index 
       * of the channel 
       */
      void SetCADC(uint16_t channel);

      /**
       * @brief Get CADC
       * @return index of the channel
       */
      uint16_t GetCADC();
      
      /**
       * @brief  Set CBDC
       * @param channel the index 
       * of the channel 
       */
      void SetCBDC(uint16_t channel);

      /**
       * @brief Get CBDC
       * @return index of the channel
       */
      uint16_t GetCBDC();

      protected:
      void DoDispose() override;
      
      private:

      Ptr<Packet> m_pkt;        //!< the packet of the msg

      Address m_sourceAddress;  //!< the address of the source of the msg
      Address m_destinAddress;  //!< the address of the destination of the msg

      ChannelQuality m_channelQuality; //!< the map of the channle quality map
      NeighborNodeConnectivity m_neighborNodeConnectivty; //!< the neighbor connectivity map
      NeighborClusterReachability m_neighborClusterReachability; //!< the neighbor cluster reachability of the map

      Time m_creationTime;              //!< the creation time of the message

      double m_energy;                  //!< the remaining energy

      uint16_t m_CADC ;                 //!< the index of the common active data channel
      uint16_t m_CBDC ;                 //!< the index of the common backup data channel

      CognitiveControlMessage::Kind m_kind;            //!< the kind of the message

};

}

#endif // MAC_FRAMES