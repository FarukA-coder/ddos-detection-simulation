#pragma once

// ============================================================
//  DDoS Detection Simulation - Entropy-Based Detector
//  Uses Shannon entropy to detect traffic anomalies
//  Low entropy = few sources generating lots of traffic = DDoS
// ============================================================

#include "IDetector.h"

namespace ddos {

class EntropyDetector : public IDetector {
public:
    // ─── Constructor ───
    explicit EntropyDetector(double entropyThreshold = 2.0);

    // ─── Override interface ───
    DetectionResult analyze(const TrafficWindow& window) override;
    std::string getName() const override { return "Entropy Detector"; }
    std::string getDescription() const override {
        return "Uses Shannon entropy of source IPs to detect spoofed/concentrated traffic";
    }
    void reset() override;

    // ─── Configuration ───
    void setEntropyThreshold(double threshold);
    double getEntropyThreshold() const;
    double getLastEntropy() const;

    // ─── Entropy calculation (public for educational purposes) ───
    static double calculateEntropy(const std::map<std::string, int>& frequencyMap,
                                    int totalPackets);

private:
    double m_entropyThreshold;  // Below this = suspicious
    double m_lastEntropy;
    double m_baselineEntropy;
    int m_sampleCount;
};

} // namespace ddos
