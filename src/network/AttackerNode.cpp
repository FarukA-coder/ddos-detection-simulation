// ============================================================
//  DDoS Detection Simulation - Attacker Node Implementation
// ============================================================

#include "network/AttackerNode.h"
#include <sstream>

namespace ddos {

// ─── Constructor ───
AttackerNode::AttackerNode(const std::string& name, const std::string& ipAddress,
                           AttackType attackType, int intensity)
    : NetworkNode(name, ipAddress)
    , m_attackType(attackType)
    , m_intensity(std::min(100, std::max(1, intensity)))
    , m_attacking(false)
    , m_spoofing(true)
    , m_rng(std::random_device{}())
    , m_targetIP("10.0.0.1")
{
}

// ─── Generate attack packets ───
std::vector<Packet> AttackerNode::generatePackets() {
    if (!m_active || !m_attacking) {
        return {};
    }

    // Dispatch to appropriate attack generator (Factory-like pattern)
    switch (m_attackType) {
        case AttackType::SYN_FLOOD:         return generateSynFlood();
        case AttackType::UDP_FLOOD:         return generateUdpFlood();
        case AttackType::HTTP_FLOOD:        return generateHttpFlood();
        case AttackType::DNS_AMPLIFICATION: return generateDnsAmplification();
        default:                            return {};
    }
}

// ─── Update ───
void AttackerNode::update(double /*deltaTime*/) {
    // Attackers can vary intensity over time
    if (m_attacking) {
        std::uniform_real_distribution<double> jitter(-5.0, 5.0);
        int newIntensity = m_intensity + static_cast<int>(jitter(m_rng));
        m_intensity = std::min(100, std::max(1, newIntensity));
    }
}

// ─── Attack control ───
void AttackerNode::startAttack() { m_attacking = true; }
void AttackerNode::stopAttack() { m_attacking = false; }
bool AttackerNode::isAttacking() const { return m_attacking; }

// ─── Configuration ───
void AttackerNode::setAttackType(AttackType type) { m_attackType = type; }
AttackType AttackerNode::getAttackType() const { return m_attackType; }
void AttackerNode::setIntensity(int intensity) {
    m_intensity = std::min(100, std::max(1, intensity));
}
int AttackerNode::getIntensity() const { return m_intensity; }
void AttackerNode::setSpoofing(bool enabled) { m_spoofing = enabled; }
bool AttackerNode::isSpoofing() const { return m_spoofing; }

// ─── String representation ───
std::string AttackerNode::toString() const {
    std::ostringstream oss;
    oss << "[Attacker] " << m_name << " (" << m_ipAddress << ")"
        << " | Attack: " << attackTypeToString(m_attackType)
        << " | Intensity: " << m_intensity << "%"
        << " | " << (m_attacking ? "ACTIVE" : "IDLE");
    return oss.str();
}

// ═══════════════════════════════════════════════════════════
//  Attack Generators
// ═══════════════════════════════════════════════════════════

// ─── SYN Flood: High volume of SYN packets ───
std::vector<Packet> AttackerNode::generateSynFlood() {
    std::vector<Packet> packets;
    int count = m_intensity / 2 + 1;

    for (int i = 0; i < count; ++i) {
        // Use real IP ~40% of time so detector can identify attacker
        bool useReal = !m_spoofing || (m_rng() % 5 < 2);
        std::string srcIP = useReal ? m_ipAddress : generateSpoofedIP();
        Packet pkt(srcIP, m_targetIP, PacketType::SYN, 60, 64);
        pkt.setMalicious(true);
        packets.push_back(pkt);
    }
    return packets;
}

// ─── UDP Flood: Large UDP datagrams ───
std::vector<Packet> AttackerNode::generateUdpFlood() {
    std::vector<Packet> packets;
    int count = m_intensity / 3 + 1;
    std::uniform_int_distribution<int> sizeDist(512, 65535);

    for (int i = 0; i < count; ++i) {
        bool useReal = !m_spoofing || (m_rng() % 5 < 2);
        std::string srcIP = useReal ? m_ipAddress : generateSpoofedIP();
        int size = sizeDist(m_rng);
        Packet pkt(srcIP, m_targetIP, PacketType::UDP, size, 128);
        pkt.setMalicious(true);
        packets.push_back(pkt);
    }
    return packets;
}

// ─── HTTP Flood: Many HTTP GET/POST requests ───
std::vector<Packet> AttackerNode::generateHttpFlood() {
    std::vector<Packet> packets;
    int count = m_intensity / 4 + 1;

    for (int i = 0; i < count; ++i) {
        bool useReal = !m_spoofing || (m_rng() % 5 < 2);
        std::string srcIP = useReal ? m_ipAddress : generateSpoofedIP();
        PacketType type = (m_rng() % 3 == 0) ? PacketType::HTTP_POST : PacketType::HTTP_GET;
        std::uniform_int_distribution<int> sizeDist(200, 2000);
        Packet pkt(srcIP, m_targetIP, type, sizeDist(m_rng), 64);
        pkt.setMalicious(true);
        packets.push_back(pkt);
    }
    return packets;
}

// ─── DNS Amplification: DNS queries with spoofed source ───
std::vector<Packet> AttackerNode::generateDnsAmplification() {
    std::vector<Packet> packets;
    int count = m_intensity / 3 + 1;

    for (int i = 0; i < count; ++i) {
        // Attacker sends small DNS query, but response is amplified
        std::string srcIP = m_targetIP; // Spoofed to target (reflection)
        Packet queryPkt(srcIP, "8.8.8.8", PacketType::DNS_QUERY, 40, 64);
        queryPkt.setMalicious(true);
        packets.push_back(queryPkt);

        // Simulated amplified response hitting the target
        Packet responsePkt("8.8.8.8", m_targetIP, PacketType::DNS_RESPONSE, 4000, 64);
        responsePkt.setMalicious(true);
        packets.push_back(responsePkt);
    }
    return packets;
}

// ─── Generate spoofed IP address ───
std::string AttackerNode::generateSpoofedIP() {
    std::uniform_int_distribution<int> octet(1, 254);
    return std::to_string(octet(m_rng)) + "." +
           std::to_string(octet(m_rng)) + "." +
           std::to_string(octet(m_rng)) + "." +
           std::to_string(octet(m_rng));
}

} // namespace ddos
