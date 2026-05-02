#pragma once

// ============================================================
//  DDoS Detection Simulation - Detector Interface
//  Demonstrates: Pure Virtual Interface (Strategy Pattern)
// ============================================================

#include "core/Enums.h"
#include "core/TrafficWindow.h"
#include <string>

namespace ddos {

// ─── Abstract Detector Interface ───
// This demonstrates the Strategy Pattern:
// Different detection algorithms can be swapped at runtime
class IDetector {
public:
    virtual ~IDetector() = default;

    // ─── Pure virtual methods ───
    virtual DetectionResult analyze(const TrafficWindow& window) = 0;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual void reset() = 0;

    // ─── Common interface ───
    bool isEnabled() const { return m_enabled; }
    void setEnabled(bool enabled) { m_enabled = enabled; }

protected:
    bool m_enabled = true;
};

} // namespace ddos
