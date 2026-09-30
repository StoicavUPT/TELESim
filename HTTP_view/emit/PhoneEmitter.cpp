//
// Created by vlads on 9/25/2026.
//

#include "../emit/PhoneEmitter.h"

#include "../APIEndPoint.h"
#include "../../libs/json.hpp"
#include "../../libs/httplib.h"

void PhoneEmitter::postRadioNotifPhoneNumber(int radioPort, const DeviceIdPtr& deviceId) {
    httplib::Client client(IP,radioPort);

    nlohmann::json body;

    body["ip"] = deviceId->ipAddress_->getIpAddress();
    body["mac"] = deviceId->mac_;
    body["phoneNr"] = deviceId->phoneNr_;
    body["port"] = deviceId->port_;

    logger_ << logs::info << "Sending " << RADIO_POST_ADDITIONAL_INFO << " with body " << body.dump() << " to owner port " << radioPort;
    const auto response = client.Post(RADIO_POST_ADDITIONAL_INFO,
                body.dump(),
                TYPE);
    if (response)
    {
        logger_ << logs::info
                << "Received response for "
                << RADIO_POST_ADDITIONAL_INFO
                << " status: "
                << response->status
                << " body: "
                << response->body;
    }
    else
    {
        logger_ << logs::error
            << "POST failed for "
            << RADIO_POST_ADDITIONAL_INFO
            << " error: "
            << httplib::to_string(response.error());
    }
}void PhoneEmitter::postRadioCallPacket(const std::string &callId, const std::string &message, int radioPort) {

    httplib::Client client(IP,radioPort);
    nlohmann::json body;

    body["callId"] = callId;
    body["message"] = message;
    body["isSender"] = true;

    logger_ << logs::info << "Sending " << RADIO_POST_CALL_PACKET << " with body " << body.dump() << " to radio port " << radioPort;

    const auto response = client.Post(RADIO_POST_CALL_PACKET,
        body.dump(),
        TYPE);

    if (response)
    {
        logger_ << logs::info
                << "Received response for "
                << RADIO_POST_CALL_PACKET
                << " status: "
                << response->status
                << " body: "
                << response->body;
    }
    else
    {
        logger_ << logs::error
            << "POST failed for "
            << RADIO_POST_CALL_PACKET
            << " with body "
            << body
            << " \nerror: "
            << httplib::to_string(response.error());
    }


}

int PhoneEmitter::postRadioInitCall(int radioPort,const std::string& phoneNr, const DeviceIdPtr& senderId) {

    httplib::Client client(IP,radioPort);

    nlohmann::json sender;
    nlohmann::json receiver;
    nlohmann::json body;

    sender["ip"] = senderId->ipAddress_->getIpAddress();
    sender["mac"] = senderId->mac_;
    sender["port"] = senderId->port_;
    sender["phoneNr"] = senderId->phoneNr_;

    receiver["phoneNr"] = phoneNr;

    body["sender"] = sender;
    body["receiver"] = receiver;

    logger_ << logs::info << "Sending " << RADIO_POST_INIT_CALL << " with body " << body.dump() << " to radio port " << radioPort;

    const auto response = client.Post(RADIO_POST_INIT_CALL,
        body.dump(),
        TYPE);
    if (response)
    {
        logger_ << logs::info
                << "Received response for "
                << RADIO_POST_INIT_CALL
                << " status: "
                << response->status
                << " body: "
                << response->body;
    }
    else
    {
        logger_ << logs::error
            << "POST failed for "
            << RADIO_POST_INIT_CALL
            << " with body "
            << body
            << " \nerror: "
            << httplib::to_string(response.error());
    }
    return response->status;
}

