#include "DeviceWindowFactory.h"

#include <windows.h>
#include <cstdlib>

#include "IpAddressMaster.h"
#include "../utils/UtilsFunctions.h"

utils::Logger DeviceWindowFactory::logger_{"DeviceWindowFactory"};

void DeviceWindowFactory::createDeviceWindow(
    Role::ERole role,
    const DeviceIdPtr& deviceId,
    int ownerPort)
{
    int id{};

    std::ifstream regFileRead;
    regFileRead.open(REG_PATH);
    regFileRead >> id;
    regFileRead.close();

    id++;

    std::ofstream regFileWrite;
    regFileWrite.open(REG_PATH);
    regFileWrite << id;
    regFileWrite.close();

    const std::string exePath =
        R"(D:\WORK\TELESim\cmake-build-debug\DeviceWindow.exe)";

    const auto port =
        role == Role::MASTER ? MASTER_PORT : 8080 + id;

    std::string command =
        "\"" + exePath + "\" " +
        std::to_string(id) + " " +
        std::to_string(role) + " " +
        std::to_string(port) + " " +
        deviceId->ipAddress_->getIpAddress() + " " +
        deviceId->mac_;

    if (role == Role::PHONE)
    {
        command += " " + std::to_string(ownerPort);
    }

    logger_ << logs::debug
            << "Built command: "
            << command
            << logs::endl;

    STARTUPINFOA si{};
    PROCESS_INFORMATION pi{};

    si.cb = sizeof(si);

    if (!CreateProcessA(
        nullptr,
        command.data(),
        nullptr,
        nullptr,
        FALSE,
        CREATE_NEW_CONSOLE,
        nullptr,
        nullptr,
        &si,
        &pi))
    {
        logger_ << logs::error
                << "Failed to start DeviceWindow"
                << logs::endl;
        return;
    }

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    logger_ << logs::info
            << "New Device Created"
            << logs::endl;
}
