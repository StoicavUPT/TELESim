//
// Created by vlads on 9/24/2026.
//

#include "UIHandler.h"


void UIHandler::handleMaster(const MasterRadioPtr& master, const DeviceFactoryPtr& deviceFactory) {
    std::cout<<"MASTER\n";
    master->createRadio(deviceFactory);
    master->createRadio(deviceFactory);
    int a;
    std::cin>>a;
}

void UIHandler::handleRadio(const RadioPtr& radio, const DeviceFactoryPtr& deviceFactory) {
    std::cout<<"RADIO\n";
    radio->createPhone(deviceFactory);
    radio->createPhone(deviceFactory);
    int a;
    std::cin>>a;
}

void UIHandler::handlePhone(const PhonePtr& phone, const DeviceFactoryPtr& deviceFactory) {
    std::cout << phone->deviceId_->phoneNr_<< std::endl;
    phone->startPhone();
}