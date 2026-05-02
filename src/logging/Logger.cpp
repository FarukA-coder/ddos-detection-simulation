// ============================================================
//  DDoS Detection Simulation - Logger Implementation
// ============================================================

#include "logging/Logger.h"
#include <iostream>
#include <algorithm>

namespace ddos {

// ─── Singleton instance ───
Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

// ─── Constructor ───
Logger::Logger()
    : m_minLevel(LogLevel::INFO)
    , m_fileLoggingEnabled(false)
    , m_maxEntries(1000)
{
}

// ─── Destructor ───
Logger::~Logger() {
    if (m_logFile.is_open()) {
        m_logFile.close();
    }
}

// ─── Get current timestamp ───
std::string Logger::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::tm tm_buf;
    localtime_s(&tm_buf, &time_t);

    std::ostringstream oss;
    oss << std::put_time(&tm_buf, "%H:%M:%S")
        << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return oss.str();
}

// ─── Main log method ───
void Logger::log(const std::string& message, LogLevel level,
                 const std::string& source) {
    if (level < m_minLevel) return;

    std::lock_guard<std::mutex> lock(m_mutex);

    LogEntry entry;
    entry.timestamp = getCurrentTimestamp();
    entry.level = level;
    entry.source = source;
    entry.message = message;

    m_entries.push_back(entry);

    // Trim old entries
    while (static_cast<int>(m_entries.size()) > m_maxEntries) {
        m_entries.erase(m_entries.begin());
    }

    // Write to file if enabled
    if (m_fileLoggingEnabled && m_logFile.is_open()) {
        m_logFile << entry.toString() << std::endl;
        m_logFile.flush();
    }
}

// ─── Convenience methods ───
void Logger::debug(const std::string& msg, const std::string& src) {
    log(msg, LogLevel::DEBUG, src);
}
void Logger::info(const std::string& msg, const std::string& src) {
    log(msg, LogLevel::INFO, src);
}
void Logger::warning(const std::string& msg, const std::string& src) {
    log(msg, LogLevel::WARNING, src);
}
void Logger::alert(const std::string& msg, const std::string& src) {
    log(msg, LogLevel::ALERT, src);
}
void Logger::critical(const std::string& msg, const std::string& src) {
    log(msg, LogLevel::CRITICAL, src);
}

// ─── Get recent logs ───
std::vector<LogEntry> Logger::getRecentLogs(int count) const {
    std::lock_guard<std::mutex> lock(m_mutex);

    int start = std::max(0, static_cast<int>(m_entries.size()) - count);
    return std::vector<LogEntry>(
        m_entries.begin() + start,
        m_entries.end()
    );
}

// ─── Get logs by level ───
std::vector<LogEntry> Logger::getLogsByLevel(LogLevel level) const {
    std::lock_guard<std::mutex> lock(m_mutex);

    std::vector<LogEntry> result;
    for (const auto& entry : m_entries) {
        if (entry.level == level) {
            result.push_back(entry);
        }
    }
    return result;
}

// ─── Total log count ───
int Logger::getTotalLogCount() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return static_cast<int>(m_entries.size());
}

// ─── Configuration ───
void Logger::setMinLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_minLevel = level;
}

void Logger::setFileLogging(bool enabled, const std::string& filename) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_fileLoggingEnabled = enabled;
    if (enabled) {
        if (m_logFile.is_open()) m_logFile.close();
        m_logFile.open(filename, std::ios::out | std::ios::app);
    } else {
        if (m_logFile.is_open()) m_logFile.close();
    }
}

void Logger::setMaxEntries(int max) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_maxEntries = max;
}

// ─── Export full report ───
void Logger::exportReport(const std::string& filename) const {
    std::lock_guard<std::mutex> lock(m_mutex);

    std::ofstream report(filename);
    if (!report.is_open()) return;

    report << "═══════════════════════════════════════════════════════════\n";
    report << "  DDoS Detection Simulation - Security Report\n";
    report << "═══════════════════════════════════════════════════════════\n\n";

    // Summary
    int debugCount = 0, infoCount = 0, warnCount = 0, alertCount = 0, critCount = 0;
    for (const auto& entry : m_entries) {
        switch (entry.level) {
            case LogLevel::DEBUG:    debugCount++; break;
            case LogLevel::INFO:     infoCount++; break;
            case LogLevel::WARNING:  warnCount++; break;
            case LogLevel::ALERT:    alertCount++; break;
            case LogLevel::CRITICAL: critCount++; break;
        }
    }

    report << "SUMMARY\n";
    report << "───────────────────────────────────────────────────────────\n";
    report << "Total Entries:  " << m_entries.size() << "\n";
    report << "  DEBUG:        " << debugCount << "\n";
    report << "  INFO:         " << infoCount << "\n";
    report << "  WARNING:      " << warnCount << "\n";
    report << "  ALERT:        " << alertCount << "\n";
    report << "  CRITICAL:     " << critCount << "\n\n";

    report << "FULL LOG\n";
    report << "───────────────────────────────────────────────────────────\n";
    for (const auto& entry : m_entries) {
        report << entry.toString() << "\n";
    }

    report << "\n═══════════════════════════════════════════════════════════\n";
    report << "  End of Report\n";
    report << "═══════════════════════════════════════════════════════════\n";

    report.close();
}

// ─── Clear all entries ───
void Logger::clear() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_entries.clear();
}

} // namespace ddos
