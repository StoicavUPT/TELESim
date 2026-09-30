//
// Created by vlads on 8/30/2026.
//

#ifndef TELESIM_DEVICEFACTORY_H
#define TELESIM_DEVICEFACTORY_H
#include "../utils/Logger.h"
#include "../devices/info/IpAddress.h"
#include "../devices/info/DeviceId.h"

class Radio;
class MasterRadio;
class Phone;

using RadioPtr = std::shared_ptr<Radio>;
using MasterRadioPtr = std::shared_ptr<MasterRadio>;
using PhonePtr = std::shared_ptr<Phone>;

class DeviceFactory {
public:
    DeviceFactory():
    logger_("DeviceFactory") {
    }

    RadioPtr createRadio(const DeviceIdPtr& deviceId, int port);
    MasterRadioPtr createMasterRadio(const DeviceIdPtr& deviceId, int port);
    PhonePtr createPhone(const DeviceIdPtr& deviceId, int port, int ownerPort);
    DeviceIdPtr createDeviceId(const IpAddressPtr& ipAddress);

private:


    utils::Logger logger_;



};

using DeviceFactoryPtr = std::shared_ptr<DeviceFactory>;



#endif //TELESIM_DEVICEFACTORY_H
