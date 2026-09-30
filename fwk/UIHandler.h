//
// Created by vlads on 9/24/2026.
//

#ifndef TELESIM_UIHANDLER_H
#define TELESIM_UIHANDLER_H
#include <memory>

#include "../utils/Logger.h"
#include "../devices/MasterRadio.h"
#include "../devices/Radio.h"
#include "../devices/Phone.h"

class UIHandler {
public:
    UIHandler():
    logger_("UIHandler")
    {}
    void handleRadio(const RadioPtr&, const DeviceFactoryPtr&);
    void handleMaster(const MasterRadioPtr&, const DeviceFactoryPtr&);
    void handlePhone(const PhonePtr&, const DeviceFactoryPtr&);
private:
    utils::Logger logger_;
};
using UIHandlerPtr = std::shared_ptr<UIHandler>;
#endif //TELESIM_UIHANDLER_H
