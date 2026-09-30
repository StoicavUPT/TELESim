//
// Created by vlads on 9/25/2026.
//

#ifndef TELESIM_PHONELISTENER_H
#define TELESIM_PHONELISTENER_H
#include "IListen.h"
#include "../../utils/Logger.h"


class Phone;

class PhoneListener : public IListen {
public:
    PhoneListener(int listenPort,
        const DeviceIdPtr& deviceId,
        Phone* owner);
    void prepareServer() override;
    void stop() override;
    void waitUntilStarted();
protected:

    void onPostPhoneConnectCall( const httplib::Request&, httplib::Response&);
    void onPostCallPacket( const httplib::Request&, httplib::Response&);

    Phone* owner_;
    utils::Logger logger_;

};


using PhoneListenerPtr = std::shared_ptr<PhoneListener>;

#endif //TELESIM_PHONELISTENER_H
