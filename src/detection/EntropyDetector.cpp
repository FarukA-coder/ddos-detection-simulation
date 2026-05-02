// ============================================================
//  DDoS Detection Simulation - Entropy Detector Implementation
//  Shannon Entropy: H = -Σ(p_i * log2(p_i))
//  High entropy = diverse traffic (normal)
//  Low entropy = concentrated traffic (attack)
// ============================================================

#include "detection/EntropyDetector.h"
#include <cmath>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace ddos {

// ─── Constructor ───
EntropyDetector::EntropyDetector(double entropyThreshold)
    : m_entropyThreshold(entropyThreshold)
    , m_lastEntropy(0.0)
    , m_baselineEntropy(0.0)
    , m_sampleCount(0)
{
}

// ─── Calculate Shannon Entropy ───
double EntropyDetector::calculateEntropy(
    const std::map<std::string, int>& frequencyMap, int totalPackets) {

    if (totalPackets <= 0 || frequencyMap.empty()) return 0.0;

    double entropy = 0.0;
    double total = static_cast<double>(totalPackets);

    for (const auto& [ip, count] : frequencyMap) {
        double probability = static_cast<double>(count) / total;
        if (probability > 0.0) {
            entropy -= probability * std::log2(probability);
        }
    }

    return entropy;
}

// ─── Analyze traffic window ───
DetectionResult EntropyDetector::analyze(const TrafficWindow& window) {
    DetectionResult result;
    result.detectorName = getName();

    if (!m_enabled || window.getPacketCount() < 10) {
        return result;
    }

    auto freqMap = window.getIPFrequencyMap();
    int totalPackets = static_cast<int>(window.getPacketCount());

    m_lastEntropy = calculateEntropy(freqMap, totalPackets);

    // Update baseline entropy (exponential moving average)
    m_sampleCount++;
    if (m_sampleCount <= 10) {
        m_baselineEntropy = (m_baselineEntropy * (m_sampleCount - 1) + m_lastEntropy)
                           / m_sampleCount;
    } else {
        m_baselineEntropy = m_baselineEntropy * 0.95 + m_lastEntropy * 0.05;
    }

    // Check if entropy drops significantly below threshold or baseline
    double effectiveThreshold = std::min(m_entropyThreshold,
                                         m_baselineEntropy * 0.5);

    if (m_lastEntropy < effectiveThreshold && m_sampleCount > 5) {
        result.detected = true;

        // Confidence based on how far entropy dropped
        double drop = 1.0 - (m_lastEntropy / std::max(m_baselineEntropy, 0.1));
        result.confidence = std::min(1.0, std::max(0.0, drop));

        if (result.confidence > 0.7)
            result.threatLevel = ThreatLevel::CRITICAL;
        else if (result.confidence > 0.5)
            result.threatLevel = ThreatLevel::HIGH;
        else if (result.confidence > 0.3)
            result.threatLevel = ThreatLevel::MEDIUM;
        else
            result.threatLevel = ThreatLevel::LOW;

        std::ostringstream desc;
        desc << "Low entropy detected! H=" << std::fixed
             << std::setprecision(2) << m_lastEntropy
             << " (baseline: " << m_baselineEntropy
             << ", threshold: " << effectiveThreshold
             << ") — Traffic is concentrated from few sources";
        result.description = desc.str();

        // Find IPs contributing to low entropy (highest frequency)
        std::vector<std::pair<std::string, int>> sorted(freqMap.begin(), freqMap.end());
        std::sort(sorted.begin(), sorted.end(),
            [](const auto& a, const auto& b) { return a.second > b.second; });

        // IPs with more than 20% of total traffic are suspicious
        for (const auto& [ip, count] : sorted) {
            if (count > totalPackets / 5) {
                result.suspiciousIPs.push_back(ip);
            }
        }
    }

    return result;
}

// ─── Reset state ───
void EntropyDetector::reset() {
    m_lastEntropy = 0.0;
    m_baselineEntropy = 0.0;
    m_sampleCount = 0;
}

// ─── Configuration ───
void EntropyDetector::setEntropyThreshold(double threshold) {
    m_entropyThreshold = threshold;
}
double EntropyDetector::getEntropyThreshold() const { return m_entropyThreshold; }
double EntropyDetector::getLastEntropy() const { return m_lastEntropy; }

} // namespace ddos
