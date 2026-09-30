//
// Created by vlads on 9/25/2026.
//

#include "../emit/RadioEmitter.h"

#include "../../libs/httplib.h"
#include "../../libs/json.hpp"
#include "../APIEndPoint.h"
#include "../../devices/info/CallId.h"


void RadioEmitter::postMasterPhoneVector(const DeviceIdPtr& ownerId,const std::vector<DeviceIdPtr> &deviceIds) {
    httplib::Client client(IP,MASTER_PORT);
    nlohmann::json body;
    nlohmann::json owner;
    nlohmann::json phones;

    owner["ip"] = ownerId->ipAddress_->getIpAddress();
    owner["mac"] = ownerId->mac_;
    owner["port"] = ownerId->port_;

    for (const auto& deviceId : deviceIds) {
        nlohmann::json device;
        device["ip"] = deviceId->ipAddress_->getIpAddress();
        device["mac"] = deviceId->mac_;
        device["port"] = deviceId->port_;
        device["phoneNr"] = deviceId->phoneNr_;
        phones.push_back(device);
    }

    body["owner"] = owner;
    body["phones"] = phones;

    logger_ << logs::info << "Sending " << MASTER_POST_PHONE_VECTOR << " with body " << body << " to owner port " << MASTER_PORT;

    const auto response =client.Post(MASTER_POST_PHONE_VECTOR,
        body.dump(),
        TYPE);
    if (response)
    {
        logger_ << logs::info
                << "Received response for "
                << MASTER_POST_PHONE_VECTOR
                << " status: "
                << response->status
                << " body: "
                << response->body;
    }
    else
    {
        logger_ << logs::error
            << "POST failed for "
            << MASTER_POST_PHONE_VECTOR
            << " with body "
            << body
            << " \nerror: "
            << httplib::to_string(response.error());
    }
}



std::optional<DeviceIdPtr> RadioEmitter::getMasterPhoneOwner(const std::string &phoneNr) {
    httplib::Client client(IP,MASTER_PORT);

    const auto getPath = MASTER_GET_PHONE_OWNER+"?phoneNr="+phoneNr;

    logger_ << logs::info << "sending " << getPath << " to port " << MASTER_PORT;

    const auto res = client.Get(getPath);
    logger_ << logs::info << "Received response for " << getPath << " with status " << res->status << " and body "<<res->body;

    if (!res || res->status != 200) {

        return std::nullopt;
    }

    nlohmann::json response = nlohmann::json::parse(res->body);

    const auto ip = std::make_shared<IpAddress>(response["ip"].get<std::string>());
    const auto mac = response["mac"].get<std::string>();
    const auto port = response["port"].get<int>();

    return std::make_shared<DeviceId>(mac,ip,port);
}

void RadioEmitter::postPhoneCallPacket(const std::string &message, int phonePort) {

    httplib::Client client(IP,phonePort);

    nlohmann::json body;

    body["message"] = message;

    logger_ << logs::info << "Sending " << PHONE_POST_CALL_PACKET << " with body " << body.dump() << " to radio port " << phonePort;

    const auto response = client.Post(
        PHONE_POST_CALL_PACKET,
        body.dump(),
        TYPE);

    if (response)
    {
        logger_ << logs::info
                << "Received response for "
                << PHONE_POST_CALL_PACKET
                << " status: "
                << response->status
                << " body: "
                << response->body;
    }
    else
    {
        logger_ << logs::error
            << "POST failed for "
            << PHONE_POST_CALL_PACKET
            << " with body "
            << body
            << " \nerror: "
            << httplib::to_string(response.error());
    }

}
void RadioEmitter::postRadioCallPacket(
    const std::string &callId,
    const std::string &message,
    int radioPort,
    const std::string& receiverPhoneNr) {

    httplib::Client client(IP,radioPort);
    nlohmann::json body;

    body["callId"] = callId;
    body["message"] = message;
    body["isSender"] = false;
    body["receiverPhoneNr"] = receiverPhoneNr;

    logger_ << logs::info << "Sending " << RADIO_POST_CALL_PACKET << " with body " << body.dump() << " to radio port " << radioPort;

    std::cout<<"Sent packet with callId: "<<callId<<std::endl;
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

int RadioEmitter::postRadioConnectCall (
        const std::string& senderPhoneNr,
        const DeviceIdPtr& senderOwnerId,
        const DeviceIdPtr& receiverOwnerId,
        const std::string& receiverPhoneNr){

    httplib::Client client(IP,receiverOwnerId->port_);

    nlohmann::json body;
    nlohmann::json receiver;
    nlohmann::json sender;
    nlohmann::json owner;

    receiver["phoneNr"]= receiverPhoneNr;
    sender["phoneNr"] = senderPhoneNr;
    owner["ip"] = senderOwnerId->ipAddress_->getIpAddress();
    owner["mac"] = senderOwnerId->mac_;
    owner["port"] = senderOwnerId->port_;
    sender["owner"] = owner;

    body["receiver"] = receiver;
    body["sender"] = sender;

    logger_ << logs::info << "Sending " << RADIO_POST_CONNECT_CALL << " with body " << body.dump() << " to owner port " << receiverOwnerId->port_;

    const auto response = client.Post(RADIO_POST_CONNECT_CALL,
        body.dump(),
        TYPE);

    if (response)
    {
        logger_ << logs::info
                << "Received response for "
                << RADIO_POST_CONNECT_CALL
                << " status: "
                << response->status
                << " body: "
                << response->body;

    }
    else
    {
        logger_ << logs::error
            << "POST failed for "
            << RADIO_POST_CONNECT_CALL
            << " with body "
            << body
            << " \nerror: "
            << httplib::to_string(response.error());
    }

    return response->status;

}

int RadioEmitter::postPhoneConnectCall(const std::string &senderPhoneNr, int phonePort) {

    httplib::Client client(IP,phonePort);

    nlohmann::json body;
    nlohmann::json sender;
    sender["phoneNr"] = senderPhoneNr;

    body["sender"] = sender;

    logger_ << logs::info << "Sending " << PHONE_POST_CONNECT_CALL << " with body " << body.dump() << " to owner port " << phonePort;

    const auto response = client.Post(PHONE_POST_CONNECT_CALL,
        body.dump(),
        TYPE);

    if (response)
    {
        logger_ << logs::info
                << "Received response for "
                << PHONE_POST_CONNECT_CALL
                << " status: "
                << response->status
                << " body: "
                << response->body;
    }
    else
    {
        logger_ << logs::error
            << "POST failed for "
            << PHONE_POST_CONNECT_CALL
            << " with body "
            << body
            << " \nerror: "
            << httplib::to_string(response.error());
    }
    return response->status;
}