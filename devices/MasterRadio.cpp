//
// Created by vlads on 8/31/2026.
//

#include "MasterRadio.h"

#include "Radio.h"


MasterRadio::MasterRadio(const DeviceIdPtr &deviceId) :
    Device(deviceId),
    logger_("MasterRadio|"+deviceId->mac_) {
    ipAddressMaster_ = std::make_shared<IpAddressMaster>(Role::MASTER,deviceId,deviceId_->port_);
    listener_ = std::make_shared<MasterListener>(deviceId->port_, deviceId, this);
    listener_->prepareServer();
    listenerThread_ = std::thread([this]() {
        listener_->listen(logger_);
    });
    listener_->waitUntilStarted();
}


MasterRadio::~MasterRadio() {
    listener_->stop();
    if (listenerThread_.joinable())
    {
        listenerThread_.join();
    }
}


void MasterRadio::createRadio(const DeviceFactoryPtr &deviceFactory) {
    const auto ip = ipAddressMaster_->getNextIpAddress();
    logger_ << logs::debug << "address aquired for RADIO: "<< ip->getIpAddress();
    auto deviceId= deviceFactory->createDeviceId(ip);
    {
        std::lock_guard lock(idMapMutex_);
        ipAddressMaster_->addToIdMap(deviceId);
    }
    {
        std::lock_guard lock(externalRadioToPhoneMapMutex_);
        ipAddressMaster_->externalRadioToPhonesMap_[deviceId];
    }

    DeviceWindowFactory::createDeviceWindow(Role::RADIO,deviceId,ipAddressMaster_->port_);
}

void MasterRadio::addtoExternalRadioToPhonesMap(const DeviceIdPtr& deviceId, const std::vector<DeviceIdPtr>& deviceIdVec) {
    {
        std::lock_guard lock(externalRadioToPhoneMapMutex_);
        ipAddressMaster_->addtoExternalRadioToPhonesMap(deviceId,deviceIdVec);
    }
}

std::optional<DeviceIdPtr> MasterRadio::getRadioForPhoneNr(const std::string &phoneNr) {

    std::map<DeviceIdPtr,std::vector<DeviceIdPtr>> map;
    {
        std::lock_guard lock(externalRadioToPhoneMapMutex_);
        map = ipAddressMaster_->externalRadioToPhonesMap_;
    }

    logger_<< logs::info << __func__ << "Searching for radio owner of "<<phoneNr;
    for (const auto& [radio,phones]: map) {
        for (const auto& phone:phones) {
            logger_<< logs::debug << __func__ << "Comparing with phone Nr " << phone->phoneNr_;
            if (phone->phoneNr_ == phoneNr) return radio;
        }
    }

    return std::nullopt;
}


