//
// Created by vlads on 8/30/2026.
//

#include "DeviceFactory.h"

#include "../devices/MasterRadio.h"
#include "../devices/Phone.h"
#include "../devices/Radio.h"
#include "../utils/UtilsFunctions.h"
#include "../devices/info/DeviceId.h"



DeviceIdPtr DeviceFactory::createDeviceId(const IpAddressPtr& ipAddress) {
    std::string mac;
    mac= utils::createMac();

    logger_ << logs::info << "Creating DeviceId with success for mac " << mac << " ip address " << ipAddress->getIpAddress();
    return std::make_shared<DeviceId>(mac,ipAddress);
}


RadioPtr DeviceFactory::createRadio(const DeviceIdPtr& deviceId, int port) {
    logger_ << logs::info << "Creating Radio with success";
    return std::make_shared<Radio>(deviceId);
}

MasterRadioPtr DeviceFactory::createMasterRadio(const DeviceIdPtr& deviceId, int port) {

    logger_ << logs::info << "Creating Master Radio with success";
    return std::make_shared<MasterRadio>(deviceId);

}


PhonePtr DeviceFactory::createPhone(const DeviceIdPtr& deviceId, int port, int owner) {
    logger_ << logs::info << "Creating Phone with success";
    return std::make_shared<Phone>(deviceId,owner);

}
