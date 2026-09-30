//
// Created by vlads on 9/5/2026.
//

#ifndef TELESIM_IPADDRESSMASTER_H
#define TELESIM_IPADDRESSMASTER_H
#include <map>
#include <utility>
#include <vector>

#include "../devices/Device.h"
#include "../devices/info/DeviceId.h"
#include "../devices/info/IpAddress.h"
#include "../utils/Logger.h"
#include "../utils/ERole.h"




class IpAddressMaster {
public:
    IpAddressMaster(
        Role::ERole role,
        const DeviceIdPtr& ownerId,
        int port):
    ownerId_(std::move(ownerId)),
    port_(port),
    logger_("IpAddressMaster"),
    role_(role) {
        idMap_.push_back(ownerId);
        logger_ << logs::debug << "IP ADDRESS MASTER CREATED WITH ROLE: "<< role;
    }
    IpAddressPtr getNextIpAddress();
    void addToIdMap(const DeviceIdPtr&);
    void addtoExternalRadioToPhonesMap(const DeviceIdPtr& , const std::vector<DeviceIdPtr>&);

private:
    IpAddressPtr getIpMaster() const;
    IpAddressPtr getIpRadio() const;
    IpAddressPtr getIpPhone() const;

    DeviceIdPtr ownerId_;
    std::vector<DeviceIdPtr> idMap_;
    std::map<DeviceIdPtr,std::vector<DeviceIdPtr>> externalRadioToPhonesMap_;
    utils::Logger logger_;
    Role::ERole role_;
    int port_;

    friend class MasterRadio;
    friend class Radio;

};

using IpAddressMasterPtr = std::shared_ptr<IpAddressMaster>;

#endif //TELESIM_IPADDRESSMASTER_H
