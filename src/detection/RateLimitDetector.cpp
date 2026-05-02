// ============================================================
//  DDoS Detection Simulation - Rate Limit Detector Implementation
// ============================================================

#include "detection/RateLimitDetector.h"
#include <sstream>
#include <algorithm>

namespace ddos {

// ─── Constructor ───
RateLimitDetector::RateLimitDetector(int maxRequestsPerIP,
                                     int maxNewConnectionsPerSec)
    : m_maxRequestsPerIP(maxRequestsPerIP)
    , m_maxNewConnectionsPerSec(maxNewConnectionsPerSec)
    , m_violatingIPCount(0)
{
}

// ─── Analyze traffic window ───
DetectionResult RateLimitDetector::analyze(const TrafficWindow& window) {
    DetectionResult result;
    result.detectorName = getName();

    if (!m_enabled || window.isEmpty()) {
        return result;
    }

    // Get per-IP packet counts
    m_ipRateMap = window.getIPFrequencyMap();

    // Find IPs exceeding the rate limit
    std::vector<std::pair<std::string, int>> violators;
    for (const auto& [ip, count] : m_ipRateMap) {
        if (count > m_maxRequestsPerIP) {
            violators.push_back({ip, count});
        }
    }

    m_violatingIPCount = static_cast<int>(violators.size());

    // Sort violators by count (descending)
    std::sort(violators.begin(), violators.end(),
        [](const auto& a, const auto& b) { return a.second > b.second; });

    if (!violators.empty()) {
        result.detected = true;

        // Confidence based on number and severity of violations
        double violationRatio = static_cast<double>(violators.size()) /
                                std::max(static_cast<int>(m_ipRateMap.size()), 1);
        double maxExcess = 0.0;
        for (const auto& [ip, count] : violators) {
            double excess = static_cast<double>(count) / m_maxRequestsPerIP;
            maxExcess = std::max(maxExcess, excess);
        }

        result.confidence = std::min(1.0,
            violationRatio * 0.3 + std::min(maxExcess / 10.0, 0.7));

        if (result.confidence > 0.7)
            result.threatLevel = ThreatLevel::HIGH;
        else if (result.confidence > 0.4)
            result.threatLevel = ThreatLevel::MEDIUM;
        else
            result.threatLevel = ThreatLevel::LOW;

        // Build description
        std::ostringstream desc;
        desc << violators.size() << " IP(s) exceeding rate limit ("
             << m_maxRequestsPerIP << " req/window): ";

        int shown = 0;
        for (const auto& [ip, count] : violators) {
            if (shown >= 3) {
                desc << "... and " << (violators.size() - 3) << " more";
                break;
            }
            if (shown > 0) desc << ", ";
            desc << ip << " (" << count << " req)";
            result.suspiciousIPs.push_back(ip);
            shown++;
        }
        result.description = desc.str();
    }

    return result;
}

// ─── Reset state ───
void RateLimitDetector::reset() {
    m_ipRateMap.clear();
    m_violatingIPCount = 0;
}

// ─── Configuration ───
void RateLimitDetector::setMaxRequestsPerIP(int limit) {
    m_maxRequestsPerIP = limit;
}
int RateLimitDetector::getMaxRequestsPerIP() const { return m_maxRequestsPerIP; }
void RateLimitDetector::setMaxNewConnectionsPerSec(int limit) {
    m_maxNewConnectionsPerSec = limit;
}
int RateLimitDetector::getMaxNewConnectionsPerSec() const {
    return m_maxNewConnectionsPerSec;
}

// ─── Stats ───
int RateLimitDetector::getViolatingIPCount() const { return m_violatingIPCount; }
const std::map<std::string, int>& RateLimitDetector::getIPRateMap() const {
    return m_ipRateMap;
}

} // namespace ddos
