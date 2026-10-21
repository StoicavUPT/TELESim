//
// Created by vlads on 8/23/2026.
//

#include "Phone.h"
#include "../HTTP_view/emit/PhoneEmitter.h"


Phone::Phone(const DeviceIdPtr &deviceId, const int &ownerPort):
    Device(deviceId),
    logger_("Phone|"+ deviceId->mac_),
    ownerPort_(ownerPort){
    emitter_ = std::make_shared<PhoneEmitter>();
    listener_ = std::make_shared<PhoneListener>(deviceId->port_, deviceId, this);
    listener_->prepareServer();
    listenerThread_ = std::thread([this]() {
        listener_->listen(logger_);
    });
    listener_->waitUntilStarted();
    sendAdditionalInfo();
}

Phone::~Phone() {
    listener_->stop();
    if (listenerThread_.joinable())
    {
        listenerThread_.join();
    }
}

bool Phone::isInCall() {
    return inCall_;
}


void Phone::startPhone() {
    while (true) {

        std::string input;
        std::getline(std::cin, input);

        logger_ << logs::debug
                << "INPUT RECEIVED: " << input
                << " waitingForCallResponse = "
                << waitingForCallResponse();

        if (waitingForCallResponse()) {

            logger_ << logs::debug << "INPUT IS CALL RESPONSE";

            if (input == "1") {
                acceptIncomingCall();
            }
            else{
                declineIncomingCall();
            }

        }
        else if (isInCall()) {

            logger_ << logs::debug << "INPUT IS CALL MESSAGE";

            sendCallMessage(input);

        }
        else {

            logger_ << logs::debug << "INPUT IS PHONE NUMBER";

            initCall(input);

        }
    }
}

void Phone::acceptCall(const std::string& phoneNr) {

    std::cout<<"Call initiated, start texting!" << std::endl;

    ongoingCallPhoneNumber_ = phoneNr;

    {
        std::lock_guard lock(inCallMutex);
        inCall_ = true;
    }
}

bool Phone::waitingForCallResponse() {
    std::lock_guard lock(incomingCallMutex_);
    return waitingForCallResponse_;
}

void Phone::sendCallMessage(const std::string& message) {

    emitter_->postRadioCallPacket(
        ongoingCallId_,
        message,
        ownerPort_,
        ongoingCallPhoneNumber_
    );

    if (message == "0") {
        endCall();
    }
}

void Phone::acceptIncomingCall() {

    {
        std::lock_guard lock(incomingCallMutex_);

        incomingCallAccepted_ = true;
        waitingForCallResponse_ = false;
    }

    ongoingCallPhoneNumber_ = incomingCallPhoneNumber_;
    ongoingCallId_ = incomingCallPhoneNumber_ + deviceId_->phoneNr_ ;

    {
        std::lock_guard lock(inCallMutex);
        inCall_ = true;
    }
    std::cout<<"Call initiated, start texting!" << std::endl;
    incomingCallCv_.notify_one();
}


void Phone::declineIncomingCall() {

    {
        std::lock_guard lock(incomingCallMutex_);

        incomingCallAccepted_ = false;
        waitingForCallResponse_ = false;
    }

    incomingCallCv_.notify_one();
}


bool Phone::incomingCall(const std::string& phoneNr) {

    std::cout << "Receiving Call from phoneNr "
              << phoneNr
              << "  1-accept|0-decline\n";

    {
        std::lock_guard lock(incomingCallMutex_);

        incomingCallPhoneNumber_ = phoneNr;
        waitingForCallResponse_ = true;
    }

    std::unique_lock lock(incomingCallMutex_);

    incomingCallCv_.wait(lock, [this]() {
        return !waitingForCallResponse_;
    });

    return incomingCallAccepted_;
}

void Phone::endCall() {

    system("cls");
    std::cout<<deviceId_->phoneNr_<< std::endl;
    {
        std::lock_guard lock(inCallMutex);
        inCall_ = false;
    }
}

void Phone::sendAdditionalInfo() {
    emitter_->postRadioNotifPhoneNumber(ownerPort_,deviceId_);
}

void Phone::handleCallPacket(const std::string &message) {

    std::cout<<ongoingCallPhoneNumber_<<": "<<message<< std::endl;
    if (message == "0") {
        endCall();
    }
}

void Phone::initCall(const std::string& phoneNr) {
    if (phoneNr == deviceId_->phoneNr_) {
        std::cout<<"Can t call yourself dummy"<<std::endl;
        return;
    }
    const auto status = emitter_->postRadioInitCall(ownerPort_,phoneNr,deviceId_);
    if (status == 200) {
        ongoingCallId_ = deviceId_->phoneNr_+phoneNr;
        logger_<< logs::debug << "Setting ongoingCallId_ = " << ongoingCallId_;
        acceptCall(phoneNr);
    }
    else{
        std::cout<<"Call Failed!"<<std::endl;
    }
    logger_ << logs::info << "Call made with status " << status;
}