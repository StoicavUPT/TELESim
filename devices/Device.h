//
// Created by vlads on 8/23/2026.
//

#ifndef TELESIM_DEVICE_H
#define TELESIM_DEVICE_H
#include <string>

#include "../fwk/DeviceFactory.h"
#include "../fwk/DeviceWindowFactory.h"
#include "../HTTP_view/emit/IEmit.h"
#include "../HTTP_view/listen/IListen.h"
#include "info/DeviceId.h"

class Device {
public:
    Device(const DeviceIdPtr& deviceId):
    deviceId_(deviceId),
    deviceFactory_(std::make_shared<DeviceFactory>())
    {}
    DeviceIdPtr deviceId_;
protected:
    DeviceFactoryPtr deviceFactory_;

};

using DevicePtr = std::shared_ptr<Device>;

#endif //TELESIM_DEVICE_H
