
#ifndef COGNITIVE_SPECTRUM_INTERFERENCE_H
#define COGNITIVE_SPECTRUM_INTERFERENCE_H
#include "ns3/spectrum-value.h" 
#include <ns3/nstime.h>
#include <ns3/object.h>
#include <ns3/packet.h>

namespace ns3
{

class SpectrumErrorModel;

/**
 * \ingroup spectrum
 *
 * This class implements a gaussian interference model, i.e., all
 * incoming signals are added to the total interference.
 *
 */

class CognitiveSpectrumInterference : public Object
{
  public:
    CognitiveSpectrumInterference();
    ~CognitiveSpectrumInterference() override;

    /**
     * Register this type.
     * \return The TypeId.
     */
    static TypeId GetTypeId();

    /**
     * Set the SpectrumErrorModel to be used.
     *
     * @param e the error model
     */
    void SetErrorModel(Ptr<SpectrumErrorModel> e);

    /**
     * Notify that the PHY is starting a RX attempt
     *
     * @param p the packet corresponding to the signal being RX
     * @param rxPsd the power spectral density of the signal being RX
     */
    void StartRx(Ptr<const Packet> p, Ptr<const SpectrumValue> rxPsd);

    /**
     * Notify that the PHY has aborted RX
     */
    void AbortRx();

    /**
     * Notify that the RX attempt has ended. The receiving PHY must call
     * this method upon RX end in order to:
     * 1) know if RX was successful or not
     * 2) free up resources that might eventually be used for the
     * calculation of interference. Note that for this reason this
     * method must also be called when RX is aborted.
     *
     *
     * @return true if RX was successful, false otherwise
     */
    bool EndRx();

    /**
     * Notify that a new signal is being perceived in the medium. This
     * method is to be called for all incoming signal, regardless of
     * whether they're useful signals or interferers.
     *
     * @param spd the power spectral density of the new signal
     * @param duration the duration of the new signal
     */
    void AddSignal(Ptr<const SpectrumValue> spd, const Time duration);

    /**
     * Set the Noise Power Spectral Density
     *
     * @param noisePsd the Noise Power Spectral Density in power units
     * (Watt, Pascal...) per Hz.
     */
    void SetNoisePowerSpectralDensity(Ptr<const SpectrumValue> noisePsd);

    /**
     * @brief Getting the whole spectrum valuse
     * @return spectrum value of the current time
     */
    Ptr<const SpectrumValue> GetSpectrum();

    /**
     * @brief Set the channel info 
     * @param chInd the channel index 
     * @param numBn the number of bins
     * @param numCh the total numb of channels
     */
    void SetChannelInfo(uint16_t chInd,uint16_t numBn,uint16_t numCh);

  protected:
    void DoDispose() override;

  private:
    /**
     * Evaluate a Chunk, depending on the Rx status and the last update time
     */
    void ConditionallyEvaluateChunk();
    /**
     * Adds a signal perceived in the medium.
     * @param spd the power spectral density of the new signal
     */
    void DoAddSignal(Ptr<const SpectrumValue> spd);
    /**
     * Removes a signal perceived in the medium.
     * @param spd the power spectral density of the new signal
     */
    void DoSubtractSignal(Ptr<const SpectrumValue> spd);

    bool m_receiving; //!< True if in Rx status

    /**
     * Stores the power spectral density of the signal whose RX is being attempted
     */
    Ptr<const SpectrumValue> m_rxSignal;

    /**
     * Stores the spectral power density of the sum of incoming signals;
     * does not include noise, includes the SPD of the signal being RX
     */
    Ptr<SpectrumValue> m_allSignals;

    Ptr<const SpectrumValue> m_noise; //!< Noise spectral power density

    Time m_lastChangeTime; //!< the time of the last change in m_TotalPower

    Ptr<SpectrumErrorModel> m_errorModel; //!< Error model
};

} // namespace ns3
#endif /* SPECTRUM_INTERFERENCE_H */