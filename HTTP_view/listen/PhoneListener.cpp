//
// Created by vlads on 8/23/2026.
//

#include "PhoneListener.h"
#include "../APIEndPoint.h"
#include "../../devices/Phone.h"
#include "../../libs/json.hpp"

PhoneListener::PhoneListener(int listenPort, const DeviceIdPtr &deviceId, Phone *owner) :
      IListen(listenPort, deviceId),
      logger_("PhoneListener|" + deviceId->mac_),
      owner_(owner)
{}

void PhoneListener::waitUntilStarted() {
      std::unique_lock<std::mutex> lock(serverMutex_);
      serverCv_.wait(lock, [this]()
      {
          return serverStarted_ || serverStartFailed_;
      });
}

void PhoneListener::onPostPhoneConnectCall(const httplib::Request &req, httplib::Response &res) {
      logger_<< logs::info << "Handling "<< PHONE_POST_CONNECT_CALL ;

      nlohmann::json body = nlohmann::json::parse(req.body);
      const auto phoneNr = body["sender"]["phoneNr"].get<std::string>();
      if (!owner_->isInCall()) {
            logger_<< logs::info << "Phone is not in call";
            const auto accepted = owner_->incomingCall(phoneNr);
            if (accepted) {
                  res.status=200;
                  res.set_content("Call Accepted", "text/plain");
            }
            else {
                  res.status = 401;
                  res.set_content("Call DECLINED by user", "text/plain");
            }
      }
      else {
            res.status = 400;
            res.set_content("User in another call", "text/plain");
      }
}

void PhoneListener::onPostCallPacket(const httplib::Request &req, httplib::Response &res) {
      logger_<< logs::info << "Handling "<< PHONE_POST_CALL_PACKET ;

      nlohmann::json body = nlohmann::json::parse(req.body);
      const auto message = body["message"];
      owner_->handleCallPacket(message);
}

void PhoneListener::prepareServer() {
      server_.Post(PHONE_POST_CONNECT_CALL,
      [this](const httplib::Request& req, httplib::Response& res) {
            onPostPhoneConnectCall(req,res);
      });
      server_.Post(PHONE_POST_CALL_PACKET,
      [this](const httplib::Request& req, httplib::Response& res) {
            onPostCallPacket(req,res);
      });
}

void PhoneListener::stop() {
      server_.stop();
}