//
// Created by vlads on 8/30/2026.
//

#include "Logger.h"
#include <fstream>




namespace utils {

    std::mutex Logger::writeMutex_{};

    void Logger::createPath() {
        absoluteLogPath_ = std::filesystem::path(PROJECT_PATH)/ "logs"/"LOGS.log";
    }

    LogEntry Logger::operator<<(const std::string& level) {
        return LogEntry(*this, level);
    }

    void Logger::writeLog(const std::string& level, const std::string& message) {

        std::ofstream file;
        file.open(absoluteLogPath_, std::ios::app);

        if (!file.is_open()) {
            return;
        }
        const auto now = std::chrono::system_clock::now();

        const auto time = std::chrono::system_clock::to_time_t(now);

        std::tm tm{};
        localtime_s(&tm, &time);

        const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()
        ) % 1000;


        file << "["
            << std::put_time(&tm, "%H:%M:%S")
            << "." << std::setfill('0') << std::setw(3) << ms.count()
            << "] "
            <<"[" << id_ << "]: " << level << message << "\n";

        file.close();
    }

    LogEntry::~LogEntry() {
        logger_.writeLog(level_, message_.str());
    }
}