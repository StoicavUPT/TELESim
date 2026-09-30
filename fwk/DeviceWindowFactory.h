//
// Created by vlads on 8/30/2026.
//

#ifndef TELESIM_DEVICEWINDOWFACTORY_H
#define TELESIM_DEVICEWINDOWFACTORY_H

#include "IpAddressMaster.h"
#include "../utils/Logger.h"
#include "../utils/ERole.h"

class DeviceWindowFactory {
public:
    DeviceWindowFactory()
    {}

    static void createDeviceWindow(Role::ERole, const DeviceIdPtr&, int ownerPort=0);
private:
    static utils::Logger logger_;

};



using DeviceWindowFactoryPtr = std::shared_ptr<DeviceWindowFactory>;


#endif //TELESIM_DEVICEWINDOWFACTORY_H
