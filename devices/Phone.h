//
// Created by vlads on 8/23/2026.
//

#ifndef TELESIM_PHONE_H
#define TELESIM_PHONE_H
#include "Device.h"
#include "../utils/Logger.h"
#include "../HTTP_view/emit/PhoneEmitter.h"
#include "../HTTP_view/listen/PhoneListener.h"

class Phone: public Device {
public:
    Phone(const DeviceIdPtr& deviceId, const int& ownerPort);
    ~Phone();

    void sendAdditionalInfo();
    void initCall(const std::string&);
    bool isInCall();
    bool incomingCall(const std::string&);
    void acceptCall(const std::string&);
    void endCall();
    void startPhone();
    bool waitingForCallResponse();
    void sendCallMessage(const std::string&);
    void acceptIncomingCall();
    void declineIncomingCall();
    void handleCallPacket(const std::string& message);
private:
    utils::Logger logger_;
    int ownerPort_;
    PhoneListenerPtr listener_;
    PhoneEmitterPtr emitter_;

    std::thread listenerThread_;
    std::thread ongoingCallThread_;

    std::string ongoingCallPhoneNumber_;

    bool running_ = true;

    std::mutex cinMutex_;

    std::mutex inCallMutex;
    std::atomic_bool inCall_{false};

    std::string ongoingCallId_;

    std::mutex incomingCallMutex_;
    std::condition_variable incomingCallCv_;

    bool waitingForCallResponse_ = false;
    bool incomingCallAccepted_ = false;

    std::string incomingCallPhoneNumber_;
};

using PhonePtr = std::shared_ptr<Phone>;


#endif //TELESIM_PHONE_H
