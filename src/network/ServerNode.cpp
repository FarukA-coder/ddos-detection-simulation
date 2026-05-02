// ============================================================
//  DDoS Detection Simulation - Server Node Implementation
// ============================================================

#include "network/ServerNode.h"
#include <sstream>
#include <algorithm>

namespace ddos {

// ─── Constructor ───
ServerNode::ServerNode(const std::string& name, const std::string& ipAddress,
                       int maxCapacity, double maxBandwidth)
    : NetworkNode(name, ipAddress)
    , m_maxCapacity(maxCapacity)
    , m_maxBandwidth(maxBandwidth)
    , m_currentBandwidth(0.0)
    , m_activeConnections(0)
    , m_totalProcessed(0)
    , m_totalDropped(0)
    , m_load(0.0)
{
}

// ─── Server doesn't generate traffic (only responds) ───
std::vector<Packet> ServerNode::generatePackets() {
    return {}; // Server is passive in this simulation
}

// ─── Process incoming packets each tick ───
void ServerNode::update(double /*deltaTime*/) {
    processPackets();
}

// ─── Receive a packet ───
void ServerNode::receivePacket(const Packet& packet) {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (static_cast<int>(m_incomingQueue.size()) >= m_maxCapacity) {
        // Queue full — drop packet
        m_totalDropped++;
        return;
    }

    m_incomingQueue.push_back(packet);
    m_currentBandwidth += packet.getSize();
}

// ─── Process packets in queue ───
void ServerNode::processPackets() {
    std::lock_guard<std::mutex> lock(m_mutex);

    // Processing capacity decreases under high load (realistic behavior)
    int baseCapacity = m_maxCapacity / 20;  // slower processing
    int capacity = baseCapacity;
    if (m_load > 0.7) {
        capacity = baseCapacity / 2;  // server slows down under pressure
    }

    int toProcess = std::min(static_cast<int>(m_incomingQueue.size()), capacity);

    for (int i = 0; i < toProcess; ++i) {
        if (m_incomingQueue.empty()) break;

        const auto& pkt = m_incomingQueue.front();

        if (pkt.getType() == PacketType::SYN) {
            m_activeConnections++;
        } else if (pkt.getType() == PacketType::FIN ||
                   pkt.getType() == PacketType::RST) {
            m_activeConnections = std::max(0, m_activeConnections - 1);
        }

        m_totalProcessed++;
        m_incomingQueue.pop_front();
    }

    // Load = combined metric of queue, bandwidth, and connections
    double queueLoad = static_cast<double>(m_incomingQueue.size()) /
                       static_cast<double>(m_maxCapacity);
    double bandwidthLoad = m_currentBandwidth / m_maxBandwidth;
    double connectionLoad = static_cast<double>(m_activeConnections) /
                            static_cast<double>(m_maxCapacity);

    // Weighted combination for realistic server load
    m_load = std::min(1.0, queueLoad * 0.4 + bandwidthLoad * 0.3 + connectionLoad * 0.3);

    // Decay bandwidth over time
    m_currentBandwidth *= 0.92;
    if (m_activeConnections > 0 && m_incomingQueue.empty()) {
        m_activeConnections = static_cast<int>(m_activeConnections * 0.97);
    }
}

// ─── Status getters ───
double ServerNode::getLoad() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_load;
}

bool ServerNode::isOverloaded() const {
    return getLoad() > 0.8;
}

int ServerNode::getQueueSize() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return static_cast<int>(m_incomingQueue.size());
}

int ServerNode::getMaxCapacity() const { return m_maxCapacity; }

double ServerNode::getBandwidthUsage() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_currentBandwidth;
}

double ServerNode::getMaxBandwidth() const { return m_maxBandwidth; }

int ServerNode::getActiveConnections() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_activeConnections;
}

int ServerNode::getTotalProcessed() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_totalProcessed;
}

int ServerNode::getTotalDropped() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_totalDropped;
}

// ─── String representation ───
std::string ServerNode::toString() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::ostringstream oss;
    oss << "[Server] " << m_name << " (" << m_ipAddress << ")"
        << " | Load: " << static_cast<int>(m_load * 100) << "%"
        << " | Queue: " << m_incomingQueue.size() << "/" << m_maxCapacity
        << " | Connections: " << m_activeConnections;
    return oss.str();
}

// ─── Clear queue for recovery ───
void ServerNode::clearQueue() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_incomingQueue.clear();
    m_activeConnections = 0;
    m_currentBandwidth = 0.0;
    m_load = 0.0;
}

} // namespace ddos
