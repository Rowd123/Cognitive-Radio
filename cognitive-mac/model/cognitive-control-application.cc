/*
 * Copyright (c) 2025 Telecommunications Lab, Higher Institiute for Applied Sciences and Technology , Damascus.
 *
 *
 * Author: Rida Takla <raidotakla@gmail.com>
 */

#include "cognitive-control-application.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("CognitiveControlApplication");
  
NS_OBJECT_ENSURE_REGISTERED(CognitiveControlApplication);

CognitiveControlApplication::CognitiveControlApplication():
      m_spectrumControlModule(nullptr),
      m_resenseTime(Seconds(60)),m_ctrlMsgDuration(Seconds(10)),
      m_SendNCCIPeriod(Seconds(5)),m_updateTables(Seconds(0.1)),
      m_initialtime(Seconds(2)),m_clusterAge(Seconds(5)),
      m_CADC(-1),m_CBDC(-1),m_CHrequests(0),
      m_NNmax(10),m_NRCmax(10),
      m_beta1(0.25),m_beta2(0.25),
      m_beta3(0.25),m_beta4(0.25),
      m_CFmax(20),m_Emax(15000),m_Etta(0.5),
      m_curEnergy(150),m_ImClusterHead(false)
{
      m_availableChannelQvalues = new std::map<uint16_t,double>();
      m_neighborsInfo = new std::map<Address,std::set<uint16_t>>();
      m_neighborCluster = new std::map<Address,std::pair<uint16_t,uint16_t>>();
}

CognitiveControlApplication::~CognitiveControlApplication()
{
      m_spectrumControlModule = nullptr; 
}

TypeId

CognitiveControlApplication::GetTypeId()
{
      static TypeId tid =
        TypeId("ns3::CognitiveControlApplication")
            .SetParent<Application>()
            .SetGroupName("Network")
            .AddConstructor<CognitiveControlApplication>();
      return tid;
}

void

CognitiveControlApplication::SetSpectrumControlModule(Ptr<SpectrumControlModule> spectrumModule)
{
      m_spectrumControlModule = spectrumModule;
}

void

CognitiveControlApplication::SetDataDevice(Ptr<NetDevice> dataDevice)
{
      m_dataDevice = dataDevice;
}

void

CognitiveControlApplication::SetControlDevice(Ptr<NetDevice> controlDevice)
{
      m_controlDevice = controlDevice;
}

void

CognitiveControlApplication::SetChannelSensingCallback(ChannelSensingCallback c)
{
      m_channelSensingCallback = c ;
}

void

CognitiveControlApplication::SetStartSensingPeriodCallback(StartSensingPeriodCallback c)
{
      m_startSensingPeriodCallback = c ;
}

void

CognitiveControlApplication::SetGetRemainingEnergyCallback(GetRemainingEnergyCallback c)
{
      m_getRemainingEnergyCallback = c ;
}

void

CognitiveControlApplication::SetSetCommonDataChannelsCallback(SetCommonDataChannelsCallback c)
{
      m_setCommonDataChannelsCallback = c;
}



double

CognitiveControlApplication::SelectedChannelResult(uint16_t Index)
{
      NS_ASSERT_MSG(m_dataDevice,"there is no data device assigned to the control application");
      NS_ASSERT_MSG(!m_channelSensingCallback.IsNull(),"Set the channel sensing callback");
      return m_channelSensingCallback(Index);
}

void

CognitiveControlApplication::StartApplication()
{
      NS_ASSERT_MSG(m_spectrumControlModule,"you haven't linked the spectrum module yet");
      Simulator::ScheduleNow(&CognitiveControlApplication::StartSensingPeriod,this);
      m_updateneighbors = Simulator::Schedule(m_updateTables,
                                              &CognitiveControlApplication::UpdateNeighborsInfoTables,this);
      m_updateVtable = Simulator::Schedule(m_updateTables,
                                           &CognitiveControlApplication::CalculateVvalues,this);

      Simulator::Schedule(m_initialtime,&CognitiveControlApplication::SendMsg,this,(CognitiveControlMessage::NCCI));
      Simulator::Schedule(m_initialtime+Seconds(0.5),&CognitiveControlApplication::SendMsg,
                                                      this,(CognitiveControlMessage::NCCI));
      Simulator::Schedule(m_initialtime+Seconds(1),&CognitiveControlApplication::ChooseMyCluseterCandidate,this);
      Simulator::Schedule(m_initialtime+Seconds(1.3),&CognitiveControlApplication::EvaluateCH_REQs,this);
      Simulator::Schedule(m_initialtime+Seconds(1.5),&CognitiveControlApplication::EvaluateCH_ANMs,this);
      Simulator::Schedule(m_initialtime,&CognitiveControlApplication::SendNCCIMsg,this);
}


void

CognitiveControlApplication::StopApplication()
{
      m_curAction.Cancel();
      m_resense.Cancel();

}

void

CognitiveControlApplication::SendNCCIMsg()
{
      Simulator::ScheduleNow(&CognitiveControlApplication::SendMsg,
                              this,(CognitiveControlMessage::NCCI));
      Simulator::Schedule(m_clusterAge,&CognitiveControlApplication::SendNCCIMsg,this);
}

void

CognitiveControlApplication::ReceivingQtable(std::map<uint16_t,double> Qtable)
{
      (*m_availableChannelQvalues) = Qtable;
      m_resense = Simulator::Schedule(m_resenseTime,&CognitiveControlApplication::StartSensingPeriod,this);
}

void

CognitiveControlApplication::StartSensingPeriod()
{
     NS_ASSERT_MSG(!m_startSensingPeriodCallback.IsNull(),"the start sensing period isn't connected to the net device");
     m_startSensingPeriodCallback(m_spectrumControlModule->GetBandGroupSensingTime());
     Simulator::ScheduleNow(&SpectrumControlModule::StartWorking,m_spectrumControlModule); 
}

void

CognitiveControlApplication::SendMsg(CognitiveControlMessage::Kind kind)
{

      if(kind==CognitiveControlMessage::NCCI)
      {
            
            NS_ASSERT_MSG(!m_getRemainingEnergyCallback.IsNull(),"you haven't set the get energy callback");

            std::cout << m_node->GetId() << " generating NCCI msg " << Simulator::Now() << '\n'; 

            Ptr<CognitiveControlMessage> msg = CreateObject<CognitiveControlMessage>();
            m_curEnergy = m_getRemainingEnergyCallback();
            Ptr<Packet> pkt = Create<Packet>(NCCIsize);
            msg->SetPacket(pkt);
            msg->SetChannelQualityMap(m_availableChannelQvalues);
            msg->SetNeighborNodeConnectivityMap(m_neighborsInfo);
            msg->SetNeighborClusterReachabilityMap(m_neighborCluster);
            msg->SetCreationTime(Simulator::Now());
            msg->SetEnergy(m_curEnergy);
            msg->SetDestinationAddress(Broadcast);
            msg->SetSourceAddress(m_address);
            msg->SetKind(CognitiveControlMessage::NCCI);
            msg->SetNodeId(m_node->GetId());
            msgMap[pkt->GetUid()] = msg ;
            m_controlDevice->Send(pkt,Broadcast,m_protocol);
      }
      else if(kind==CognitiveControlMessage::CH_ANM)
      {
            
            
            NS_ASSERT_MSG(m_ImClusterHead,"going to send CH_ANM while not a CH");

           // std::cout << m_node->GetId() << " generating CH_ANM msg " << Simulator::Now() << '\n'; 
            
            Ptr<CognitiveControlMessage> msg = CreateObject<CognitiveControlMessage>();
            Ptr<Packet> pkt = Create<Packet>(CH_ANMsize);
            msg->SetCADC(m_CADC);
            msg->SetCBDC(m_CBDC);
            msg->SetSourceAddress(m_address);
            msg->SetDestinationAddress(Broadcast);
            msg->SetCreationTime(Simulator::Now());
            msg->SetPacket(pkt);
            msg->SetKind(CognitiveControlMessage::CH_ANM);
            msgMap[pkt->GetUid()] = msg;
            m_controlDevice->Send(pkt,Broadcast,m_protocol);      
      }
      else if(kind==CognitiveControlMessage::CH_REQ)
      {
            NS_ASSERT_MSG(m_CHCaddress!=m_address,"sending CH_REQ to the same node");
           // std::cout << m_node->GetId() << " generating CH_REQ msg " << Simulator::Now() << ' '<< m_CHCaddress << '\n'; 
            Ptr<CognitiveControlMessage> msg = CreateObject<CognitiveControlMessage>();
            Ptr<Packet> pkt = Create<Packet>(CH_REQsize);
            msg->SetPacket(pkt);
            msg->SetSourceAddress(m_address);
            msg->SetDestinationAddress(m_CHCaddress);
            msg->SetCreationTime(Simulator::Now());
            msg->SetKind(CognitiveControlMessage::CH_REQ);
            msgMap[pkt->GetUid()] = msg;
            m_controlDevice->Send(pkt,m_CHCaddress,m_protocol);
       
      }
      else if(kind==CognitiveControlMessage::JOIN_REQ)
      {
            if(m_CHaddress==m_address){return;}
          //  std::cout << m_node->GetId() << " generating JOIN_REQ msg " << Simulator::Now() << '\n'; 
            Ptr<CognitiveControlMessage> msg = CreateObject<CognitiveControlMessage>();
            Ptr<Packet> pkt = Create<Packet>(JOIN_REQsize);
            msg->SetPacket(pkt);
            msg->SetSourceAddress(m_address);
            msg->SetDestinationAddress(m_CHaddress);
            msg->SetCreationTime(Simulator::Now());
            msg->SetKind(CognitiveControlMessage::JOIN_REQ);
            msgMap[pkt->GetUid()] = msg;
            m_controlDevice->Send(pkt,m_CHaddress,m_protocol); 
      }
      else if(kind==CognitiveControlMessage::GH_ANM)
      {
          //  std::cout << m_node->GetId() << " generating GH_ANM msg " << Simulator::Now() << '\n'; 
            for(auto& i : m_gateways)
            {
                  if(i.second==m_address)
                  {
                        continue;
                  }
                  Ptr<CognitiveControlMessage> msg = CreateObject<CognitiveControlMessage>();
                  Ptr<Packet> pkt = Create<Packet>(GH_ANMsize);
                  msg->SetPacket(pkt);
                  msg->SetSourceAddress(m_address);
                  msg->SetDestinationAddress(i.second);
                  msg->SetCreationTime(Simulator::Now());
                  msg->SetKind(CognitiveControlMessage::GH_ANM);
                  msgMap[pkt->GetUid()] = msg ;
                  m_controlDevice->Send(pkt,i.second,m_protocol);
            }
      }
}

void

CognitiveControlApplication::ReceiveMsg(Ptr<CognitiveControlMessage> msg)
{
      CognitiveControlMessage::Kind kind = msg->GetKind();
      
      switch(kind)
      {
      case(CognitiveControlMessage::NCCI):
      {
        //   std::cout << m_node->GetId() << " I have received NCCI "<<msg->GetSourceAddress() <<' ' << Simulator::Now() << '\n';
            Address src = msg->GetSourceAddress();
            m_msgs[src] = msg;
            m_ncci_Expiracy[src].Cancel();
            m_ncci_Expiracy[src] = Simulator::Schedule(m_ctrlMsgDuration,
                                                      &CognitiveControlApplication::DeleteCtrlMsg,this,src,kind);
            break;
      }
      case(CognitiveControlMessage::CH_REQ):
      {     
          //  std::cout << m_node->GetId() << " I have received CH_REQ " << Simulator::Now() << '\n'; 
            Address reqAddress = msg->GetSourceAddress();
            m_ch_req_Expiracy[reqAddress] = Simulator::Schedule(m_ctrlMsgDuration,
                                                               &CognitiveControlApplication::DeleteCtrlMsg,this,
                                                                reqAddress,CognitiveControlMessage::CH_REQ);
            break;
      }
      case(CognitiveControlMessage::CH_ANM):
      {
           // std::cout << m_node->GetId() << " I have received CH_ANM " << Simulator::Now() << '\n';
            
            Address CHaddress = msg->GetSourceAddress();
            uint16_t CADC = msg->GetCADC();
            uint16_t CBDC = msg->GetCBDC();
            (*m_neighborCluster)[CHaddress] = std::make_pair(CADC,CBDC);
            m_ch_anm_Expiracy[CHaddress].Cancel();
            m_ch_anm_Expiracy[CHaddress] = 
            Simulator::Schedule(m_ctrlMsgDuration,
                              &CognitiveControlApplication::DeleteCtrlMsg,this,CHaddress,kind);
       
            break;
      }
      case(CognitiveControlMessage::JOIN_REQ):
      {
          //  std::cout << m_node->GetId() << " I have received JOIN_REQ " << Simulator::Now() << '\n';
            m_members_Expiracy[msg->GetSourceAddress()].Cancel();
            m_members_Expiracy[msg->GetSourceAddress()]=
                  Simulator::Schedule(m_ctrlMsgDuration,&CognitiveControlApplication::DeleteCtrlMsg,this,
                                                        msg->GetSourceAddress(),kind);
                                                        
            break;
      }
      case(CognitiveControlMessage::GH_ANM):
      {     
            std::cout << m_node->GetId() << " I have received GH_ANM " << Simulator::Now() << '\n';
            
            m_ImGateway = true;
            m_gh_anm_Expiracy[msg->GetSourceAddress()].Cancel();
            m_gh_anm_Expiracy[msg->GetSourceAddress()]=
                  Simulator::Schedule(m_ctrlMsgDuration,&CognitiveControlApplication::DeleteCtrlMsg,this,
                                                            msg->GetSourceAddress(),kind);
            break;
      }
      default:
            NS_ASSERT_MSG(false,"An Unknown type control message");
      }

}

void

CognitiveControlApplication::ReceiveControlMsg(Ptr<Packet> pkt)
{
      if(msgMap.count(pkt->GetUid()))
      {
            Ptr<CognitiveControlMessage> msg = msgMap[pkt->GetUid()];
            ReceiveMsg(msg);  
            return;
      }
      else
      {
            return;
      }
}

void 

CognitiveControlApplication::DeleteCtrlMsg(Address address,
                                           CognitiveControlMessage::Kind kind)
{
      switch(kind)
      {
      case(CognitiveControlMessage::NCCI):
      {
            m_msgs.erase(address);
            (*m_neighborsInfo).erase(address);
            m_ncci_Expiracy.erase(address);
            break;
      }
      case(CognitiveControlMessage::CH_ANM):
      {
            (*m_neighborCluster).erase(address);
            m_ch_anm_Expiracy.erase(address);
            break;     
      }
      case(CognitiveControlMessage::CH_REQ):
      {
            m_ch_req_Expiracy.erase(address);
            break;
      }
      case(CognitiveControlMessage::JOIN_REQ):
      {
            m_members_Expiracy.erase(address);
            break;
      }
      case(CognitiveControlMessage::GH_ANM):
      {
            m_gh_anm_Expiracy.erase(address);
            if(!m_ImClusterHead)
            {
                  if(m_gh_anm_Expiracy.size()==0)
                  {
                        m_ImGateway = false;
                  }
            }
            break;
      }
      default:
            break;
      }

}

void 

CognitiveControlApplication::UpdateNeighborsInfoTables()
{
      (*m_neighborsInfo).clear();
      for(auto& i : (m_msgs))
      {
            Address tempAd = i.first;
            ChannelQuality temCQ = i.second->GetChannelQualityMap();
            std::set<uint16_t> avCH ;
            for(auto& j : *temCQ)
            {
                  avCH.insert(j.first);
            }
            (*m_neighborsInfo).insert({tempAd,avCH});      
      }
      m_updateneighbors = Simulator::Schedule(m_updateTables,
                                              &CognitiveControlApplication::UpdateNeighborsInfoTables,this);
}


void

CognitiveControlApplication::SetAddress(Address address)
{
      m_address = address;
}

void 

CognitiveControlApplication::CalculateVvalues()
{
      m_Vvalues.clear();
      for(auto& i : m_msgs)
      {
            m_Vvalues[i.first] = DoCaculateVvalue(i.second);
      }
      Ptr<CognitiveControlMessage> msg = CreateObject<CognitiveControlMessage>();
      msg->SetChannelQualityMap(m_availableChannelQvalues);
      msg->SetNeighborNodeConnectivityMap(m_neighborsInfo);
      msg->SetNeighborClusterReachabilityMap(m_neighborCluster);
      msg->SetEnergy(m_curEnergy);
      m_Vvalues[m_address] = DoCaculateVvalue(msg);
      m_updateVtable = Simulator::Schedule(m_updateTables,
                                           &CognitiveControlApplication::CalculateVvalues,this);
}

double

CognitiveControlApplication::DoCaculateVvalue(Ptr<CognitiveControlMessage> msg)
{
      std::set<uint16_t> EAC; //!< effective available channels
      for(auto& i : *(msg->GetChannelQualityMap()))
      {
            EAC.insert(i.first);
      }
      for(auto& i : *(msg->GetNeighborClusterReachabilityMap()))
      {
            EAC.erase(i.second.first);
      }
      double CF = 0.0 ;
      double NN = (double)(*msg->GetNeighborNodeConnectivityMap()).size();
      double NRC = (double)(*msg->GetNeighborClusterReachabilityMap()).size();
      double E = msg->GetEnergy();
      for(auto& i : EAC)
      {
            uint16_t Nkj = 0 ;
            for(auto& j : *(msg->GetNeighborNodeConnectivityMap()))
            {
                  if(j.second.count(i))
                  {
                        Nkj++;
                  }
            }
            CF+= (*msg->GetChannelQualityMap())[i]*Nkj;
      }
      double V = m_beta1*(E/m_Emax) + m_beta2*(CF/m_CFmax) + m_beta3*(NRC/m_NRCmax) + m_beta4*(NN/m_NNmax);
      return V; 
}

void 

CognitiveControlApplication::ChooseMyCluseterCandidate()
{
      double mx = 0.0 ;
      Address address;
      for(auto& i : m_Vvalues)
      {
            if(mx < i.second)
            {
                  mx = i.second;
                  address = i.first;
            }    
      }
      m_CHCaddress = address;
      if(m_CHCaddress!=m_address)
      {
            Simulator::ScheduleNow(&CognitiveControlApplication::SendMsg,this,
                                   CognitiveControlMessage::CH_REQ);
      }
      Simulator::Schedule(m_clusterAge,&CognitiveControlApplication::ChooseMyCluseterCandidate,this);
}

void

CognitiveControlApplication::EvaluateCH_REQs()
{
      uint16_t m_nCH_REQ = (uint16_t)(m_ch_req_Expiracy.size());
      uint16_t m_nNN = (uint16_t)((*m_neighborsInfo).size());
      std::cout << m_nNN << ' ' << m_nCH_REQ << ' ' << Simulator::Now() <<'\n';
      if((double)(m_nCH_REQ)>=m_Etta*((double)m_nNN))
      {
            std::set<uint16_t> EAC;
            for(auto& i : (*m_availableChannelQvalues))
            {
                  EAC.insert(i.first);
            }
            for(auto& i : (*m_neighborCluster))
            {
                  EAC.insert(i.second.first);
            }
            std::set<std::pair<double,uint16_t>> channels;
            for(auto& i : EAC)
            {
                  uint16_t Njk = 0 ;
                  for(auto& j : (*m_neighborsInfo))
                  {
                        if(j.second.count(i))
                        {
                              Njk++;
                        }
                  }
                  channels.insert(std::make_pair(Njk*(*m_availableChannelQvalues)[i],i));
            }
            if(channels.empty())
            {
                  m_ImClusterHead = false;
                  m_CADC = -1;
                  m_CBDC = -1;      
                  return;
            }
            if(channels.size()>=1){m_CADC = (*channels.rbegin()).second; channels.erase(*channels.rbegin());}
            if(channels.size()>=1){m_CBDC = (*channels.rbegin()).second; channels.erase(*channels.rbegin());}
            m_CHaddress = m_address;
            m_ImClusterHead = true;
            if(!m_setCommonDataChannelsCallback.IsNull())
            {
                  m_routingUnite->SetCluster(m_CHaddress);
                  m_setCommonDataChannelsCallback(m_CADC,m_CBDC,m_CHaddress);
            }
            std::cout << m_node->GetId() << ' ' << m_CADC << ' ' << m_CBDC << '\n';
            Simulator::ScheduleNow(&CognitiveControlApplication::SendMsg,this,
                                    CognitiveControlMessage::CH_ANM);
            Simulator::Schedule(Seconds(1),&CognitiveControlApplication::ChooseGateways,this);
      }
      else
      {
            if(m_ImClusterHead)
            {
                  m_CADC = -1;
                  m_CBDC = -1;
            }
            m_ImClusterHead = false;
      }
      Simulator::Schedule(m_clusterAge,&CognitiveControlApplication::EvaluateCH_REQs,this);
      
}

void

CognitiveControlApplication::EvaluateCH_ANMs()
{
      if(m_ImClusterHead)
      {
            return ;
      } 
      double mx = 0.0;
      for(auto& i : (*m_neighborCluster))
      {
            if(m_Vvalues[i.first] > mx)
            {
                  mx = m_Vvalues[i.first];
                  m_CADC = i.second.first;
                  m_CBDC = i.second.second;
                  m_CHaddress = i.first;
            }
      }
      if(mx > 0.0)
      {
            if(!m_setCommonDataChannelsCallback.IsNull())
            {
                  m_routingUnite->SetCluster(m_CHaddress);
                  m_setCommonDataChannelsCallback(m_CADC,m_CBDC,m_CHaddress);
            }
            Simulator::ScheduleNow(&CognitiveControlApplication::SendMsg,this,
                                   CognitiveControlMessage::JOIN_REQ);
      }
      Simulator::Schedule(m_clusterAge,&CognitiveControlApplication::EvaluateCH_ANMs,this);
}

void

CognitiveControlApplication::ChooseGateways()
{
      if(!m_ImClusterHead)
      {
            m_gateways.clear();
            return ;
      }
      for(auto& i : *m_neighborCluster)
      {
            double mx = 0.0 ;
            Address temAddress;
            for(auto& j : m_msgs)
            {
                  if((*j.second->GetNeighborClusterReachabilityMap()).count(i.first))
                  {
                        double k = (*j.second->GetChannelQualityMap())[m_CADC] +
                                   (*j.second->GetChannelQualityMap())[i.second.first];
                        k/=2;
                        if(k > mx)
                        {
                              mx = k ;
                              temAddress = j.first;
                        } 
                  }
            }
            if(mx > 0.0)
            {
                  m_gateways[i.first] = temAddress;
            }
      }
      Simulator::ScheduleNow(&CognitiveControlApplication::SendMsg,this,
                              CognitiveControlMessage::GH_ANM);
}

void 

CognitiveControlApplication::SetVparameters(double beta1, double beta2,
                                            double beta3, double beta4)
{
      m_beta1 = beta1;
      m_beta2 = beta2;
      m_beta3 = beta3;
      m_beta4 = beta4;
}

void 

CognitiveControlApplication::SetMaximumEnergy(double energy)
{
      m_Emax = energy;
}

void 

CognitiveControlApplication::SetMaximumNeighboringClusters(uint16_t number)
{
      m_NRCmax = number;
}

void

CognitiveControlApplication::SetMaximumNeighboringNodes(uint16_t number)
{
      m_NNmax = number;
}

void 

CognitiveControlApplication::SetCognitiveRoutingUnite(Ptr<CognitiveRoutingUnite> routingUnite)
{
      m_routingUnite = routingUnite;
}

Ptr<CognitiveRoutingUnite>

CognitiveControlApplication::GetCognitiveRoutingUnite()
{
      return m_routingUnite;
}

bool 

CognitiveControlApplication::IsClusterMember(Address address)
{
      if(address==m_address)
      {
            return true;
      }
      else
      {
            return m_members_Expiracy.count(address);
      }
}

}