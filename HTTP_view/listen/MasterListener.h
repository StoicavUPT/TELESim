//
// Created by vlads on 9/25/2026.
//

#ifndef TELESIM_MASTERLISTENER_H
#define TELESIM_MASTERLISTENER_H
#include "IListen.h"
#include "../../utils/Logger.h"
#include "../../devices/info/DeviceId.h"

class MasterRadio;

class MasterListener : public IListen {
public:
    MasterListener(
        int listenPort,
        const DeviceIdPtr& deviceId,
        MasterRadio* owner
    );

    void prepareServer() override;
    void stop() override;
    void waitUntilStarted();
private:

    void onPostMasterPhoneVector(const httplib::Request& , httplib::Response&);
    void onGetMasterPhoneOwner(const httplib::Request& , httplib::Response&);


    MasterRadio* owner_;
    utils::Logger logger_;
};

using MasterListenerPtr = std::shared_ptr<MasterListener>;


#endif //TELESIM_MASTERLISTENER_H
