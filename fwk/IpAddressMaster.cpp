//
// Created by vlads on 9/5/2026.
//

#include "IpAddressMaster.h"

#include "../utils/UtilsFunctions.h"


IpAddressPtr IpAddressMaster::getNextIpAddress() {
    logger_ << logs::info << "IP address requested as role: " << (role_ == Role::MASTER ? "MASTER" : "RADIO");
    switch (role_) {
        case Role::MASTER:
            return getIpMaster();
        case Role::RADIO:
            return getIpRadio();
        case Role::PHONE:

        default:
            logger_<< logs::error << "Unknown role received, " << role_ << ", returning no IpAddress";
            return nullptr;
    }

}


IpAddressPtr IpAddressMaster::getIpMaster() const {
    const auto lastIpInMap = utils::parseIpAddress(idMap_.back()->ipAddress_->getIpAddress());
    const int byte = std::stoi(lastIpInMap[1])+1;
    return std::make_shared<IpAddress>(lastIpInMap[0],std::to_string(byte),"1","1");
}

IpAddressPtr IpAddressMaster::getIpRadio() const {
    const auto lastIpInMap = utils::parseIpAddress(idMap_.back()->ipAddress_->getIpAddress());
    const int byte = std::stoi(lastIpInMap[2])+1;
    return  std::make_shared<IpAddress>(lastIpInMap[0],lastIpInMap[1],std::to_string(byte),"1");
}

IpAddressPtr IpAddressMaster::getIpPhone() const {
    const auto lastIpInMap = utils::parseIpAddress(idMap_.back()->ipAddress_->getIpAddress());
    const int byte = std::stoi(lastIpInMap[3])+1;
    return  std::make_shared<IpAddress>(lastIpInMap[0],lastIpInMap[1],lastIpInMap[2],std::to_string(byte));
}


void IpAddressMaster::addToIdMap(const DeviceIdPtr& deviceId) {
    idMap_.push_back(deviceId);
}

void IpAddressMaster::addtoExternalRadioToPhonesMap(const DeviceIdPtr &deviceId, const std::vector<DeviceIdPtr> &deviceIdVec) {
    for (auto& pair : externalRadioToPhonesMap_) {
        if (*pair.first == *deviceId) {
            *pair.first = *deviceId;
            pair.second = deviceIdVec;
            pair.second = deviceIdVec;
            logger_<< logs::debug << "Updated externalRadioToPhonesMap_ for " << pair.first->ipAddress_->getIpAddress();
            break;
        }
    }
}
