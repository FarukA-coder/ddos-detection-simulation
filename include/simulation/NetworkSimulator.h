#pragma once

// ============================================================
//  DDoS Detection Simulation - Network Simulator
//  Main simulation engine coordinating all components
//  Demonstrates: Composition, Observer-like pattern
// ============================================================

#include "core/Enums.h"
#include "core/TrafficWindow.h"
#include "network/NetworkNode.h"
#include "network/ClientNode.h"
#include "network/AttackerNode.h"
#include "network/ServerNode.h"
#include "detection/IDetector.h"
#include "detection/ThresholdDetector.h"
#include "detection/EntropyDetector.h"
#include "detection/RateLimitDetector.h"
#include "security/Firewall.h"
#include "logging/Logger.h"

#include <vector>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <functional>

namespace ddos {

class NetworkSimulator {
public:
    // ─── Constructor / Destructor ───
    NetworkSimulator();
    ~NetworkSimulator();

    // No copy (has threads)
    NetworkSimulator(const NetworkSimulator&) = delete;
    NetworkSimulator& operator=(const NetworkSimulator&) = delete;

    // ─── Simulation Control ───
    void start();
    void stop();
    void pause();
    void resume();
    bool isRunning() const;
    SimulationState getState() const;

    // ─── Node Management ───
    void addClient(const std::string& name, const std::string& ip, double rate = 1.0);
    void addAttacker(const std::string& name, const std::string& ip,
                     AttackType type = AttackType::SYN_FLOOD, int intensity = 50);
    void removeAllClients();
    void removeAllAttackers();

    // ─── Attack Control ───
    void startAttack(AttackType type = AttackType::SYN_FLOOD, int intensity = 50);
    void stopAttack();
    void setAttackType(AttackType type);
    void setAttackIntensity(int intensity);
    bool isUnderAttack() const;
    AttackType getCurrentAttackType() const;

    // ─── Detector Management ───
    void setActiveDetector(int index);
    int getActiveDetectorIndex() const;
    std::vector<std::string> getDetectorNames() const;
    IDetector* getActiveDetector();
    const DetectionResult& getLastDetectionResult() const;

    // ─── Access components ───
    const ServerNode& getServer() const;
    const Firewall& getFirewall() const;
    Firewall& getFirewall();
    const TrafficWindow& getTrafficWindow() const;
    SimulationStats getStats() const;
    double getSimulationTime() const;

    // ─── Configuration ───
    void setTickRate(int ticksPerSecond);
    void setAutoMitigate(bool enabled);
    bool isAutoMitigateEnabled() const;

    // ─── Report ───
    void generateReport(const std::string& filename = "security_report.txt");

private:
    void simulationLoop();
    void tick();
    void updateStats();

    // ─── Components (Composition) ───
    std::unique_ptr<ServerNode> m_server;
    std::vector<std::unique_ptr<ClientNode>> m_clients;
    std::vector<std::unique_ptr<AttackerNode>> m_attackers;
    std::vector<std::unique_ptr<IDetector>> m_detectors;
    int m_activeDetectorIndex;

    Firewall m_firewall;
    TrafficWindow m_trafficWindow;
    SimulationStats m_stats;
    DetectionResult m_lastDetectionResult;

    // ─── Simulation state ───
    std::atomic<SimulationState> m_state;
    std::atomic<bool> m_shouldRun;
    std::thread m_simThread;
    mutable std::recursive_mutex m_mutex;

    // ─── Timing ───
    int m_tickRate;
    double m_simulationTime;
    bool m_autoMitigate;

    // ─── Attack state ───
    std::atomic<bool> m_underAttack;
    AttackType m_currentAttackType;
};

} // namespace ddos
