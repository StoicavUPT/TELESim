
#include "devices/Device.h"
#include <iostream>
#include <windows.h>

#include "devices/MasterRadio.h"
#include "fwk/UIHandler.h"
#include "utils/Logger.h"
#include "utils/UtilsFunctions.h"


int main(int argc, char* argv[]) {

    char exePath[MAX_PATH];

    GetModuleFileNameA(
        nullptr,
        exePath,
        MAX_PATH
    );

    utils::Logger logger_("Main_DeviceWindow");
    logger_ << logs::info<< "EXE: " << exePath;
    logger_ << logs::info<< "PID: " << GetCurrentProcessId();
    logger_ << logs::info << "Started new window";
    logger_ << logs::info << "Initializing device factory";

    const auto givenRole = static_cast<Role::ERole>(std::stoi(argv[2]));
    const auto port = std::stoi(argv[3]);
    const auto ipAddress = argv[4];
    const auto mac = argv[5];
    logger_  << logs::info << "Received info: "<<
        " Role: " << givenRole <<
            " IP: "<< ipAddress <<
                " Mac: " << mac;
    auto ipBytes = utils::parseIpAddress(ipAddress);
    auto deviceId = std::make_shared<DeviceId>(mac,std::make_shared<IpAddress>(ipBytes),port);
    const auto uiHandler = std::make_shared<UIHandler>();





    auto deviceFactory = std::make_shared<DeviceFactory>();

    switch (givenRole) {
        case Role::MASTER:
            uiHandler->handleMaster(deviceFactory->createMasterRadio(deviceId,port),deviceFactory);
            break;
        case Role::RADIO:
            uiHandler->handleRadio(deviceFactory->createRadio(deviceId,port),deviceFactory);
            break;
        case Role::PHONE:
            const auto ownerPort = argv[6];

            deviceId->phoneNr_=utils::createPhoneNumber();
            logger_ << logs::info << "Created phone number: " << deviceId->phoneNr_ << " for phone with MAC: "<< mac;
            uiHandler->handlePhone(deviceFactory->createPhone(deviceId,port,std::stoi(ownerPort)),deviceFactory);
            break;
    }

    return 0;
}
