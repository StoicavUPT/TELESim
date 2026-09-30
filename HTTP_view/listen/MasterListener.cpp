//
// Created by vlads on 9/25/2026.
//

#include "MasterListener.h"
#include "../APIEndPoint.h"
#include "../../devices/MasterRadio.h"
#include "../../libs/json.hpp"
#include "../../libs/httplib.h"

MasterListener::MasterListener(
    int listenPort,
    const DeviceIdPtr& deviceId,
    MasterRadio* owner)
    : IListen(listenPort, deviceId),
      logger_("MasterListener|" + deviceId->mac_),
      owner_(owner)
{}


void MasterListener::prepareServer() {

    server_.Post(
        MASTER_POST_PHONE_VECTOR,
        [this](const httplib::Request &req, httplib::Response &res) {
            onPostMasterPhoneVector(req,res);
        });

    server_.Get(MASTER_GET_PHONE_OWNER,
        [this](const httplib::Request& req, httplib::Response& res) {
            onGetMasterPhoneOwner(req,res);
        });

    logger_ << logs::info << "Prepared callbacks for MASTER Listener";
}


std::mutex phoneVectorMutex;
void MasterListener::onPostMasterPhoneVector(const httplib::Request &req, httplib::Response &res) {
    //logger_<< logs::info << "Handling "<< MASTER_POST_PHONE_VECTOR ;
    nlohmann::json body = nlohmann::json::parse(req.body);
    const auto owner = body["owner"];

    auto ownerMac = owner["mac"].get<std::string>();
    auto ownerIp = owner["ip"].get<std::string>();
    auto ownerPort = owner["port"].get<int>();
    const auto ownerIpAddress = std::make_shared<IpAddress>(ownerIp);
    const auto ownerDeviceId = std::make_shared<DeviceId>(ownerMac,ownerIpAddress,ownerPort);

    logger_ << logs::info << "Received phones info for owner :" <<
    "\nMAC: " << ownerMac<<
        "\nIP: "<< ownerIp<<
            "\nport: "<< ownerPort;

    nlohmann::json phones = body["phones"];
    std::vector<DeviceIdPtr> deviceIdVec{};
    for (const auto& phoneBody: phones) {
        auto mac = phoneBody["mac"].get<std::string>();
        auto ip = phoneBody["ip"].get<std::string>();
        auto phoneNr = phoneBody["phoneNr"].get<std::string>();
        auto port = phoneBody["port"].get<int>();
        const auto ipAddress = std::make_shared<IpAddress>(ip);
        const auto deviceId = std::make_shared<DeviceId>(mac,ipAddress,port);
        deviceId->phoneNr_= phoneNr;
        deviceIdVec.push_back(deviceId);
    }

    {
        std::lock_guard lock(phoneVectorMutex);
        owner_->addtoExternalRadioToPhonesMap(
            ownerDeviceId,
            deviceIdVec
        );
    }
    res.status = 200;
    res.set_content("OK", "text/plain");

}


void MasterListener::onGetMasterPhoneOwner(const httplib::Request &req, httplib::Response &res) {

    logger_<< logs::info << "Handling "<< MASTER_GET_PHONE_OWNER ;

    const auto phoneNr = req.get_param_value("phoneNr");
    logger_<< "Extracted phone number " << phoneNr;
    std::optional<DeviceIdPtr> deviceId;
    {
        std::lock_guard lock(phoneVectorMutex);
        deviceId = owner_->getRadioForPhoneNr(phoneNr);
    }
    if (deviceId.has_value()) {
        nlohmann::json body;
        const auto device = *deviceId;

        body["ip"] = device->ipAddress_->getIpAddress();
        body["mac"] = device->mac_;
        body["port"] = device->port_;

        res.status = 200;
        res.set_content(body.dump(),TYPE);
    }
    else {
        res.status = 400;
        res.set_content("No radio found for phone nr: "+phoneNr,"text/plain");
    }
}


void MasterListener::waitUntilStarted()
{
    std::unique_lock<std::mutex> lock(serverMutex_);

    serverCv_.wait(lock, [this]()
    {
        return serverStarted_ || serverStartFailed_;
    });
}

void MasterListener::stop() {
    server_.stop();
}
