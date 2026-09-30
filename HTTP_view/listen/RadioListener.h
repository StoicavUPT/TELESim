#pragma once

#include "IListen.h"
#include "../../utils/Logger.h"
#include "../../devices/info/DeviceId.h"

class Radio;

class RadioListener : public IListen
{
public:
    RadioListener(
        int listenPort,
        const DeviceIdPtr& deviceId,
        Radio* owner
    );

    void prepareServer() override;
    void stop() override;
    void waitUntilStarted();
private:
    void onPostRadioAdditionalInfo( const httplib::Request&, httplib::Response&);
    void onPostRadioInitCall( const httplib::Request&, httplib::Response&);
    void onPostRadioConnectCall( const httplib::Request&, httplib::Response&);
    void onPostRadioCallPacket( const httplib::Request&, httplib::Response&);

    Radio* owner_;
    utils::Logger logger_;

};

using RadioListenerPtr = std::shared_ptr<RadioListener>;