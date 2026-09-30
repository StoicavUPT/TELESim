//
// Created by vlads on 8/31/2026.
//

#ifndef TELESIM_UTILSFUNCTIONS_H
#define TELESIM_UTILSFUNCTIONS_H
#include <string>
#include <vector>
#include <windows.h>
namespace utils {
    std::string createMac();
    std::string createPhoneNumber();
    std::vector<std::string> parseIpAddress(const std::string& ipAddress);
    HWND FindWindowByPID(DWORD pid);
    void FocusCMD(DWORD pid);
}
#endif //TELESIM_UTILSFUNCTIONS_H


