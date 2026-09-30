//
// Created by vlads on 8/23/2026.
//

#include "Radio.h"

#include "Packet.h"
#include "Phone.h"
#include "../fwk/DeviceFactory.h"
#include "../utils/Logger.h"
#include "../HTTP_view/listen/RadioListener.h"
#include "../HTTP_view/emit/RadioEmitter.h"

Radio::Radio(const DeviceIdPtr &deviceId) : Device(deviceId), logger_("Radio|" + deviceId->mac_) {
    ipAddressMaster_ = std::make_shared<IpAddressMaster>(Role::ERole::RADIO, deviceId, deviceId_->port_);
    emitter_ = std::make_shared<RadioEmitter>();
    listener_ = std::make_shared<RadioListener>(deviceId->port_, deviceId, this);
    listener_->prepareServer();
    listenerThread_ = std::thread([this]() {
        listener_->listen(logger_);
    });
    listener_->waitUntilStarted();
    handleReceivedPacketsThread_ = std::thread([this]() {
        handleReceivedPackets();
    });
    sendPacketsThread_ = std::thread([this]() {
        sendPackets();
    });
}


Radio::~Radio() {

    running_ = false;

    if (recursiveUpdateThread_.joinable()) recursiveUpdateThread_.join();

    listener_->stop();

    if (listenerThread_.joinable()) listenerThread_.join();
}


void Radio::completeAdditionalInfo(const DeviceIdPtr &deviceId, int port, const std::string &phoneNr) {


    {
        std::lock_guard<std::mutex> lock(idMapMutex_);
        logger_ << logs::info << "Completing additional info for ip " << deviceId->ipAddress_->getIpAddress()<< " and mac "<< deviceId->mac_;

        for (auto& device : ipAddressMaster_->idMap_) {
            if (*device==*deviceId) {
                device->port_ = port;
                device->phoneNr_ = phoneNr;
                logger_<< logs::info << "Got port " << port << " and phoneNr "<< phoneNr<<" for ip "<<deviceId->ipAddress_->getIpAddress();
                break;
            }
        }
    }
    updateMaster();
}

void Radio::createPhone(const DeviceFactoryPtr& deviceFactory) {
    const auto ip = ipAddressMaster_->getNextIpAddress();
    logger_ << logs::info << "ip address aquired for PHONE: "<< ip->getIpAddress();
    const auto deviceId = deviceFactory->createDeviceId(ip);
    {
        std::lock_guard lock(idMapMutex_);
        ipAddressMaster_->addToIdMap(deviceId);
    }
    DeviceWindowFactory::createDeviceWindow(Role::PHONE, deviceId , ipAddressMaster_->port_);
}

std::optional<int> Radio::isPhoneOwner(const std::string &phoneNr) {
    std::vector<DeviceIdPtr> phones;
    {
        std::lock_guard lock(idMapMutex_);
        phones = ipAddressMaster_->idMap_;
    }
    for (const auto& phone: phones) {
        if (phone->phoneNr_ == phoneNr) return phone->port_;
    }

    logger_ << "Phone Nr " << phoneNr << " not found for RADIO with MAC " << deviceId_->mac_;

    return std::nullopt;
}


int Radio::postPhoneConnectCall(const std::string& senderPhoneNr, int phonePort) {
    return emitter_->postPhoneConnectCall(senderPhoneNr,phonePort);
}


int Radio::postRadioConnectCall(const std::string &senderPhoneNr, const DeviceIdPtr &senderOwnerId, const DeviceIdPtr &receiverOwnerId, const std::string &receiverPhoneNr) {
    return emitter_->postRadioConnectCall(senderPhoneNr,senderOwnerId,receiverOwnerId,receiverPhoneNr);
}


void Radio::registerCall(const CallIdPtr &callId) {

    logger_ << logs::debug << "REGISTER CALL - ENTERED";

    logger_ << logs::debug << "REGISTER CALL - BEFORE LOCK";

    std::lock_guard lock(ongoingCallsMutex_);

    logger_ << logs::debug << "REGISTER CALL - LOCK ACQUIRED";

    logger_ << logs::info
            << "Registering call with id "
            << callId->id;

    ongoingCalls_.push_back(callId);

    logger_ << logs::debug << "REGISTER CALL - PUSH FINISHED";
}


void Radio::completePacket(PacketPtr& packet) {

    const auto callId = packet->callId_;

    std::vector<CallIdPtr> ongoingCalls;

    {
        std::lock_guard lock(ongoingCallsMutex_);
        ongoingCalls = ongoingCalls_;
    }

    logger_ << logs::debug
            << __func__
            << " Searching for callId "
            << callId;

    for (const auto& call : ongoingCalls) {

        logger_ << logs::debug
                << __func__
                << " Comparing with "
                << call->id;

        if (call->id == callId) {

            if (*call->senderRadioId == *deviceId_) {

                packet->receiverRadioPort_ =
                    call->receiverRadioId->port_;

                logger_ << logs::debug
                        << "Current radio is SENDER. "
                        << "Sending to receiver radio port "
                        << packet->receiverRadioPort_;
            }
            else if (*call->receiverRadioId == *deviceId_) {

                packet->receiverRadioPort_ =
                    call->senderRadioId->port_;

                logger_ << logs::debug
                        << "Current radio is RECEIVER. "
                        << "Sending to sender radio port "
                        << packet->receiverRadioPort_;
            }

            return;
        }
    }

    logger_ << logs::error
            << "Could not complete packet. "
            << "Call not found: "
            << callId;
}


bool Radio::isLocalCall(const std::string &callId) {
    std::vector<CallIdPtr> ongoingCalls;
    {
        std::lock_guard lock(ongoingCallsMutex_);
        ongoingCalls = ongoingCalls_;
    }
    CallIdPtr foundCall;
    for (const auto& call : ongoingCalls) {
        if (call->id == callId) {
            foundCall = call;
        }
    }

    return *deviceId_ == *getPhoneRadio(foundCall->receiverPhoneNr).value_or(std::make_shared<DeviceId>());
}

void Radio::handlePacket(const std::string &callId, const std::string &message,bool isSender) {
    auto packet = std::make_shared<Packet>();
    packet->message_ = message;
    packet->callId_ = callId;
    if (isSender && !isLocalCall(callId)) {
        completePacket(packet);
        {
            std::lock_guard lock(toSendPacketsMutex_);
            toSendPackets_.push(packet);
        }
        toSendPacketsCV_.notify_one();
    }
    else {
        {
            std::lock_guard lock(receivedPacketsMutex_);
            receivedPackets_.push(packet);
        }

        receivedPacketsCV_.notify_one();
    }

}



void Radio::sendPackets()
{
    while (running_) {

        std::unique_lock lock(toSendPacketsMutex_);

        toSendPacketsCV_.wait(lock, [this] {
            return !toSendPackets_.empty() || !running_;
        });

        if (!running_ && toSendPackets_.empty())
            return;

        auto packet = std::move(toSendPackets_.front());
        toSendPackets_.pop();

        std::cout<<"Sent packet with callId: "<<packet->callId_<<std::endl;

        lock.unlock();

        sendPacket(packet);
    }
}
void Radio::handleReceivedPackets()
{
    while (running_) {

        std::unique_lock lock(receivedPacketsMutex_);

        receivedPacketsCV_.wait(lock, [this] {
            return !receivedPackets_.empty() || !running_;
        });

        if (!running_ && receivedPackets_.empty()) {
            return;
        }

        auto packet = std::move(receivedPackets_.front());
        receivedPackets_.pop();

        lock.unlock();

        handleReceivedPacket(packet);
    }
}

void Radio::handleReceivedPacket(const PacketPtr& packet) {

    std::vector<CallIdPtr> ongoingCalls;

    {
        std::lock_guard lock(ongoingCallsMutex_);
        ongoingCalls = ongoingCalls_;
    }

    std::string phoneNr;

    logger_ << logs::debug
            << __func__
            << " Searching for callId "
            << packet->callId_;

    for (const auto& call : ongoingCalls) {

        logger_ << logs::debug
                << "Comparing with "
                << call->id;

        if (call->id == packet->callId_) {


            if (*call->receiverRadioId == *deviceId_) {

                phoneNr = call->receiverPhoneNr;

                logger_ << logs::info
                        << "Current Radio is RECEIVER. "
                        << "Packet goes to phone "
                        << phoneNr;
            }

            /*
             * If this Radio is the sender Radio,
             * the packet must go to the sender phone.
             */
            else if (*call->senderRadioId == *deviceId_) {

                phoneNr = call->senderPhoneNr;

                logger_ << logs::info
                        << "Current Radio is SENDER. "
                        << "Packet goes to phone "
                        << phoneNr;
            }

            break;
        }
    }

    if (phoneNr.empty()) {

        logger_ << logs::error
                << "No call found for callId "
                << packet->callId_;

        return;
    }

    std::vector<DeviceIdPtr> phones;

    {
        std::lock_guard lock(externalRadioToPhoneMapMutex_);
        phones = ipAddressMaster_->idMap_;
    }

    int phonePort = 0;

    for (const auto& phone : phones) {

        if (phone->phoneNr_ == phoneNr) {

            phonePort = phone->port_;

            logger_ << logs::info
                    << "Found phone "
                    << phoneNr
                    << " on port "
                    << phonePort;

            break;
        }
    }

    if (phonePort == 0) {

        logger_ << logs::error
                << "Could not find port for phone "
                << phoneNr;

        return;
    }

    if (packet->message_ == "0") {
        endCall(packet->callId_);
    }

    emitter_->postPhoneCallPacket(
        packet->message_,
        phonePort
    );
}

void Radio::sendPacket(const PacketPtr &packet) {
    if (packet->message_ == "0") {
        endCall(packet->callId_);
    }
    emitter_->postRadioCallPacket(packet->callId_,packet->message_,packet->receiverRadioPort_);
}

void Radio::endCall(const std::string& callId) {

    std::cout<<"Ending call with callId: "<<callId<<std::endl;
    {
        std::lock_guard lock(ongoingCallsMutex_);

        std::erase_if(ongoingCalls_,[callId](const CallIdPtr& callIdObject) {
            return callId == callIdObject->id;
        });
    }
}

bool Radio::isSenderPacket(const PacketPtr& packet) {
    const auto callId = packet->callId_;
    std::vector<CallIdPtr> ongoingCalls;
    {
        std::lock_guard lock(ongoingCallsMutex_);
        ongoingCalls = ongoingCalls_;
    }
    for (const auto& call: ongoingCalls) {
        if (*call->senderRadioId==*deviceId_ && callId == call->id) {
            logger_ << logs::info << "packet wiht id " << callId << " found. Radio is Sender";
            return true;
        }
    }
    logger_ << logs::info << "Radio is not sender";
    return false;
}

void Radio::updateMaster() {
    logger_<< logs::info << "Updating list for Master";
    std::vector<DeviceIdPtr> phones;
    {
        std::lock_guard lock(idMapMutex_);
        phones = ipAddressMaster_->idMap_;
        phones.erase(phones.begin());
    }
    emitter_->postMasterPhoneVector(ipAddressMaster_->ownerId_,phones);
}

std::optional<DeviceIdPtr> Radio::getPhoneRadio(const std::string &phoneNr) {
    {
        std::lock_guard lock(idMapMutex_);

        for (const auto& id : ipAddressMaster_->idMap_) {
            if (phoneNr == id->phoneNr_) {
                logger_<< logs::info << "Phone found in local idMap";
                return deviceId_;
            }
        }
    }
    {
        std::lock_guard lock(externalRadioToPhoneMapMutex_);
        for (const auto& [radio,phones]: ipAddressMaster_->externalRadioToPhonesMap_) {
            for (const auto& phone:phones) {
                if (phone->phoneNr_ == phoneNr) {
                    logger_<< logs::info << "Phone found in external Radio to Phone map";
                    return radio;
                }
            }
        }
    }
    const auto phoneOwner = emitter_->getMasterPhoneOwner(phoneNr);
    if (phoneOwner) {
        {
            std::lock_guard lock(externalRadioToPhoneMapMutex_);
            const auto deviceId = std::make_shared<DeviceId>();
            deviceId->phoneNr_ = phoneNr;
            ipAddressMaster_->externalRadioToPhonesMap_[*phoneOwner].push_back(deviceId);
        }
    }
    return phoneOwner;
}

