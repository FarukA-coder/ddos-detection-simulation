// ============================================================
//  DDoS Detection Simulation - Network Simulator Implementation
// ============================================================

#include "simulation/NetworkSimulator.h"
#include <algorithm>
#include <chrono>
#include <sstream>

namespace ddos {

// ─── Constructor ───
NetworkSimulator::NetworkSimulator()
    : m_server(std::make_unique<ServerNode>("Main Server", "10.0.0.1", 1000,
                                            10000.0)),
      m_activeDetectorIndex(0), m_trafficWindow(2000, std::chrono::seconds(10)),
      m_state(SimulationState::STOPPED), m_shouldRun(false), m_tickRate(10),
      m_simulationTime(0.0), m_autoMitigate(true), m_underAttack(false),
      m_currentAttackType(AttackType::NONE) {
  // Initialize default detectors (Strategy Pattern)
  m_detectors.push_back(std::make_unique<ThresholdDetector>(100.0, 50, 200));
  m_detectors.push_back(std::make_unique<EntropyDetector>(2.0));
  m_detectors.push_back(std::make_unique<RateLimitDetector>(30, 50));

  // Add some default normal clients
  addClient("User-Alpha", "192.168.1.10", 1.5);
  addClient("User-Beta", "192.168.1.11", 1.0);
  addClient("User-Gamma", "192.168.1.12", 2.0);
  addClient("User-Delta", "192.168.1.13", 0.8);
  addClient("User-Epsilon", "192.168.1.14", 1.2);

  // Whitelist normal clients
  for (const auto &client : m_clients) {
    m_firewall.whitelistIP(client->getIP());
  }

  Logger::getInstance().info("Network Simulator initialized", "Simulator");
  Logger::getInstance().info("Server: " + m_server->toString(), "Simulator");
  Logger::getInstance().info(
      std::to_string(m_clients.size()) + " clients registered", "Simulator");
  Logger::getInstance().info(
      std::to_string(m_detectors.size()) + " detectors loaded", "Simulator");
}

// ─── Destructor ───
NetworkSimulator::~NetworkSimulator() { stop(); }

// ─── Start simulation ───
void NetworkSimulator::start() {
  if (m_state == SimulationState::RUNNING)
    return;

  m_shouldRun = true;
  m_state = SimulationState::RUNNING;

  Logger::getInstance().info("Simulation STARTED", "Simulator");

  m_simThread = std::thread(&NetworkSimulator::simulationLoop, this);
}

// ─── Stop simulation ───
void NetworkSimulator::stop() {
  m_shouldRun = false;
  m_state = SimulationState::STOPPED;

  if (m_simThread.joinable()) {
    m_simThread.join();
  }

  Logger::getInstance().info("Simulation STOPPED", "Simulator");
}

// ─── Pause ───
void NetworkSimulator::pause() {
  m_state = SimulationState::PAUSED;
  Logger::getInstance().info("Simulation PAUSED", "Simulator");
}

// ─── Resume ───
void NetworkSimulator::resume() {
  if (m_state == SimulationState::PAUSED) {
    m_state = SimulationState::RUNNING;
    Logger::getInstance().info("Simulation RESUMED", "Simulator");
  }
}

// ─── Status ───
bool NetworkSimulator::isRunning() const {
  return m_state == SimulationState::RUNNING;
}

SimulationState NetworkSimulator::getState() const { return m_state.load(); }

// ═══════════════════════════════════════════════════════════
//  Node Management
// ═══════════════════════════════════════════════════════════

void NetworkSimulator::addClient(const std::string &name, const std::string &ip,
                                 double rate) {
  std::lock_guard<std::recursive_mutex> lock(m_mutex);
  m_clients.push_back(std::make_unique<ClientNode>(name, ip, rate));
}

void NetworkSimulator::addAttacker(const std::string &name,
                                   const std::string &ip, AttackType type,
                                   int intensity) {
  std::lock_guard<std::recursive_mutex> lock(m_mutex);
  auto attacker = std::make_unique<AttackerNode>(name, ip, type, intensity);
  m_attackers.push_back(std::move(attacker));
}

void NetworkSimulator::removeAllClients() {
  std::lock_guard<std::recursive_mutex> lock(m_mutex);
  m_clients.clear();
}

void NetworkSimulator::removeAllAttackers() {
  std::lock_guard<std::recursive_mutex> lock(m_mutex);
  m_attackers.clear();
  m_underAttack = false;
}

// ═══════════════════════════════════════════════════════════
//  Attack Control
// ═══════════════════════════════════════════════════════════

void NetworkSimulator::startAttack(AttackType type, int intensity) {
  std::lock_guard<std::recursive_mutex> lock(m_mutex);

  m_currentAttackType = type;
  m_underAttack = true;

  // Create attacker nodes if none exist
  if (m_attackers.empty()) {
    addAttacker("Botnet-A", "203.0.113.1", type, intensity);
    addAttacker("Botnet-B", "198.51.100.5", type, intensity);
    addAttacker("Botnet-C", "192.0.2.10", type, intensity);
  }

  // Activate all attackers
  for (auto &attacker : m_attackers) {
    attacker->setAttackType(type);
    attacker->setIntensity(intensity);
    attacker->startAttack();
  }

  Logger::getInstance().alert("ATTACK STARTED: " + attackTypeToString(type) +
                                  " | Intensity: " + std::to_string(intensity) +
                                  "%",
                              "Attack Control");
}

void NetworkSimulator::stopAttack() {
  std::lock_guard<std::recursive_mutex> lock(m_mutex);

  for (auto &attacker : m_attackers) {
    attacker->stopAttack();
  }
  m_underAttack = false;
  m_currentAttackType = AttackType::NONE;

  // Recovery: clear attack data so system can recover
  m_attackers.clear();
  m_trafficWindow.clear();
  m_firewall.clearBlockedIPs();
  m_server->clearQueue();
  m_lastDetectionResult = DetectionResult();

  // Reset detection state
  for (auto &det : m_detectors) {
    det->reset();
  }

  Logger::getInstance().info("Attack STOPPED - System recovering...", "Attack Control");
}

void NetworkSimulator::setAttackType(AttackType type) {
  std::lock_guard<std::recursive_mutex> lock(m_mutex);
  m_currentAttackType = type;
  for (auto &attacker : m_attackers) {
    attacker->setAttackType(type);
  }
}

void NetworkSimulator::setAttackIntensity(int intensity) {
  std::lock_guard<std::recursive_mutex> lock(m_mutex);
  for (auto &attacker : m_attackers) {
    attacker->setIntensity(intensity);
  }
}

bool NetworkSimulator::isUnderAttack() const { return m_underAttack; }
AttackType NetworkSimulator::getCurrentAttackType() const {
  return m_currentAttackType;
}

// ═══════════════════════════════════════════════════════════
//  Detector Management (Strategy Pattern)
// ═══════════════════════════════════════════════════════════

void NetworkSimulator::setActiveDetector(int index) {
  if (index >= 0 && index < static_cast<int>(m_detectors.size())) {
    m_activeDetectorIndex = index;
    Logger::getInstance().info("Active detector changed to: " +
                                   m_detectors[index]->getName(),
                               "Detector");
  }
}

int NetworkSimulator::getActiveDetectorIndex() const {
  return m_activeDetectorIndex;
}

std::vector<std::string> NetworkSimulator::getDetectorNames() const {
  std::vector<std::string> names;
  for (const auto &det : m_detectors) {
    names.push_back(det->getName());
  }
  return names;
}

IDetector *NetworkSimulator::getActiveDetector() {
  if (m_activeDetectorIndex < static_cast<int>(m_detectors.size())) {
    return m_detectors[m_activeDetectorIndex].get();
  }
  return nullptr;
}

const DetectionResult &NetworkSimulator::getLastDetectionResult() const {
  return m_lastDetectionResult;
}

// ═══════════════════════════════════════════════════════════
//  Component Access
// ═══════════════════════════════════════════════════════════

const ServerNode &NetworkSimulator::getServer() const { return *m_server; }
const Firewall &NetworkSimulator::getFirewall() const { return m_firewall; }
Firewall &NetworkSimulator::getFirewall() { return m_firewall; }
const TrafficWindow &NetworkSimulator::getTrafficWindow() const {
  return m_trafficWindow;
}
double NetworkSimulator::getSimulationTime() const { return m_simulationTime; }

SimulationStats NetworkSimulator::getStats() const {
  std::lock_guard<std::recursive_mutex> lock(m_mutex);
  return m_stats;
}

// ─── Configuration ───
void NetworkSimulator::setTickRate(int ticksPerSecond) {
  m_tickRate = std::max(1, std::min(60, ticksPerSecond));
}

void NetworkSimulator::setAutoMitigate(bool enabled) {
  m_autoMitigate = enabled;
  Logger::getInstance().info(std::string("Auto-mitigate ") +
                                 (enabled ? "ENABLED" : "DISABLED"),
                             "Firewall");
}

bool NetworkSimulator::isAutoMitigateEnabled() const { return m_autoMitigate; }

// ═══════════════════════════════════════════════════════════
//  Simulation Loop
// ═══════════════════════════════════════════════════════════

void NetworkSimulator::simulationLoop() {
  auto lastTime = std::chrono::steady_clock::now();
  int tickInterval = 1000 / m_tickRate;

  while (m_shouldRun) {
    auto now = std::chrono::steady_clock::now();
    auto elapsed =
        std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTime);

    if (elapsed.count() >= tickInterval) {
      if (m_state == SimulationState::RUNNING) {
        double deltaTime = elapsed.count() / 1000.0;
        m_simulationTime += deltaTime;
        tick();
      }
      lastTime = now;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
}

// ─── Single simulation tick ───
void NetworkSimulator::tick() {
  std::lock_guard<std::recursive_mutex> lock(m_mutex);
  double deltaTime = 1.0 / m_tickRate;

  // 1. Generate packets from all nodes
  std::vector<Packet> allPackets;

  for (auto &client : m_clients) {
    client->update(deltaTime);
    auto packets = client->generatePackets();
    allPackets.insert(allPackets.end(), packets.begin(), packets.end());
  }

  for (auto &attacker : m_attackers) {
    attacker->update(deltaTime);
    auto packets = attacker->generatePackets();
    allPackets.insert(allPackets.end(), packets.begin(), packets.end());
  }

  // 2. Process packets through firewall and deliver to server
  for (auto &pkt : allPackets) {
    // Add to traffic window for analysis
    m_trafficWindow.addPacket(pkt);
    m_stats.totalPackets++;

    if (pkt.isMalicious()) {
      m_stats.totalAttackPackets++;
    } else {
      m_stats.totalNormalPackets++;
    }

    // Firewall filtering
    if (m_firewall.filterPacket(pkt)) {
      m_server->receivePacket(pkt);
    } else {
      m_stats.totalBlocked++;
    }
  }

  // 3. Update server
  m_server->update(deltaTime);

  // 4. Run detection (every tick)
  if (m_activeDetectorIndex < static_cast<int>(m_detectors.size())) {
    m_lastDetectionResult =
        m_detectors[m_activeDetectorIndex]->analyze(m_trafficWindow);

    if (m_lastDetectionResult.detected) {
      m_stats.alertsTriggered++;

      // Log detection
      Logger::getInstance().log(m_lastDetectionResult.description,
                                m_lastDetectionResult.threatLevel >=
                                        ThreatLevel::HIGH
                                    ? LogLevel::CRITICAL
                                    : LogLevel::ALERT,
                                m_lastDetectionResult.detectorName);

      // Auto-mitigate: block suspicious IPs
      if (m_autoMitigate) {
        for (const auto &ip : m_lastDetectionResult.suspiciousIPs) {
          if (!m_firewall.isWhitelisted(ip) && !m_firewall.isBlocked(ip)) {
            m_firewall.blockIP(ip);
            Logger::getInstance().warning("Auto-blocked IP: " + ip, "Firewall");
          }
        }
      }
    }
  }

  // 5. Prune old traffic data
  m_trafficWindow.pruneOldPackets();

  // 6. Update stats
  updateStats();
}

// ─── Update statistics ───
void NetworkSimulator::updateStats() {
  m_stats.packetsPerSecond = m_trafficWindow.getPacketsPerSecond();
  m_stats.peakPacketsPerSecond =
      std::max(m_stats.peakPacketsPerSecond, m_stats.packetsPerSecond);
  m_stats.serverLoad = m_server->getLoad();
  m_stats.peakServerLoad = std::max(m_stats.peakServerLoad, m_stats.serverLoad);
  m_stats.activeConnections = m_server->getActiveConnections();
  m_stats.normalClients = static_cast<int>(m_clients.size());
  m_stats.attackerNodes = static_cast<int>(m_attackers.size());
  m_stats.blockedIPs = m_firewall.getBlockedIPCount();
  m_stats.uniqueSourceIPs = m_trafficWindow.getUniqueSourceIPCount();
}

// ─── Generate report ───
void NetworkSimulator::generateReport(const std::string &filename) {
  std::lock_guard<std::recursive_mutex> lock(m_mutex);

  std::ofstream report(filename);
  if (!report.is_open())
    return;

  auto stats = m_stats;

  report
      << "================================================================\n";
  report << "     DDoS DETECTION SIMULATION - SECURITY REPORT\n";
  report
      << "================================================================\n\n";

  report << "SIMULATION SUMMARY\n";
  report << "────────────────────────────────────────────────────────────\n";
  report << "  Runtime:              " << std::fixed << std::setprecision(1)
         << m_simulationTime << " seconds\n";
  report << "  Total Packets:        " << stats.totalPackets << "\n";
  report << "  Normal Packets:       " << stats.totalNormalPackets << "\n";
  report << "  Attack Packets:       " << stats.totalAttackPackets << "\n";
  report << "  Blocked Packets:      " << stats.totalBlocked << "\n\n";

  report << "TRAFFIC METRICS\n";
  report << "────────────────────────────────────────────────────────────\n";
  report << "  Current Rate:         " << std::fixed << std::setprecision(1)
         << stats.packetsPerSecond << " pkt/s\n";
  report << "  Peak Rate:            " << stats.peakPacketsPerSecond
         << " pkt/s\n";
  report << "  Unique Source IPs:    " << stats.uniqueSourceIPs << "\n\n";

  report << "SERVER STATUS\n";
  report << "────────────────────────────────────────────────────────────\n";
  report << "  Current Load:         "
         << static_cast<int>(stats.serverLoad * 100) << "%\n";
  report << "  Peak Load:            "
         << static_cast<int>(stats.peakServerLoad * 100) << "%\n";
  report << "  Active Connections:   " << stats.activeConnections << "\n";
  report << "  Processed:            " << m_server->getTotalProcessed() << "\n";
  report << "  Dropped:              " << m_server->getTotalDropped() << "\n\n";

  report << "DETECTION & MITIGATION\n";
  report << "────────────────────────────────────────────────────────────\n";
  report << "  Active Detector:      "
         << m_detectors[m_activeDetectorIndex]->getName() << "\n";
  report << "  Alerts Triggered:     " << stats.alertsTriggered << "\n";
  report << "  Blocked IPs:          " << stats.blockedIPs << "\n\n";

  auto blockedIPs = m_firewall.getBlockedIPs();
  if (!blockedIPs.empty()) {
    report << "BLOCKED IP LIST\n";
    report << "────────────────────────────────────────────────────────────\n";
    for (const auto &ip : blockedIPs) {
      report << "  - " << ip << "\n";
    }
    report << "\n";
  }

  report
      << "================================================================\n";
  report << "  Report generated by DDoS Detection Simulation v1.0\n";
  report
      << "================================================================\n";

  report.close();

  Logger::getInstance().info("Report exported to: " + filename, "Report");
}

} // namespace ddos
