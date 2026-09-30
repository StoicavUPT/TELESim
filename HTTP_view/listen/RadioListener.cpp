//
// Created by vlads on 9/25/2026.
//

#include "IListen.h"
#include "RadioListener.h"
#include "../APIEndPoint.h"
#include "../../devices/Radio.h"
#include "../../libs/json.hpp"

std::mutex additionalInfoMutex;

RadioListener::RadioListener(
    int listenPort,
    const DeviceIdPtr& deviceId,
    Radio* owner)
    : IListen(listenPort, deviceId),
      logger_("RadioListener|" + deviceId->mac_),
      owner_(owner)
{
}

void RadioListener::onPostRadioInitCall(const httplib::Request &req, httplib::Response &res) {
    logger_<< logs::info << "Handling "<< RADIO_POST_INIT_CALL ;
    std::cout<<"Initiating call..."<<std::endl;
    nlohmann::json body = nlohmann::json::parse(req.body);
    const auto sender = body["sender"];
    const auto senderPhoneNr = sender["phoneNr"].get<std::string>();
    const auto receiverPhoneNr = body["receiver"]["phoneNr"].get<std::string>();
    const auto maybePhoneOwner = owner_->getPhoneRadio(receiverPhoneNr);


    if (!maybePhoneOwner.has_value()) {
        res.status = 400;
        std::cout<<"Call failed"<<std::endl;
    }
    else {
        const auto& phoneOwner =*maybePhoneOwner;
        if (*phoneOwner != *owner_->deviceId_) {

            res.status = owner_->postRadioConnectCall(
                senderPhoneNr,
                owner_->deviceId_,
                phoneOwner,
                receiverPhoneNr);

            if (res.status == 200) {
                std::cout<<"Call initiated"<<std::endl;
                auto callId = std::make_shared<CallId>();

                callId->id = senderPhoneNr + receiverPhoneNr;
                callId->senderPhoneNr = senderPhoneNr;
                callId->receiverPhoneNr = receiverPhoneNr;

                callId->senderRadioId = owner_->deviceId_;
                callId->receiverRadioId = phoneOwner;

                owner_->registerCall(callId);
            }else {
                std::cout<<"Call failed"<<std::endl;
            }
        }
        else {
            res.status = owner_->postPhoneConnectCall(
                sender["phoneNr"].get<std::string>(),
                *owner_->isPhoneOwner(receiverPhoneNr));
            if (res.status == 200) {
                std::cout<<"Call initiated"<<std::endl;
                auto callId = std::make_shared<CallId>();

                callId->id = senderPhoneNr + receiverPhoneNr;
                callId->senderPhoneNr = senderPhoneNr;
                callId->receiverPhoneNr = receiverPhoneNr;

                callId->senderRadioId = owner_->deviceId_;
                callId->receiverRadioId = phoneOwner;

                owner_->registerCall(callId);
            }else {
                std::cout<<"Call failed"<<std::endl;
            }
        }
    }
    res.set_content("OK", "text/plain");
}

void RadioListener::onPostRadioAdditionalInfo(const httplib::Request& req, httplib::Response& res) {
    logger_<< logs::info << "Handling "<< RADIO_POST_ADDITIONAL_INFO ;
    nlohmann::json body = nlohmann::json::parse(req.body);
    auto mac = body["mac"].get<std::string>();
    auto ip = body["ip"].get<std::string>();
    auto phoneNr = body["phoneNr"].get<std::string>();
    auto port = body["port"].get<int>();

    logger_ << logs::info << "Received Info :" <<
        "\nMAC: " << mac<<
            "\nIP: "<< ip<<
                "\nphoneNr: "<< phoneNr<<
                    "\nport: "<< port;
    auto deviceId = std::make_shared<DeviceId>(
        mac,
        std::make_shared<IpAddress>(ip)
    );

    {
        std::lock_guard lock(additionalInfoMutex);

        owner_->completeAdditionalInfo(
            deviceId,
            port,
            phoneNr
        );
    }
    res.status = 200;
    res.set_content("OK", "text/plain");
}

void RadioListener::onPostRadioConnectCall(const httplib::Request &req, httplib::Response &res) {
    logger_<< logs::info << "Handling "<< RADIO_POST_CONNECT_CALL ;
    nlohmann::json body = nlohmann::json::parse(req.body);
    const auto senderPhoneNr = body["sender"]["phoneNr"].get<std::string>();
    const auto receiverPhoneNr = body["receiver"]["phoneNr"].get<std::string>();
    if (const auto port = owner_->isPhoneOwner(receiverPhoneNr)) {

        std::cout<<"Initiating call..."<<std::endl;

        res.status = owner_->postPhoneConnectCall(
            senderPhoneNr,
            *port);

        if (res.status == 200) {
            const auto id = senderPhoneNr+receiverPhoneNr;
            const auto ownerIp = std::make_shared<IpAddress>(
                body["sender"]["owner"]["ip"].get<std::string>()
            );

            std::cout<<"Call initiated."<<std::endl;
            const auto ownerMac =
                body["sender"]["owner"]["mac"].get<std::string>();

            const auto ownerPort =
                body["sender"]["owner"]["port"].get<int>();
            const auto ownerDeviceId = std::make_shared<DeviceId>(ownerMac,ownerIp,ownerPort);
            auto callId = std::make_shared<CallId>();
            callId->senderPhoneNr = senderPhoneNr;
            callId->receiverPhoneNr = receiverPhoneNr;
            callId->id = id;
            callId->receiverRadioId = owner_->deviceId_;
            callId->senderRadioId = ownerDeviceId;
            logger_ << logs::debug << "callId ptr = " << callId.get();
            logger_ << logs::debug << "callId id = " << callId->id;
            logger_ << logs::debug << "sender phone = " << callId->senderPhoneNr;
            logger_ << logs::debug << "receiver phone = " << callId->receiverPhoneNr;
            owner_->registerCall(callId);


            res.status = 200;
        }
    }
    else {

        std::cout<<"Call failed."<<std::endl;
        res.status = 400;
    }
    res.set_content("OK", "text/plain");
    logger_ << logs::debug
        << "Returning RADIO/POST/connect_call with status "
        << res.status;
}

void RadioListener::onPostRadioCallPacket(const httplib::Request &req, httplib::Response &res) {
    logger_<< logs::info << "Handling "<< RADIO_POST_CALL_PACKET ;

    nlohmann::json body = nlohmann::json::parse(req.body);
    const auto callId = body["callId"].get<std::string>();
    const auto message = body["message"].get<std::string>();
    const auto isSender = body["isSender"].get<boolean>();
    std::cout<<"Received packet with callId: "<<callId<<std::endl;
    logger_ << logs::info
            << "Received Packet with callId "
            << callId
            << " message "
            << message;

    owner_->handlePacket(callId,message,isSender);

    res.status = 200;
    res.set_content("OK", "text/plain");
}

void RadioListener::prepareServer() {
    server_.Post(RADIO_POST_ADDITIONAL_INFO,
        [this](const httplib::Request& req, httplib::Response& res) {
            onPostRadioAdditionalInfo(req,res);
        });

    server_.Post(RADIO_POST_INIT_CALL,
        [this](const httplib::Request& req, httplib::Response& res) {
            onPostRadioInitCall(req,res);
        });

    server_.Post(RADIO_POST_CONNECT_CALL,
        [this](const httplib::Request& req, httplib::Response& res) {
            onPostRadioConnectCall(req,res);
        });

    server_.Post(RADIO_POST_CALL_PACKET,
        [this](const httplib::Request& req, httplib::Response& res) {
            onPostRadioCallPacket(req,res);
        });

    logger_ << logs::info << "Prepared callbacks for RADIO Listener";
}

void RadioListener::waitUntilStarted()
{
    std::unique_lock lock(serverMutex_);

    serverCv_.wait(lock, [this]()
    {
        return serverStarted_ || serverStartFailed_;
    });
}


void RadioListener::stop() {
    server_.stop();
}
