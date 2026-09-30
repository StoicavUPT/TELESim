//
// Created by vlads on 8/30/2026.
//

#include "DeviceInfoFileParser.h"

#include <filesystem>
#include <fstream>

#include "UtilsFunctions.h"

utils::Logger DeviceInfoFileParser::logger_{"DeviceInfoFileParser"};

std::vector<std::string> DeviceInfoFileParser::getFileInput(const std::string &fileName) {
    std::ifstream file;
    file.open(std::filesystem::path(PROJECT_PATH));
    if (!file.is_open()) {
        logger_ << logs::error << "Failure for opening file for input reading of device info.";
    }
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file,line)) {
        logger_<< logs::info << "EXTRACTED LINE: "<< line;
        lines.push_back(line);
    }

    return lines;
}

DeviceIdPtr DeviceInfoFileParser::parseFileInput(const std::vector<std::string> &lines) {
    DeviceIdPtr deviceId = std::make_shared<DeviceId>();
    deviceId->mac_ = lines[0];
    const auto ip = utils::parseIpAddress(lines[1]);
    deviceId->ipAddress_ = std::make_shared<IpAddress>(ip[0],ip[1],ip[2],ip[3]);
    return deviceId;
}
