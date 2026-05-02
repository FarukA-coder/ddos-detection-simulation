// ============================================================
//  DDoS Detection Simulation - Packet Implementation
// ============================================================

#include "core/Packet.h"
#include <sstream>
#include <iomanip>

namespace ddos {

// Static member initialization
unsigned long long Packet::s_nextId = 0;

// ─── Default Constructor ───
Packet::Packet()
    : m_id(++s_nextId)
    , m_sourceIP("0.0.0.0")
    , m_destIP("0.0.0.0")
    , m_type(PacketType::ACK)
    , m_size(64)
    , m_ttl(64)
    , m_timestamp(std::chrono::steady_clock::now())
    , m_malicious(false)
{
}

// ─── Parameterized Constructor ───
Packet::Packet(const std::string& sourceIP,
               const std::string& destIP,
               PacketType type,
               int size,
               int ttl)
    : m_id(++s_nextId)
    , m_sourceIP(sourceIP)
    , m_destIP(destIP)
    , m_type(type)
    , m_size(size)
    , m_ttl(ttl)
    , m_timestamp(std::chrono::steady_clock::now())
    , m_malicious(false)
{
}

// ─── Getters ───
const std::string& Packet::getSourceIP() const { return m_sourceIP; }
const std::string& Packet::getDestIP() const { return m_destIP; }
PacketType Packet::getType() const { return m_type; }
int Packet::getSize() const { return m_size; }
int Packet::getTTL() const { return m_ttl; }
Packet::TimePoint Packet::getTimestamp() const { return m_timestamp; }
unsigned long long Packet::getId() const { return m_id; }
bool Packet::isMalicious() const { return m_malicious; }

// ─── Setters ───
void Packet::setMalicious(bool malicious) { m_malicious = malicious; }
void Packet::setTTL(int ttl) { m_ttl = ttl; }

// ─── Get timestamp as formatted string ───
std::string Packet::getTimestampString() const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::tm tm_buf;
    localtime_s(&tm_buf, &time_t);

    std::ostringstream oss;
    oss << std::put_time(&tm_buf, "%H:%M:%S");
    return oss.str();
}

// ─── String representation ───
std::string Packet::toString() const {
    std::ostringstream oss;
    oss << "[PKT #" << m_id << "] "
        << m_sourceIP << " -> " << m_destIP
        << " | Type: " << packetTypeToString(m_type)
        << " | Size: " << m_size << "B"
        << " | TTL: " << m_ttl;
    if (m_malicious) {
        oss << " | [MALICIOUS]";
    }
    return oss.str();
}

// ─── Operator Overloading: Stream insertion ───
std::ostream& operator<<(std::ostream& os, const Packet& packet) {
    os << packet.toString();
    return os;
}

// ─── Operator Overloading: Equality ───
bool Packet::operator==(const Packet& other) const {
    return m_id == other.m_id;
}

bool Packet::operator!=(const Packet& other) const {
    return !(*this == other);
}

// ─── Operator Overloading: Less-than (sort by timestamp) ───
bool Packet::operator<(const Packet& other) const {
    return m_timestamp < other.m_timestamp;
}

} // namespace ddos
