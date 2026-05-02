#pragma once

// ============================================================
//  DDoS Detection Simulation - Rate Limit Detector
//  Monitors per-IP request rates to detect abuse
// ============================================================

#include "IDetector.h"
#include <map>

namespace ddos {

class RateLimitDetector : public IDetector {
public:
    // ─── Constructor ───
    explicit RateLimitDetector(int maxRequestsPerIP = 30,
                                int maxNewConnectionsPerSec = 50);

    // ─── Override interface ───
    DetectionResult analyze(const TrafficWindow& window) override;
    std::string getName() const override { return "Rate Limit Detector"; }
    std::string getDescription() const override {
        return "Monitors per-IP request rates and flags IPs exceeding the limit";
    }
    void reset() override;

    // ─── Configuration ───
    void setMaxRequestsPerIP(int limit);
    int getMaxRequestsPerIP() const;
    void setMaxNewConnectionsPerSec(int limit);
    int getMaxNewConnectionsPerSec() const;

    // ─── Stats ───
    int getViolatingIPCount() const;
    const std::map<std::string, int>& getIPRateMap() const;

private:
    int m_maxRequestsPerIP;
    int m_maxNewConnectionsPerSec;
    std::map<std::string, int> m_ipRateMap;
    int m_violatingIPCount;
};

} // namespace ddos
