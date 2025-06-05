/*
 * Copyright (c) 2025 Telecommunications Lab, Higher Institiute for Applied Sciences and Technology , Damascus.
 *
 *
 * Author: Rida Takla <raidotakla@gmail.com>
 */

#ifndef COGNITIVE_NET_DEVICE_HELPER_H
#define COGNITIVE_NET_DEVICE_HELPER_H

#include <ns3/attribute.h>
#include <ns3/net-device-container.h>
#include <ns3/node-container.h>
#include <ns3/object-factory.h>
#include <ns3/spectrum-model.h>
#include <ns3/queue.h>
#include <string>

namespace ns3
{

class SpectrumValue;
class SpectrumChannel;

/**
 * \ingroup spectrum
 * \brief create the AlohaNoackNetDevice
 */

class CognitiveNetDeviceHelper
{
  public:
    CognitiveNetDeviceHelper();
    ~CognitiveNetDeviceHelper();

    /**
     * set the SpectrumChannel that will be used by SpectrumPhy instances created by this helper
     * 
     * @param channel
     */
    void SetChannel(Ptr<SpectrumChannel> channel);

    /**
     * set the SpectrumChannel that will be used by SpectrumPhy instances created by this helper
     * 
     * @param channelName
     */
    void SetChannel(std::string channelName);

    /**
     *
     * @param dataTxPsd the Power Spectral Density to be used for transmission by all created PHY
     * instances
     */
    void SetDataTxPowerSpectralDensity(Ptr<SpectrumValue> dataTxPsd);

    /**
     *
     * @param ctrlTxPsd the Power Spectral Density to be used for transmission by all created PHY
     * instances
     */
    void SetCtrlTxPowerSpectralDensity(Ptr<SpectrumValue> ctrlTxPsd);

    /**
     * 
     * @param localmodel the Spectrum Model for a single channel
     */
    void SetLocalSpectrumModel(Ptr<SpectrumModel> localModel);

    /**
     *
     * @param noisePsd the Power Spectral Density to be used for transmission by all created PHY
     * instances
     */
    void SetNoisePowerSpectralDensity(Ptr<SpectrumValue> noisePsd);

    /**
     * @param name the name of the attribute to set
     * @param v the value of the attribute
     *
     * Set these attributes on each HdOfdmSpectrumPhy instance to be created
     */
    void SetPhyAttribute(std::string name, const AttributeValue& v);

    /**
     * @param n1 the name of the attribute to set
     * @param v1 the value of the attribute to set
     *
     * Set these attributes on each AlohaNoackNetDevice created
     */
    void SetDeviceAttribute(std::string n1, const AttributeValue& v1);

    
    /**
     * @param time the stopping time 
     */
    void SetStopTime(Time time);

    /**
     * @param time the starting time
     */
    void SetStartTime(Time time);

    /**
     * @param bgCount the number of band groups
     * @param bgSize the number of channels in each group
     * @param numOfChannels the number of channels
     * @param numBins the number of bins in each channel 
     */
    void SetChannelsInfo(uint16_t bgCount , uint16_t bgSize , uint16_t numOfChannels,uint16_t numBins);

    /**
     * @param threshold the comparison threshold
     */
    void SetThreshold(double threshold);

    
    /**
     * \tparam Ts \deduced Argument types
     * \param type the type of the model to set
     * \param [in] args Name and AttributeValue pairs to set.
     *
     * Configure the AntennaModel instance for each new device to be created
     */
    template <typename... Ts>
    void SetAntenna(std::string type, Ts&&... args);

    /**
     * @param c the set of nodes on which a device must be created
     * @return a device container which contains all the devices created by this method.
     */
    NetDeviceContainer Install(NodeContainer c) const;
    /**
     * @param node the node on which a device must be created
     * \returns a device container which contains all the devices created by this method.
     */
    NetDeviceContainer Install(Ptr<Node> node) const;
    /**
     * @param nodeName the name of node on which a device must be created
     * @return a device container which contains all the devices created by this method.
     */
    NetDeviceContainer Install(std::string nodeName) const;


  protected:
    ObjectFactory m_phy;                //!< Object factory for the phy objects
    ObjectFactory m_device;             //!< Object factory for the NetDevice objects
    ObjectFactory m_antenna;            //!< Object factory for the Antenna objects
    ObjectFactory m_controlApp;         //!< Object factory for the control net device
    ObjectFactory m_spectrumCtrl;       //!< Object factory for the spectrum control module 
    ObjectFactory m_routingUnite;       //!< Object factory for the routing uniter 
    Ptr<SpectrumChannel> m_channel;     //!< data Channel
    Ptr<SpectrumValue> m_dataTxPsd;     //!< Tx power spectral density for data device
    Ptr<SpectrumValue> m_ctrlTxPsd;     //!< Tx power spectral density for control device
    Ptr<SpectrumValue> m_noisePsd;      //!< Noise power spectral density for the data device
    Ptr<SpectrumModel> m_localModel;    //!< the local spectrum model for a single channel 
    Time m_stopTime;                    //!< the stopping time of the control application
    Time m_startTime;                   //!< the starting time of the control application
    uint16_t m_bgCount;                 //!< the number of large bandgroups 
    uint16_t m_bgSize;                  //!< the size of each large band group
    uint16_t m_numOfChannels;           //!< the total number of channels
    uint16_t m_numBins;                 //!< the number of bins in each channel
    double m_threshold;                 //!< the threshold used for deciding the channel status
};

/***************************************************************
 *  Implementation of the templates declared above.
 ***************************************************************/

template <typename... Ts>
void

CognitiveNetDeviceHelper::SetAntenna(std::string type, Ts&&... args)
{
    m_antenna = ObjectFactory(std::forward<Ts>(args)...);
}

} // namespace ns3

#endif /* COGNITIVE_NET_DEVICE_HELPER_H */