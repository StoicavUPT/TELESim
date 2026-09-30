//
// Created by vlads on 8/30/2026.
//

#ifndef TELESIM_LOGGER_H
#define TELESIM_LOGGER_H
#include <filesystem>
#include <iostream>
#include <ostream>
#include <fstream>
#include <mutex>
#include <sstream>
#include <string>

namespace utils {

    class Logger;

    class LogEntry{
    public:
        LogEntry(Logger& logger, const std::string& level):
        logger_(logger),
        level_(level)
        {
        }

        ~LogEntry();

        template<typename T>
        LogEntry& operator<<(const T& value){
            message_ << value;
            return *this;
        }

    private:
        Logger& logger_;
        std::string level_;
        std::ostringstream message_;
    };

    class Logger{
    public:
        Logger(const std::string& id):
        id_(id)
        {
            //std::filesystem::remove_all(std::filesystem::path(PROJECT_PATH)/ "logs");
            //std::filesystem::create_directories(std::filesystem::path(PROJECT_PATH)/ "logs");
            createPath();

        }

        LogEntry operator<<(const std::string& level);

    private:
        void createPath();
        void writeLog(const std::string& level, const std::string& message);

        std::filesystem::path absoluteLogPath_;
        const std::string id_;
        static std::mutex writeMutex_;

        friend class LogEntry;
    };
}

namespace logs{
    const std::string warning = std::string(" [WARNING] ");
    const std::string info = std::string(" [INFO] ");
    const std::string debug = std::string(" [DEBUG] ");
    const std::string error = std::string(" [ERROR] ");
    const std::string endl = "\n";
}

#endif //TELESIM_LOGGER_H