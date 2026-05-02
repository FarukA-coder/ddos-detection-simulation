// ============================================================
//  DDoS Detection Simulation - Threshold Detector Implementation
// ============================================================

#include "detection/ThresholdDetector.h"
#include <sstream>
#include <algorithm>

namespace ddos {

// ─── Constructor ───
ThresholdDetector::ThresholdDetector(double packetRateThreshold,
                                     int synThreshold,
                                     int connectionThreshold)
    : m_packetRateThreshold(packetRateThreshold)
    , m_synThreshold(synThreshold)
    , m_connectionThreshold(connectionThreshold)
    , m_consecutiveAlerts(0)
{
}

// ─── Analyze traffic window ───
DetectionResult ThresholdDetector::analyze(const TrafficWindow& window) {
    DetectionResult result;
    result.detectorName = getName();

    if (!m_enabled || window.isEmpty()) {
        return result;
    }

    double packetsPerSec = window.getPacketsPerSecond();
    auto synPackets = window.getPacketsByType(PacketType::SYN);
    int uniqueIPs = window.getUniqueSourceIPCount();

    bool rateExceeded = packetsPerSec > m_packetRateThreshold;
    bool synExceeded = static_cast<int>(synPackets.size()) > m_synThreshold;
    bool connectionExceeded = uniqueIPs > m_connectionThreshold;

    // Calculate confidence based on how many thresholds are exceeded
    int violations = 0;
    if (rateExceeded) violations++;
    if (synExceeded) violations++;
    if (connectionExceeded) violations++;

    if (violations > 0) {
        result.detected = true;
        m_consecutiveAlerts++;

        // Confidence increases with more violations and consecutive alerts
        result.confidence = std::min(1.0,
            (violations / 3.0) * 0.7 + std::min(m_consecutiveAlerts * 0.05, 0.3));

        // Set threat level based on confidence
        if (result.confidence > 0.8)
            result.threatLevel = ThreatLevel::CRITICAL;
        else if (result.confidence > 0.6)
            result.threatLevel = ThreatLevel::HIGH;
        else if (result.confidence > 0.3)
            result.threatLevel = ThreatLevel::MEDIUM;
        else
            result.threatLevel = ThreatLevel::LOW;

        // Build description
        std::ostringstream desc;
        desc << "Threshold violations detected: ";
        if (rateExceeded)
            desc << "[Rate: " << static_cast<int>(packetsPerSec)
                 << " > " << static_cast<int>(m_packetRateThreshold) << " pkt/s] ";
        if (synExceeded)
            desc << "[SYN: " << synPackets.size()
                 << " > " << m_synThreshold << "] ";
        if (connectionExceeded)
            desc << "[Connections: " << uniqueIPs
                 << " > " << m_connectionThreshold << "] ";
        result.description = desc.str();

        // Identify suspicious IPs — always include top sources when attack detected
        auto topIPs = window.getTopSourceIPs(10);
        for (const auto& [ip, count] : topIPs) {
            result.suspiciousIPs.push_back(ip);
        }
    } else {
        m_consecutiveAlerts = 0;
    }

    return result;
}

// ─── Reset state ───
void ThresholdDetector::reset() {
    m_consecutiveAlerts = 0;
}

// ─── Configuration ───
void ThresholdDetector::setPacketRateThreshold(double threshold) {
    m_packetRateThreshold = threshold;
}
void ThresholdDetector::setSynThreshold(int threshold) {
    m_synThreshold = threshold;
}
void ThresholdDetector::setConnectionThreshold(int threshold) {
    m_connectionThreshold = threshold;
}

double ThresholdDetector::getPacketRateThreshold() const { return m_packetRateThreshold; }
int ThresholdDetector::getSynThreshold() const { return m_synThreshold; }
int ThresholdDetector::getConnectionThreshold() const { return m_connectionThreshold; }

} // namespace ddos
