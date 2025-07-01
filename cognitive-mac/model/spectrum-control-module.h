/*
 * Copyright (c) 2025 Telecommunications Lab, Higher Institiute for Applied Sciences and Technology , Damascus.
 *
 *
 * Author: Rida Takla <raidotakla@gmail.com>
 */
#ifndef SPECTRUM_CONTROL_MODULE
#define SPECTRUM_CONTROL_MODULE

#include <ns3/object.h>
#include <ns3/nstime.h>
#include <ns3/random-variable-stream.h>
#include <ns3/node.h>
#include <iostream>
#include <vector>

namespace ns3
{

  
typedef Callback<double,uint16_t> SenseResultCallback;
typedef Callback<void,std::map<uint16_t,double>> QtableResultCallback;

    class SpectrumControlModule : public Object
    {
        public:
        static TypeId GetTypeId();

        SpectrumControlModule();
        ~SpectrumControlModule() override;

        /**
         * @brief Setting the callback used
         * by the specturm control module
         * to get the sensing result from 
         * the phy
         * @param c the callback
         */
        void SetSenseResultCallback(SenseResultCallback c);

        /**
         * @brief Setting the callback 
         * for giving the result to the control
         * application
         * @param c the callback
         */
        void SetQtableResultCallback(QtableResultCallback c);

        /**
         * @brief Setting the channel settings
         * @param bgSize number of channels inside a large group
         * @param bgCount number of large groups
         */
        void SetChannels(uint16_t bgSize , uint16_t bgCount);

        /**
         * @brief Setting the time for single
         * channel sensing period
         * @param Ts the sensing time
         */
        void SetSingleChannelSensingPeriod(Time Ts);

        /**
         * @brief Setting the time for measuring 
         * a channel
         * @param Ts the sensing time
         */
        void SetChannelMeasuringTime(Time Ts);

        /**
         * @brief Set the learning rate
         * @param learningRate 
         */
        void SetLearningRate(double learningRate);

        /**
         * @brief starting the work of 
         * the module
         */
        void StartWorking();

        /**
         * @brief the main function 
         * of the module using the Q-learning
         * for updating each channel status 
         */
        
        void SenseSpectrum();

        /**
         * @brief setting the threshold for deciding
         * the channel status
         * @param threshold 
         */
        void SetThreshold(double threshold);

        /**
         * @brief updating the Q-table after 
         * taking the measurments
         * @param Index of the band group
         */
        void UpdateQtable(uint16_t Index);

        /**
         * @brief sending the result for 
         * the control application
         */
        void SendSensingResult(uint16_t bgIndex);

        /**
         * @return the bandgroup sensing time
         */
        Time GetBandGroupSensingTime();

        /**
         * @brief Set the node attached 
         * to the device
         */
        void SetNode(Ptr<Node> node);

        private :

        void DoSenseChannel(uint16_t bgIndex , uint16_t Index , uint16_t fois);

        SenseResultCallback m_senseResultCallback;
        QtableResultCallback m_QtableResultCallback;
       
        std::vector<double> *m_Qtable;   //!< the Qtable of the channels
        std::vector<double> *m_bgQtable; //!< the Qtable of the large groups

        std::vector<std::vector<bool>> Temp;    //!< vector used for measurments
 
        Time m_SingleChannelSensingPeriod;//!< the single channel sensing time 


        uint16_t m_bgSize ;             //!< number of channels inside the large group 
        uint16_t m_bgCount ;            //!< the number of large groups 
        uint16_t m_Nsensing;            //!< the number of sensing times for each channel within the Tbg

        double m_threshold ;            //!< the threshold for deciding the channel status
        double m_w1;                    //!< first constant for calculating the reward
        double m_w2;                    //!< second constant for calculating the reward
        double m_learningRate;          //!< the learning rate
        double m_discountFactor;        //!< the discount factor

        Ptr<Node> m_node ;              //!< the node the module attached to 



    };

}

#endif // SPECTRUM_CONTROL_MODULE