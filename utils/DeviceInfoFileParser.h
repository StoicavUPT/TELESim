//
// Created by vlads on 8/30/2026.
//

#ifndef TELESIM_DEVICEINFOFILEPARSER_H
#define TELESIM_DEVICEINFOFILEPARSER_H
#include <vector>

#include "Logger.h"
#include "../devices/info/DeviceId.h"


class DeviceInfoFileParser {

public:
    static DeviceIdPtr parseFileInput(const std::vector<std::string>& lines);

    static std::vector<std::string> getFileInput(const std::string& fileName);
private:
    static utils::Logger logger_;
};



#endif //TELESIM_DEVICEINFOFILEPARSER_H
