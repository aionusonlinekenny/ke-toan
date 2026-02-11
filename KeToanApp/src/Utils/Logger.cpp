#include "Logger.h"
#include <ctime>
#include <cstdarg>
#include <iostream>
#include <iomanip>

namespace KeToanApp {

    std::ofstream Logger::logFile_;
    std::mutex Logger::mutex_;
    LogLevel Logger::currentLevel_ = LogLevel::Debug;
    bool Logger::consoleOutput_ = false;
    bool Logger::initialized_ = false;

    void Logger::Initialize(const std::string& filename) {
        std::lock_guard<std::mutex> lock(mutex_);

        if (initialized_) {
            return;
        }

        logFile_.open(filename, std::ios::out | std::ios::app);
        if (!logFile_.is_open()) {
            std::cerr << "Failed to open log file: " << filename << std::endl;
            return;
        }

        initialized_ = true;
        Info("Logger initialized");
    }

    void Logger::Shutdown() {
        std::lock_guard<std::mutex> lock(mutex_);

        if (!initialized_) {
            return;
        }

        Info("Logger shutting down");

        if (logFile_.is_open()) {
            logFile_.close();
        }

        initialized_ = false;
    }

    void Logger::Debug(const char* format, ...) {
        va_list args;
        va_start(args, format);
        Log(LogLevel::Debug, format, args);
        va_end(args);
    }

    void Logger::Info(const char* format, ...) {
        va_list args;
        va_start(args, format);
        Log(LogLevel::Info, format, args);
        va_end(args);
    }

    void Logger::Warning(const char* format, ...) {
        va_list args;
        va_start(args, format);
        Log(LogLevel::Warning, format, args);
        va_end(args);
    }

    void Logger::Error(const char* format, ...) {
        va_list args;
        va_start(args, format);
        Log(LogLevel::Error, format, args);
        va_end(args);
    }

    void Logger::SetLogLevel(LogLevel level) {
        currentLevel_ = level;
    }

    void Logger::SetConsoleOutput(bool enable) {
        consoleOutput_ = enable;
    }

    void Logger::Log(LogLevel level, const char* format, va_list args) {
        if (level < currentLevel_) {
            return;
        }

        std::lock_guard<std::mutex> lock(mutex_);

        // Format message
        char buffer[4096];
        vsnprintf(buffer, sizeof(buffer), format, args);

        // Create log entry
        std::string timestamp = GetTimestamp();
        const char* levelStr = GetLevelString(level);

        std::string logEntry = timestamp + " [" + levelStr + "] " + buffer + "\n";

        // Write to file
        if (initialized_ && logFile_.is_open()) {
            logFile_ << logEntry;
            logFile_.flush();
        }

        // Write to console
        if (consoleOutput_) {
            if (level == LogLevel::Error) {
                std::cerr << logEntry;
            } else {
                std::cout << logEntry;
            }
        }
    }

    const char* Logger::GetLevelString(LogLevel level) {
        switch (level) {
            case LogLevel::Debug:   return "DEBUG";
            case LogLevel::Info:    return "INFO ";
            case LogLevel::Warning: return "WARN ";
            case LogLevel::Error:   return "ERROR";
            default:                return "UNKNOWN";
        }
    }

    std::string Logger::GetTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;

        std::tm tm;
        localtime_s(&tm, &time);

        char buffer[32];
        sprintf_s(buffer, "%04d-%02d-%02d %02d:%02d:%02d.%03lld",
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
            tm.tm_hour, tm.tm_min, tm.tm_sec,
            ms.count());

        return buffer;
    }

} // namespace KeToanApp
