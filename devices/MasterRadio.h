//
// Created by vlads on 8/31/2026.
//

#ifndef TELESIM_MASTERRADIO_H
#define TELESIM_MASTERRADIO_H
#include <vector>

#include "Device.h"
#include "../fwk/DeviceFactory.h"
#include "../fwk/IpAddressMaster.h"
#include "../utils/Logger.h"
#include "../HTTP_view/emit/MasterEmitter.h"
#include "../HTTP_view/listen/MasterListener.h"

class MasterRadio: public Device {
public:
    MasterRadio(const DeviceIdPtr& deviceId);
    ~MasterRadio();
    void createRadio(const DeviceFactoryPtr& deviceFactory);

    void addtoExternalRadioToPhonesMap(const DeviceIdPtr& , const std::vector<DeviceIdPtr>&);
    std::optional<DeviceIdPtr> getRadioForPhoneNr(const std::string&);
private:

    MasterListenerPtr listener_;
    MasterEmitterPtr emitter_;
    utils::Logger logger_;
    IpAddressMasterPtr ipAddressMaster_;
    std::thread listenerThread_;

    std::mutex idMapMutex_;
    std::mutex externalRadioToPhoneMapMutex_;
};

using MasterRadioPtr = std::shared_ptr<MasterRadio>;



#endif //TELESIM_MASTERRADIO_H
