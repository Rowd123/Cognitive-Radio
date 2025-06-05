/*
 * Copyright (c) 2025 Telecommunications Lab, Higher Institiute for Applied Sciences and Technology , Damascus.
 *
 *
 * Author: Rida Takla <raidotakla@gmail.com>
 */
#ifndef COGNITIVE_CONTROL_APPLICATION
#define COGNITIVE_CONTROL_APPLICATION

#include "cognitive-general-net-device.h"
#include "spectrum-control-module.h"
#include "cognitive-control-message.h"
#include "cognitive-routing-unite.h"
#include "ns3/application.h"
#include "ns3/address.h"
#include "ns3/simulator.h"
#include "ns3/net-device.h"

namespace ns3
{

typedef Callback<double , uint16_t> ChannelSensingCallback ;
typedef Callback<void , Time> StartSensingPeriodCallback ; 
typedef Callback<double> GetRemainingEnergyCallback ;
typedef Callback<void,uint16_t,uint16_t,Address> SetCommonDataChannelsCallback;

    class CognitiveControlApplication : public Application
    {
        public:

          struct NCCI_MSG
          {
            Address address;                                //!< the source address
            double energy;                                  //!< the residual energy
            ChannelQuality channelQuality;                  //!< the channel quality info
            NeighborNodeConnectivity neighborInfo;          //!< the neighboring node and their available channels
            NeighborClusterReachability neighborCluster;    //!< the neighboring clusters and theis CADCs
            Time createTime;                                //!< the time of the creation of the MSG
            Ptr<Packet> packet;                             //!< the packet containing the message
          };


          CognitiveControlApplication();
          ~CognitiveControlApplication();

          static TypeId GetTypeId();

          /**
           * @brief Set the spectrum control
           * module
           * @param specturmModule
           */
          void SetSpectrumControlModule(Ptr<SpectrumControlModule> spectrumModule);

          /**
           * @brief Set the data net device
           * @param dataDevice
           */
          void SetDataDevice(Ptr<NetDevice> dataDevice);

          /**
           * @brief Set the control net device
           * @param controlDevice 
           */
          void SetControlDevice(Ptr<NetDevice> controlDevice);

          /**
           * @brief set the channel sensing callback
           * @param c the callback
           */
          void SetChannelSensingCallback(ChannelSensingCallback c);

          /**
           * @brief StartSensingPeriodCallback
           * @param c the callback
           */
          void SetStartSensingPeriodCallback(StartSensingPeriodCallback c);

          /**
           * @brief set the remaining energy value
           * callback
           * @param c the callback
           */
          void SetGetRemainingEnergyCallback(GetRemainingEnergyCallback c);

          /**
           * @brief set the CADC and CBDS
           * callback
           * @param c the callback
           */
          void SetSetCommonDataChannelsCallback(SetCommonDataChannelsCallback c);


          /**
           * @brief get the sensing result of a specified channel
           * @param Index of the channel wanted 
           */
          double SelectedChannelResult(uint16_t Index);

          /**
           * @brief receiving the result of the 
           * Q-learning
           * @param Qtable resulting vector
           */
          void ReceivingQtable(std::map<uint16_t,double> Qtable);

          /**
           * @brief starting the sensing period
           */
          void StartSensingPeriod();

          /**
           * @brief Sending a control message
           * @param kind of the control message
           */
          void SendMsg(CognitiveControlMessage::Kind kind);
        
          /**
           * @brief Receive a NCCI message
           */
          void ReceiveMsg(Ptr<CognitiveControlMessage> msg);

          /**
           * @brief Delete NCCI message
           * after the NCCI time duration
           * finishes 
           */
          void DeleteCtrlMsg(Address address , CognitiveControlMessage::Kind kind);

          /**
           * @brief Setting the address of the 
           * Ctrl net device
           * @param address the address
           */
          void SetAddress(Address m_address);

          /**
           * @brief set the routing unite
           * representing the network 
           * layer
           * @param routingUnite the routing unite
           */
          void SetCognitiveRoutingUnite(Ptr<CognitiveRoutingUnite> routingUnite);

          /**
           * @brief get the routing unite
           * representing the network
           * layer
           * @return the routing unite
           */
          Ptr<CognitiveRoutingUnite> GetCognitiveRoutingUnite();

          /**
           * @brief receiving a packet by the 
           * MAC and then forwarding it to the
           * control application
           * @param pkt the received packet
           */
          void ReceiveControlMsg(Ptr<Packet> pkt);

          /**
           * @brief set the V calculation parameters
           * @param beta1 energy fraction parameter
           * @param beta2 channel fitness fraction parameter
           * @param beta3 number of neighboring clusters parameter
           * @param beta4 number of neighboring nodes parameter
           */
          void SetVparameters(double beta1 , double beta2,
                              double beta3 , double beta4);
          
          /**
           * @brief Set the maximum possible energy 
           */
          void SetMaximumEnergy(double energy);

          /**
           * @brief Set the maximum number of 
           * neighboring clusters
           * @param number 
           */
          void SetMaximumNeighboringClusters(uint16_t number);

          /**
           * @brief Set the maximum number
           * of neighboring nodes
           * @param number 
           */
          void SetMaximumNeighboringNodes(uint16_t number);

          private:

          void StartApplication() override;
          void StopApplication() override;

          /**
           * @brief updating the neighbor node
           * connectivity periodically 
           */
          void UpdateNeighborsInfoTables();

          /**
           * @brief choose the cluster 
           * to send the the CH_REQ
           */
          void ChooseMyCluseterCandidate();

          /**
           * @brief evaluating the CH_REQs
           * to decide being a CH
           */
          void EvaluateCH_REQs();

          /**
           * @brief evaluate the CH_ANMs
           * to decide the cluster head
           */
          void EvaluateCH_ANMs();
          /**
           * @brief calculate the V values
           * for determining the CH 
           */
           void CalculateVvalues();

           /**
            * @brief calculate the 
            * V value for a one node
            * @param address of the node
            */
           double DoCaculateVvalue(Ptr<CognitiveControlMessage> msg);
          
          
          Ptr<SpectrumControlModule> m_spectrumControlModule;  //!< the spectrum control module 
          Ptr<NetDevice> m_dataDevice;                         //!< the net device 
          Ptr<NetDevice> m_controlDevice;                      //!< the control device
          Ptr<CognitiveRoutingUnite> m_routingUnite;           //!< the routing unite

          ChannelSensingCallback m_channelSensingCallback; 
          StartSensingPeriodCallback m_startSensingPeriodCallback;
          GetRemainingEnergyCallback m_getRemainingEnergyCallback;
          SetCommonDataChannelsCallback m_setCommonDataChannelsCallback;


          EventId m_curAction ;                                //!< the current action to be done
          EventId m_resense;                                   //!< event for resensing and updating Q-table
          EventId m_updateneighbors;                           //!< update the neighboring nodes table
          EventId m_updateVtable;                              //!< update the neighboring clusters table

          Time m_resenseTime;                                  //!< period for resensing
          Time m_ctrlMsgDuration;                              //!< the duration until the expiracy of the control message
          Time m_SendNCCIPeriod;                               //!< the period for sending NCCI msg 
          Time m_updateTables;                                 //!< the duration for updating tables
          Time m_initialtime;                                  //!< the initial waiting time for sending first NCCI
          Time m_clusterAge;                                   //!< the age of a cluster
          
          ChannelQuality m_availableChannelQvalues;            //!< available channels and their Q values
          NeighborNodeConnectivity m_neighborsInfo;            //!< neighboring nodes and their available channels
          NeighborClusterReachability m_neighborCluster;       //!< neighboring cluster and their CADC 
          std::map<Address,EventId> m_ncci_Expiracy;           //!< the event of the expiract of the NCCI message
          std::map<Address,EventId> m_ch_anm_Expiracy;         //!< the event of the expiracy of the CH_ANM message 
          std::map<Address,EventId> m_ch_req_Expiracy;         //!< the event of the expiracy of the CH_REQ message
          std::map<Address,Ptr<CognitiveControlMessage>> m_msgs;//!< a map containing received messagess
          std::map<Address,double> m_Vvalues;                  //!< a map containig the Vvalues of the neighboring nodes
          std::map<Address,EventId> m_members_Expiracy;       //!< the event of the expiracy of the JOIN_REQ message                      

          std::vector<double> m_channelsQtable;                //!< the Q table of the channels

          Address m_address;                                   //!< the address of my device
          Address m_CHCaddress;                                //!< the address of candidate cluster head
          Address m_CHaddress;                                 //!< the address of the chosen m_CHaddress

          inline static std::map<uint32_t, Ptr<CognitiveControlMessage>> msgMap; 

          inline static uint16_t m_protocol = 1000;         //!< the protocol port used for control (default)

          uint16_t m_CADC;                                  //!< the common active data channel
          uint16_t m_CBDC;                                  //!< the common backup data channel
          uint16_t m_CHrequests;                            //!< number of cluster head requests 
          uint16_t m_NNmax;                                 //!< maximum number of neighbors
          uint16_t m_NRCmax;                                //!< maximum number of nighboring clusters
          

          double m_beta1,m_beta2,m_beta3,m_beta4;           //!< V calculation parameters
          double m_CFmax;                                   //!< the channel fitness max value
          double m_Emax;                                    //!< the maximum energy of a node
          double m_Etta;                                    //!< the percentile threshold

  
          bool m_ImClusterHead;                             //!< boolean to know if I'm a cluster head
    };
}

#endif  // COGNITIVE_CONTROL_APPLICATION