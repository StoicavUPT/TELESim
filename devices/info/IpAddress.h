//
// Created by vlads on 9/5/2026.
//

#ifndef TELESIM_IPADDRESS_H
#define TELESIM_IPADDRESS_H
#include <memory>
#include <vector>

#include "../../utils/UtilsFunctions.h"


class IpAddress {
public:
    IpAddress(
        const std::string& byte1,
        const std::string& byte2,
        const std::string& byte3,
        const std::string& byte4):
    byte1_{byte1},
    byte2_{byte2},
    byte3_{byte3},
    byte4_{byte4}
    {}
    IpAddress(const std::vector<std::string>& bytes):
    byte1_{bytes[0]},
    byte2_{bytes[1]},
    byte3_{bytes[2]},
    byte4_{bytes[3]}
    {}
    IpAddress(const std::string& ipAddress)
    {
        const auto bytes = utils::parseIpAddress(ipAddress);
        byte1_ = bytes[0];
        byte2_ = bytes[1];
        byte3_ = bytes[2];
        byte4_ = bytes[3];
    }
    IpAddress(const std::shared_ptr<IpAddress>& ipAddress)
    {
        const auto bytes = utils::parseIpAddress(ipAddress->getIpAddress());
        byte1_ = bytes[0];
        byte2_ = bytes[1];
        byte3_ = bytes[2];
        byte4_ = bytes[3];
    }


    std::string getIpAddress();
private:
    std::string  byte1_;
    std::string  byte2_;
    std::string  byte3_;
    std::string  byte4_;

};

using IpAddressPtr = std::shared_ptr<IpAddress>;

#endif //TELESIM_IPADDRESS_H
