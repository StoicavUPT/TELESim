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

Radio::Radio(const DeviceIdPtr &deviceId) : Device(deviceId), logger_("Radio|" + std::to_string(deviceId->port_)) {
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

std::optional<int> Radio::isPhoneOwner(const std::string &phoneNr) { //WA 2 functions into one :)
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


int Radio::postPhoneConnectCall(const std::string& senderPhoneNr, const int phonePort) {
    return emitter_->postPhoneConnectCall(senderPhoneNr,phonePort);
}


int Radio::postRadioConnectCall(const std::string &senderPhoneNr, const DeviceIdPtr &senderOwnerId, const DeviceIdPtr &receiverOwnerId, const std::string &receiverPhoneNr) {
    return emitter_->postRadioConnectCall(senderPhoneNr,senderOwnerId,receiverOwnerId,receiverPhoneNr);
}


void Radio::registerCall(const CallIdPtr &callId) {
    logger_ << logs::info << "Registering call with id "<< callId->id;

    std::lock_guard lock(ongoingCallsMutex_);

    ongoingCalls_.push_back(callId);
}


void Radio::completePacket(PacketPtr& packet) {

    const auto callId = packet->callId_;

    std::vector<CallIdPtr> ongoingCalls;

    {
        std::lock_guard lock(ongoingCallsMutex_);
        ongoingCalls = ongoingCalls_;
    }

    for (const auto& call : ongoingCalls) {
        if (call->id == callId) {

            if (*call->senderRadioId == *deviceId_) {

                packet->receiverRadioPort_ =
                    call->receiverRadioId->port_;
            }
            else if (*call->receiverRadioId == *deviceId_) {
                packet->receiverRadioPort_ =
                    call->senderRadioId->port_;
            }
            return;
        }
    }

    logger_ << logs::error
            << "Could not complete packet. "
            << "Call not found: "
            << callId;
}


bool Radio::isReceiverRadio(const std::string &phoneNr) {
    std::vector<DeviceIdPtr> idMap;
    {
        std::lock_guard lock(idMapMutex_);
        idMap = ipAddressMaster_->idMap_;
    }
    for (const auto& id : idMap) {
        if (id->phoneNr_ == phoneNr) {
            return true;
        }
    }
    return false;

}

void Radio::handlePacket(const std::string& callId,
        const std::string& message,
        const std::string& receiverPhoneNr) {
    auto packet = std::make_shared<Packet>();
    packet->message_ = message;
    packet->callId_ = callId;
    packet->receiverPhoneNr_ = receiverPhoneNr;
    if (!isReceiverRadio(receiverPhoneNr)) {
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

        std::cout<<"Sending packet with callId: "<<packet->callId_<<std::endl;

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

        logger_ << logs::info << "Sending packet with callId " << packet->callId_;
        lock.unlock();

        handleReceivedPacket(packet);
    }
}

void Radio::handleReceivedPacket(const PacketPtr& packet) {

    const auto phoneNr = packet->receiverPhoneNr_;
    auto phone = getLocalPhone(phoneNr);
    if (!phone) {
        logger_ << logs::error << "No phones found in internal map for phoneNr "<<phoneNr;
        return;
    }
    auto phonePort = phone->get()->port_;

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
    emitter_->postRadioCallPacket(packet->callId_,
        packet->message_,
        packet->receiverRadioPort_,
        packet->receiverPhoneNr_);
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


void Radio::updateMaster() {
    logger_<< logs::info << "Updating list for Master";
    std::vector<DeviceIdPtr> phones;
    {
        std::lock_guard lock(idMapMutex_);
        phones = ipAddressMaster_->idMap_;
    }
    phones.erase(phones.begin());
    emitter_->postMasterPhoneVector(ipAddressMaster_->ownerId_,phones);
}

std::optional<DeviceIdPtr> Radio::getLocalPhone(const std::string &phoneNr) {
    std::vector<DeviceIdPtr> phones;
    {
        std::lock_guard lock(idMapMutex_);
        phones = ipAddressMaster_->idMap_;
    }
    const auto it = std::ranges::find_if(phones,
                                 [&phoneNr](const DeviceIdPtr& phone) {
                                     return phone->phoneNr_ == phoneNr;});
    if (it == phones.end()) return std::nullopt;
    return *it;
}


std::optional<DeviceIdPtr> Radio::getPhoneRadio(const std::string &phoneNr) {
    std::vector<DeviceIdPtr> idMap;
    if (const auto localPhone = getLocalPhone(phoneNr)) return deviceId_;

    logger_  << logs::info << "Phone not found in local map";

    std::map<DeviceIdPtr,std::vector<DeviceIdPtr>> radiosToPhonesMap;

    {
        std::lock_guard lock(externalRadioToPhoneMapMutex_);
        radiosToPhonesMap = ipAddressMaster_->externalRadioToPhonesMap_;
    }
    for (const auto& [radio,phones]: radiosToPhonesMap) {
        for (const auto& phone:phones) {
            if (phone->phoneNr_ == phoneNr) {
                logger_<< logs::info << "Phone found in external Radio to Phone map";
                return radio;
            }
        }
    }
    logger_  << logs::info << "Phone not found in external map";
    const auto phoneOwner = emitter_->getMasterPhoneOwner(phoneNr);
    if (phoneOwner) {
        const auto deviceId = std::make_shared<DeviceId>();
        deviceId->phoneNr_ = phoneNr;
        {
            std::lock_guard lock(externalRadioToPhoneMapMutex_);
            ipAddressMaster_->externalRadioToPhonesMap_[*phoneOwner].push_back(deviceId);
        }
        logger_  << logs::info << "Phone found in MASTER map";
    }
    return phoneOwner;
}

