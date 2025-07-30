/*
 * Copyright (c) 2025 Telecommunications Lab, Higher Institiute for Applied Sciences and Technology , Damascus.
 *
 *
 * Author: Rida Takla <raidotakla@gmail.com>
 */
#ifndef MAC_FRAMES
#define MAC_FRAMES

#include "cognitive-mac-constants.h"

#include <ns3/object.h>
#include <ns3/packet.h>
#include <ns3/address.h>
#include <ns3/nstime.h>
#include <ns3/data-rate.h>

namespace ns3
{

/**
 * this class is for the implementation of the DCF MAC protocl
 * it has four kinds of frams RTS,CTS,DATA,ACK
 */

class MacDcfFrame : public Object
{
    public:
      MacDcfFrame();
      
      ~MacDcfFrame();

      /**
       * Register this type
       * @return the type ID 
       */
      static TypeId GetTypeId();

      /**
       * Set the Packet of the frame 
       * @param the packet to be sent
       */
      void SetPacket(Ptr<Packet> packet);

      /**
       * Set the Sender Address
       * @param sender the address of the sender
       */
      void SetOriginalSender(const Mac48Address sender);

      /**
       * Set the receiver Address
       * @param receiver the address of the receiver
       */
      void SetOriginalReceiver(const Mac48Address receiver);

      /**
       * set the current sender
       * hop address
       * @param curSender the address of the sender
       */
      void SetCurrentSender(const Mac48Address curSender);

      /**
       * set the current receiver
       * hop address
       * @param curReceiver the address of the receiver
       */
      void SetCurrentReceiver(const Mac48Address curReceiver);

      /**
       * Set the duration
       */
      void SetDuration(const Time duration);

      /**
       * Set the kind
       */
      void SetKind(FrameType typ);

      /**
       * Set the protocol
       * number 
       */
      void SetProtocolNumber(uint16_t protocolNumber);
      /**
       * Get the Packet
       * @return the packet of this frame
       */
      Ptr<Packet> GetPacket();

      /**
       * Get the Sender Address
       * @return the address of the sender
       */
      Mac48Address GetOriginalSender();
      
      /**
       * Get the receiver address
       * @return address of the sender
       */
      Mac48Address GetOriginalReceiver();

      /**
       * Get the current sender address
       * @return the address of the current sender
       */
      Mac48Address GetCurrentSender();

      /**
       * Get the current receiver address
       * @return the address of the current receiver
       */
      Mac48Address GetCurrentReceiver();

      /**
       * Get the duration of the transmission
       * @return the time of the transmission
       */
      Time GetDuration();

      /**
       * Get the kind of the frame RTS, CTS, DATA, ACK
       * @return enum the frame type
       */
      FrameType GetKind() ;

      /**
       * Getting the latency of the packet
       * @return time latency
       */
      Time CalculateLatency();

      /**
       * Getting the number of 
       * protocol 
       * @return protocol number
       */
      uint16_t GetProtocolNumber();

      /**
       * Ge the uid of the original packet
       * @return uid of the packet
       */
      uint32_t GetOriginalPacketUid();

      /**
       * Set the creation time of the packet
       * @param time the time of the creation 
       */
      void SetCreationTime(const Time  time);

      /**
       * Set the final destination arrival time of the packet
       * @param time the time of the creation 
       */
      void SetArrivalTime(const Time  time);

      /**
       * set the uid of the main packet 
       * @param uid 
       */
      void SetOriginalPacketUid(uint32_t uid);

      /**
       * set the message type if it was
       * a routing message
       * @param type 
       */
      void SetMsgType(RoutingMsgType type);

      /**
       * get the routing message
       * type
       */
      RoutingMsgType GetMsgType();

      /**
       * set the required address
       * for routing messages
       * @param address the address 
       */
      void SetRequiredAddress(Address address);

      /**
       * get the required address
       */
      Address GetRequiredAddress();

      /**
       * Set the delay
       */
      void SetDelay(double del);

      /**
       * Get the total delay
       */
      double GetDelay();

      protected:
      void DoDispose() override;
      
      private:
      Ptr<Packet>  m_packet;       //!< the packet of the data frame
      Mac48Address m_TxAddress;    //!< the address of the sender
      Mac48Address m_RxAddress;    //!< the address of the receiever
      Mac48Address m_curTxAddress; //!< the address of the current sender hop
      Mac48Address m_curRxAddress; //!< the address of the current receiver hop
      Address m_requiredAddress;   //!< the required address 
      Time     m_duration;         //!< the duration of the transmission
      Time     m_creationTime ;    //!< the creation time of the packet
      Time     m_arrivalTime;      //!< the time for finishing reception
      FrameType    m_kind;         //!< the kind of the frame
      RoutingMsgType m_rtType;     //!< the type of the routing message
      uint32_t m_originalPkt;      //!< the origial packet uid
      uint16_t m_protocolNum;      //!< the protocol number 
      double m_totDelay;           //!< the total delay(used for routing messages)

};

}

#endif // MAC_FRAMES