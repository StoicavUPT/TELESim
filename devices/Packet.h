//
// Created by vlads on 9/27/2026.
//

#ifndef TELESIM_PACKET_H
#define TELESIM_PACKET_H
#include <memory>
#include <string>


struct Packet {
    std::string callId_;
    std::string message_;
    int receiverRadioPort_;
};

using PacketPtr = std::shared_ptr<Packet>;

#endif //TELESIM_PACKET_H

