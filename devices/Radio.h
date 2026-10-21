//
// Created by vlads on 8/23/2026.
//

#ifndef TELESIM_RADIO_H
#define TELESIM_RADIO_H
#include "Device.h"
#include <memory>
#include <queue>
#include <vector>

#include "Packet.h"
#include "Phone.h"
#include "../fwk/IpAddressMaster.h"
#include "../utils/Logger.h"
#include "info/IpAddress.h"
#include "../fwk/DeviceFactory.h"
#include "info/CallId.h"

class RadioListener;
using RadioListenerPtr = std::shared_ptr<RadioListener>;
class RadioEmitter;
using RadioEmitterPtr = std::shared_ptr<RadioEmitter>;

class Radio: public Device {
public:
    Radio(const DeviceIdPtr& deviceId);
    ~Radio();

    void createPhone(const DeviceFactoryPtr&);
    void completeAdditionalInfo(const DeviceIdPtr& , int , const std::string&);
    std::optional<DeviceIdPtr> getPhoneRadio(const std::string&);
    std::optional<int> isPhoneOwner(const std::string&);
    int postRadioConnectCall(const std::string& senderPhoneNr,
        const DeviceIdPtr& senderOwnerId,
        const DeviceIdPtr& receiverOwnerId,
        const std::string& receiverPhoneNr);
    int postPhoneConnectCall(const std::string& senderPhoneNr, int phonePort);
    void registerCall(const CallIdPtr& callId);
    void handlePacket(const std::string& callId,
        const std::string& message,
        const std::string& receiverPhoneNr);
    void sendPacket(const PacketPtr&);
    void handleReceivedPacket(const PacketPtr& packet);
private:
    std::optional<DeviceIdPtr> getLocalPhone(const std::string& phoneNr);
    bool isReceiverRadio(const std::string&);
    void completePacket(PacketPtr& packet);
    void sendPackets();
    void handleReceivedPackets();
    void updateMaster();
    void endCall(const std::string&);
    RadioListenerPtr listener_;
    RadioEmitterPtr emitter_;

    utils::Logger logger_;
    IpAddressMasterPtr ipAddressMaster_;

    std::thread listenerThread_;
    std::thread recursiveUpdateThread_;
    std::thread sendPacketsThread_;
    std::thread handleReceivedPacketsThread_;

    std::mutex idMapMutex_;
    std::mutex externalRadioToPhoneMapMutex_;
    std::mutex ongoingCallsMutex_;

    std::queue<PacketPtr> toSendPackets_;
    std::mutex toSendPacketsMutex_;
    std::condition_variable toSendPacketsCV_;

    std::queue<PacketPtr> receivedPackets_;
    std::mutex receivedPacketsMutex_;
    std::condition_variable receivedPacketsCV_;

    std::vector<CallIdPtr> ongoingCalls_;

    bool running_ = true;
};



using RadioPtr = std::shared_ptr<Radio>;


#endif //TELESIM_RADIO_H
