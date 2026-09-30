//
// Created by vlads on 8/23/2026.
//

#include "DeviceId.h"


bool DeviceId::operator==(const DeviceId &other) const {
    return this->ipAddress_->getIpAddress()==other.ipAddress_->getIpAddress() &&
        this->mac_==other.mac_;
}
