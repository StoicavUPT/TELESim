//
// Created by vlads on 9/25/2026.
//

#ifndef TELESIM_ILISTEN_H
#define TELESIM_ILISTEN_H
#include "../../utils/Logger.h"
#include "../../devices/info/IpAddress.h"
#include "../../devices/info/DeviceId.h"
#include "../../libs/httplib.h"
#include "../APIEndPoint.h"


class IListen {
public:


    IListen(int listenPort, const DeviceIdPtr& deviceId):
    listenIpAddress_(deviceId->ipAddress_),
    listenPort_(listenPort)
    {}

    virtual ~IListen() = default;

    virtual void prepareServer() = 0;
    void listen( utils::Logger& logger_) {
        logger_ << logs::info << "Starting listener on "<< IP<< ":"<< listenPort_;

        if (!server_.bind_to_port(IP, listenPort_))
        {
            logger_ << logs::error << "Failed to bind listener on "<< IP<< ":"<< listenPort_;
            {
                std::lock_guard<std::mutex> lock(serverMutex_);
                serverStartFailed_ = true;
            }
            serverCv_.notify_one();
            return;
        }
        {
            std::lock_guard<std::mutex> lock(serverMutex_);
            serverStarted_ = true;
        }
        serverCv_.notify_one();
        logger_ << logs::info << "Listener successfully bound on "<< IP<< ":"<< listenPort_;
        server_.listen_after_bind();
        logger_ << logs::info << "Listener stopped";
    }
    virtual void stop() = 0;

protected:

    IpAddressPtr listenIpAddress_;
    int listenPort_;
    httplib::Server server_;
    std::mutex serverMutex_;
    std::condition_variable serverCv_;
    bool serverStarted_ = false;
    bool serverStartFailed_ = false;

};

using IListenPtr = std::shared_ptr<IListen>;

#endif //TELESIM_ILISTEN_H
