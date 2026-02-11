#pragma once

#include "KeToanApp/Common.h"
#include <fstream>
#include <mutex>

namespace KeToanApp {

    enum class LogLevel {
        Debug,
        Info,
        Warning,
        Error
    };

    class Logger {
    public:
        static void Initialize(const std::string& filename);
        static void Shutdown();

        static void Debug(const char* format, ...);
        static void Info(const char* format, ...);
        static void Warning(const char* format, ...);
        static void Error(const char* format, ...);

        static void SetLogLevel(LogLevel level);
        static void SetConsoleOutput(bool enable);

    private:
        static std::ofstream logFile_;
        static std::mutex mutex_;
        static LogLevel currentLevel_;
        static bool consoleOutput_;
        static bool initialized_;

        static void Log(LogLevel level, const char* format, va_list args);
        static const char* GetLevelString(LogLevel level);
        static std::string GetTimestamp();
    };

} // namespace KeToanApp
