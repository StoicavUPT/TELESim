//
// Created by vlads on 9/5/2026.
//

#include "IpAddress.h"


std::string IpAddress::getIpAddress() {
    return byte1_+"."+byte2_+"."+byte3_+"."+byte4_;
}
