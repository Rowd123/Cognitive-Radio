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
      void SetSender(const Mac48Address sender);

      /**
       * Set the receiver Address
       * @param receiver the address of the receiver
       */
      void SetReceiver(const Mac48Address receiver);

      /**
       * Set the duration
       */
      void SetDuration(const Time duration);

      /**
       * Set the kind
       */
      void SetKind(FrameType typ);
      /**
       * Get the Packet
       * @return the packet of this frame
       */
      Ptr<Packet> GetPacket();

      /**
       * Get the Sender Address
       * @return the address of the sender
       */
      Mac48Address GetSender();
      
      /**
       * Get the receiver address
       * @return address of the sender
       */
      Mac48Address GetReceiver();

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


      protected:
      void DoDispose() override;
      
      private:
      Ptr<Packet>  m_packet;       //!< the packet of the data frame
      Mac48Address m_TxAddress;    //!< the address of the sender
      Mac48Address m_RxAddress;    //!< the address of the receiever
      Time     m_duration;         //!< the duration of the transmission
      Time     m_creationTime ;    //!< the creation time of the packet
      Time     m_arrivalTime;      //!< the time for finishing reception
      FrameType    m_kind;         //!< the kind of the frame
      uint32_t m_originalPkt;      //!< the origial packet uid

};

}

#endif // MAC_FRAMES