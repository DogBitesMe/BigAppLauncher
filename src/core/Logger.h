#pragma once

#include <string>
#include <fstream>

enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error
};

class Logger {
public:
    static Logger& Instance();

    void SetLogFile(const std::string& path);
    void SetMinLevel(LogLevel level) { m_minLevel = level; }

    void Log(LogLevel level, const char* format, ...);

    void Debug(const char* format, ...);
    void Info(const char* format, ...);
    void Warning(const char* format, ...);
    void Error(const char* format, ...);

private:
    Logger() = default;
    ~Logger();

    void WriteLog(LogLevel level, const char* message);
    const char* GetLevelString(LogLevel level);

    std::ofstream m_file;
    LogLevel m_minLevel = LogLevel::Info;
};

// Convenience macros
#define LOG_DEBUG(...) Logger::Instance().Debug(__VA_ARGS__)
#define LOG_INFO(...) Logger::Instance().Info(__VA_ARGS__)
#define LOG_WARNING(...) Logger::Instance().Warning(__VA_ARGS__)
#define LOG_ERROR(...) Logger::Instance().Error(__VA_ARGS__)
