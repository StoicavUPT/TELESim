//
// Created by vlads on 9/27/2026.
//

#ifndef TELESIM_CALLID_H
#define TELESIM_CALLID_H
#include <string>

#include "DeviceId.h"

struct CallId {
    std::string receiverPhoneNr{};
    std::string senderPhoneNr{};
    DeviceIdPtr receiverRadioId{};
    DeviceIdPtr senderRadioId{};
    std::string id;
};

using CallIdPtr = std::shared_ptr<CallId>;

#endif //TELESIM_CALLID_H
