#ifndef ALICEO2_DEVICES_FLPSENDER_H_
#define ALICEO2_DEVICES_FLPSENDER_H_

#include <string>
#include <queue>
#include <unordered_map>
#include <chrono>

#include "FairMQDevice.h"

namespace AliceO2 {
namespace Devices {

/// Sends sub-timframes to epnReceivers
///
/// Sub-timeframes are received from the previous step (or generated in test-mode)
/// and are sent to epnReceivers. Target epnReceiver is determined from the timeframe ID:
/// targetEpnReceiver = timeframeId % numEPNs (numEPNs is same for every flpSender, although some may be inactive).

class FLPSenderDevice : public FairMQDevice
{
  public:
    /// Default constructor
    FLPSenderDevice() = default;

    /// Default destructor
    virtual ~FLPSenderDevice() final = default;

  protected:
    /// Overloads the InitTask() method of FairMQDevice
    virtual void InitTask() final;

    /// Overloads the Run() method of FairMQDevice
    virtual void Run() final;

  private:
    /// Sends the "oldest" element from the sub-timeframe container
    void sendFrontData();

    std::queue<FairMQParts> mSTFBuffer; ///< Buffer for sub-timeframes
    std::queue<std::chrono::steady_clock::time_point> mArrivalTime; ///< Stores arrival times of sub-timeframes

    int mNumEPNs = 0; ///< Number of epnReceivers
    unsigned int mIndex = 0; ///< Index of the flpSender among other flpSenders
    unsigned int mSendOffset = 0; ///< Offset for staggering output
    unsigned int mSendDelay = 8; ///< Delay for staggering output

    int mEventSize = 10000; ///< Size of the sub-timeframe body (only for test mode)
    int mTestMode = false; ///< Run the device in test mode (only syncSampler+flpSender+epnReceiver)
    uint16_t mTimeFrameId;

    std::string mInChannelName = "";
    std::string mOutChannelName = "";
    int mLastTimeframeId = -1;
};

} // namespace Devices
} // namespace AliceO2

#endif
