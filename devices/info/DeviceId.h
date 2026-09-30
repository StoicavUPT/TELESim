//
// Created by vlads on 8/23/2026.
//

#ifndef TELESIM_DEVICEID_H
#define TELESIM_DEVICEID_H
#include <memory>
#include <optional>

#include "IpAddress.h"

struct DeviceId {

    DeviceId(const std::string& mac, const IpAddressPtr& ipAddress, int port = 0) :
    mac_(mac),
    ipAddress_(ipAddress),
    port_(port)
    {}
    DeviceId()
    {}
    DeviceId(const std::shared_ptr<DeviceId>& other):
    mac_(other->mac_),
    ipAddress_(other->ipAddress_),
    port_(other->port_),
    phoneNr_(other->phoneNr_)
    {

    }
    std::string mac_{};
    IpAddressPtr ipAddress_{};
    std::string phoneNr_{};
    int port_{};

    bool operator==(const DeviceId &) const;

};

using DeviceIdCPtr = std::shared_ptr<const DeviceId>;
using DeviceIdPtr = std::shared_ptr<DeviceId>;

#endif //TELESIM_DEVICEID_H
