#include <iostream>
#include "../devices/info/DeviceId.h"
#include "UtilsFunctions.h"

#include <algorithm>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <random>

namespace utils {

    std::string createMac() {
        static constexpr char letters[] = "ABCDEF1234567890";
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<int> dist(0, 15);

        std::string mac;
        for (int i=1 ; i<18 ;i++) {
            char x = (i%3 == 0) ? ':' : letters[dist(gen)];
            mac.push_back(x);
        }
        return mac;
    }

    std::string createPhoneNumber() {
        static constexpr char digits[] = "0123456789";
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<int> dist(0, 9);

        char phoneNr[10] = "07";
        for (int i=2; i<10; i++) {
            char x = digits[dist(gen)];
            phoneNr[i]=x;
        }
        return phoneNr;
    }

    std::vector<std::string> parseIpAddress(const std::string& ipAddress) {
        std::vector<std::string> parsedIp{};
        std::string byte{};
        for (const auto& digit: ipAddress) {
            if (digit == '.') {
                parsedIp.push_back(byte);
                byte = "";
            }
            else {
                byte.push_back(digit);
            }
        }
        parsedIp.push_back(byte);
        return parsedIp;
    }
}
