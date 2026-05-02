#pragma once

// ============================================================
//  DDoS Detection Simulation - Threshold-Based Detector
//  Detects attacks by monitoring traffic volume thresholds
// ============================================================

#include "IDetector.h"

namespace ddos {

class ThresholdDetector : public IDetector {
public:
    // ─── Constructor ───
    ThresholdDetector(double packetRateThreshold = 100.0,
                      int synThreshold = 50,
                      int connectionThreshold = 500);

    // ─── Override interface ───
    DetectionResult analyze(const TrafficWindow& window) override;
    std::string getName() const override { return "Threshold Detector"; }
    std::string getDescription() const override {
        return "Detects attacks by monitoring packet rate and SYN packet thresholds";
    }
    void reset() override;

    // ─── Configuration ───
    void setPacketRateThreshold(double threshold);
    void setSynThreshold(int threshold);
    void setConnectionThreshold(int threshold);

    double getPacketRateThreshold() const;
    int getSynThreshold() const;
    int getConnectionThreshold() const;

private:
    double m_packetRateThreshold;   // Max packets per second
    int m_synThreshold;             // Max SYN packets in window
    int m_connectionThreshold;      // Max unique connections
    int m_consecutiveAlerts;        // Track consecutive detections
};

} // namespace ddos
