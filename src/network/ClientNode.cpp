// ============================================================
//  DDoS Detection Simulation - Client Node Implementation
// ============================================================

#include "network/ClientNode.h"

namespace ddos {

// ─── Constructor ───
ClientNode::ClientNode(const std::string& name, const std::string& ipAddress,
                       double requestRate)
    : NetworkNode(name, ipAddress)
    , m_requestRate(requestRate)
    , m_accumulator(0.0)
    , m_rng(std::random_device{}())
    , m_targetIP("10.0.0.1")
{
}

// ─── Generate packets based on accumulated time ───
std::vector<Packet> ClientNode::generatePackets() {
    std::vector<Packet> packets;

    if (!m_active) return packets;

    // Use Poisson-like distribution for realistic traffic
    std::poisson_distribution<int> dist(m_requestRate);
    int packetCount = dist(m_rng);

    // Clamp to reasonable values
    packetCount = std::min(packetCount, 5);

    for (int i = 0; i < packetCount; ++i) {
        packets.push_back(generateNormalPacket());
    }

    return packets;
}

// ─── Update internal state ───
void ClientNode::update(double deltaTime) {
    m_accumulator += deltaTime;

    // Simulate variable activity patterns
    std::uniform_real_distribution<double> activityDist(0.0, 1.0);
    if (activityDist(m_rng) < 0.02) {
        // 2% chance to temporarily change activity
        std::uniform_real_distribution<double> rateDist(0.5, 3.0);
        m_requestRate = rateDist(m_rng);
    }
}

// ─── Generate a single normal packet ───
Packet ClientNode::generateNormalPacket() {
    // Randomly choose packet type (realistic distribution)
    static const PacketType normalTypes[] = {
        PacketType::HTTP_GET,
        PacketType::HTTP_POST,
        PacketType::DNS_QUERY,
        PacketType::ACK,
        PacketType::SYN
    };
    static const double weights[] = { 0.4, 0.15, 0.15, 0.2, 0.1 };

    std::uniform_real_distribution<double> dist(0.0, 1.0);
    double roll = dist(m_rng);

    PacketType type = PacketType::HTTP_GET;
    double cumulative = 0.0;
    for (int i = 0; i < 5; ++i) {
        cumulative += weights[i];
        if (roll <= cumulative) {
            type = normalTypes[i];
            break;
        }
    }

    // Realistic packet sizes
    std::uniform_int_distribution<int> sizeDist(40, 1500);
    int size = sizeDist(m_rng);

    return Packet(m_ipAddress, m_targetIP, type, size, 64);
}

// ─── Setters & Getters ───
void ClientNode::setRequestRate(double rate) { m_requestRate = rate; }
double ClientNode::getRequestRate() const { return m_requestRate; }

} // namespace ddos
