//
// Created by vlads on 9/25/2026.
//

#ifndef TELESIM_PHONEEMITTER_H
#define TELESIM_PHONEEMITTER_H
#include "IEmit.h"
#include "../../utils/Logger.h"
#include "../../devices/info/DeviceId.h"
#include "../../libs/httplib.h"


class PhoneEmitter: public IEmit {
public:
    PhoneEmitter():
    logger_("PhoneEmitter")
    {}

void postRadioNotifPhoneNumber(int, const DeviceIdPtr&);
int postRadioInitCall(int radioPort,
    const std::string& phoneNr,
    const DeviceIdPtr& senderId);
void postRadioCallPacket(
    const std::string& callId,
    const std::string& message,
    int radioPort,
    const std::string& receiverPhoneNr);

private:
    utils::Logger logger_;
};

using PhoneEmitterPtr = std::shared_ptr<PhoneEmitter>;


#endif //TELESIM_PHONEEMITTER_H
