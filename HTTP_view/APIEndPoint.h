//
// Created by vlads on 9/26/2026.
//

#ifndef TELESIM_APIENDPOINT_H
#define TELESIM_APIENDPOINT_H
#include <string>

const auto IP = std::string("127.0.0.1");
const auto TYPE = std::string("application/json");

const auto RADIO = std::string("RADIO");
const auto MASTER = std::string("MASTER");
const auto PHONE = std::string("PHONE");

const auto POST = std::string("/POST");
const auto GET = std::string("/GET");

const auto RADIO_POST = RADIO + POST;
const auto MASTER_POST = MASTER + POST;
const auto PHONE_POST = PHONE + POST;

const auto MASTER_GET = MASTER + GET;

const auto RADIO_POST_ADDITIONAL_INFO= RADIO_POST + "/additional_info";
const auto RADIO_POST_INIT_CALL = RADIO_POST + "/init_call";
const auto RADIO_POST_CONNECT_CALL = RADIO_POST + "/connect_call";
const auto RADIO_POST_CALL_PACKET = RADIO_POST+"/call_packet";

const auto MASTER_POST_PHONE_VECTOR = MASTER_POST + "/phone_vector";
const auto MASTER_GET_PHONE_OWNER = MASTER_GET + "/phone_owner";

const auto PHONE_POST_CONNECT_CALL = PHONE_POST + "/connect_call";
const auto PHONE_POST_CALL_PACKET = PHONE_POST + "/call_packet";


#endif //TELESIM_APIENDPOINT_H
