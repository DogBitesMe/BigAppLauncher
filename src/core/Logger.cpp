#include "Logger.h"
#include <cstdarg>
#include <ctime>
#include <Windows.h>

Logger& Logger::Instance() {
    static Logger instance;
    return instance;
}

Logger::~Logger() {
    if (m_file.is_open()) {
        m_file.close();
    }
}

void Logger::SetLogFile(const std::string& path) {
    if (m_file.is_open()) {
        m_file.close();
    }
    m_file.open(path, std::ios::out | std::ios::app);
}

void Logger::Log(LogLevel level, const char* format, ...) {
    if (level < m_minLevel) {
        return;
    }

    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    WriteLog(level, buffer);
}

void Logger::Debug(const char* format, ...) {
    if (LogLevel::Debug < m_minLevel) {
        return;
    }

    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    WriteLog(LogLevel::Debug, buffer);
}

void Logger::Info(const char* format, ...) {
    if (LogLevel::Info < m_minLevel) {
        return;
    }

    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    WriteLog(LogLevel::Info, buffer);
}

void Logger::Warning(const char* format, ...) {
    if (LogLevel::Warning < m_minLevel) {
        return;
    }

    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    WriteLog(LogLevel::Warning, buffer);
}

void Logger::Error(const char* format, ...) {
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    WriteLog(LogLevel::Error, buffer);
}

void Logger::WriteLog(LogLevel level, const char* message) {
    // Get current time
    time_t now = time(nullptr);
    struct tm timeinfo;
    localtime_s(&timeinfo, &now);

    char timeStr[32];
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", &timeinfo);

    // Format log entry
    char logEntry[1200];
    snprintf(logEntry, sizeof(logEntry), "[%s] [%s] %s\n",
             timeStr, GetLevelString(level), message);

    // Write to file
    if (m_file.is_open()) {
        m_file << logEntry;
        m_file.flush();
    }

    // Also output to debug console in debug builds
#ifdef _DEBUG
    OutputDebugStringA(logEntry);
#endif
}

const char* Logger::GetLevelString(LogLevel level) {
    switch (level) {
        case LogLevel::Debug:   return "DEBUG";
        case LogLevel::Info:    return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error:   return "ERROR";
        default:                return "UNKNOWN";
    }
}
