#include <windows.h>
#include <iostream>
#include <string>


#include "fwk/DeviceWindowFactory.h"
#include "utils/Logger.h"


int main()
{
    const auto path = std::filesystem::path(PROJECT_PATH)/ "logs"/"LOGS.log";
    std::filesystem::remove(path);

    std::ofstream regFile;

    regFile.open(REG_PATH);
    regFile<<0;
    regFile.close();

    utils::Logger logger_("Main");

    logger_ << logs::info << "App started";
    logger_ << logs::info << "Initializing device window factory";
    const auto deviceFactory = std::make_shared<DeviceFactory>();
    DeviceWindowFactory::createDeviceWindow(Role::MASTER,deviceFactory->createDeviceId(std::make_shared<IpAddress>("192.168.1.1")));
    return 0;
}