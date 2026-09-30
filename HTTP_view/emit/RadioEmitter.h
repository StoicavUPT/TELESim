//
// Created by vlads on 9/25/2026.
//

#ifndef TELESIM_RADIOEMITTER_H
#define TELESIM_RADIOEMITTER_H
#include <vector>

#include "IEmit.h"
#include "../../devices/info/DeviceId.h"
#include "../../utils/Logger.h"


class Radio;

class RadioEmitter : public IEmit{
public:
    RadioEmitter():
    logger_("RadioEmitter")
    {}

    void postMasterPhoneVector(const DeviceIdPtr& owner,const std::vector<DeviceIdPtr>&);
    int postPhoneConnectCall(const std::string& senderPhoneNr, int phonePort);
    int postRadioConnectCall(const std::string& senderPhoneNr,
        const DeviceIdPtr& senderOwnerId,
        const DeviceIdPtr& receiverOwnerId,
        const std::string& receiverPhoneNr);
    std::optional<DeviceIdPtr> getMasterPhoneOwner(const std::string& phoneNr);
    void postRadioCallPacket(const std::string& callId, const std::string& message, int radioPort);
    void postPhoneCallPacket(const std::string& message, int phonePort);
private:
    utils::Logger logger_;
};

using RadioEmitterPtr = std::shared_ptr<RadioEmitter>;


#endif //TELESIM_RADIOEMITTER_H
