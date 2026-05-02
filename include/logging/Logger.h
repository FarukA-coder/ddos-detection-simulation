#pragma once

// ============================================================
//  DDoS Detection Simulation - Logger
//  Singleton pattern for centralized logging
//  Demonstrates: Singleton Design Pattern, Templates
// ============================================================

#include "core/Enums.h"
#include <string>
#include <vector>
#include <fstream>
#include <mutex>
#include <chrono>
#include <sstream>
#include <iomanip>

namespace ddos {

// ─── Log Entry ───
struct LogEntry {
    std::string timestamp;
    LogLevel level;
    std::string source;
    std::string message;

    std::string toString() const {
        return "[" + timestamp + "] [" + logLevelToString(level) + "] "
               + source + ": " + message;
    }
};

// ─── Logger (Singleton) ───
class Logger {
public:
    // ─── Singleton access ───
    static Logger& getInstance();

    // Delete copy/move (Singleton)
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;

    // ─── Logging methods ───
    void log(const std::string& message, LogLevel level = LogLevel::INFO,
             const std::string& source = "System");

    void debug(const std::string& msg, const std::string& src = "System");
    void info(const std::string& msg, const std::string& src = "System");
    void warning(const std::string& msg, const std::string& src = "System");
    void alert(const std::string& msg, const std::string& src = "System");
    void critical(const std::string& msg, const std::string& src = "System");

    // ─── Log retrieval ───
    std::vector<LogEntry> getRecentLogs(int count = 20) const;
    std::vector<LogEntry> getLogsByLevel(LogLevel level) const;
    int getTotalLogCount() const;

    // ─── Configuration ───
    void setMinLevel(LogLevel level);
    void setFileLogging(bool enabled, const std::string& filename = "ddos_simulation.log");
    void setMaxEntries(int max);

    // ─── Report generation ───
    void exportReport(const std::string& filename) const;

    // ─── Clear ───
    void clear();

    // ─── Template method for logging any printable type ───
    template<typename T>
    void logValue(const std::string& name, const T& value,
                  LogLevel level = LogLevel::INFO,
                  const std::string& source = "System") {
        std::ostringstream oss;
        oss << name << " = " << value;
        log(oss.str(), level, source);
    }

private:
    Logger();
    ~Logger();

    std::string getCurrentTimestamp() const;

    std::vector<LogEntry> m_entries;
    LogLevel m_minLevel;
    bool m_fileLoggingEnabled;
    std::ofstream m_logFile;
    int m_maxEntries;
    mutable std::mutex m_mutex;
};

} // namespace ddos
