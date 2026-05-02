#pragma once

// ============================================================
//  DDoS Detection Simulation - Packet Class
//  Represents a network packet in the simulation
// ============================================================

#include "Enums.h"
#include <string>
#include <chrono>
#include <ostream>

namespace ddos {

class Packet {
public:
    using TimePoint = std::chrono::steady_clock::time_point;

    // ─── Constructors ───
    Packet();
    Packet(const std::string& sourceIP,
           const std::string& destIP,
           PacketType type,
           int size = 64,
           int ttl = 64);

    // ─── Copy & Move (Rule of Five) ───
    Packet(const Packet& other) = default;
    Packet(Packet&& other) noexcept = default;
    Packet& operator=(const Packet& other) = default;
    Packet& operator=(Packet&& other) noexcept = default;
    ~Packet() = default;

    // ─── Getters ───
    const std::string& getSourceIP() const;
    const std::string& getDestIP() const;
    PacketType getType() const;
    int getSize() const;
    int getTTL() const;
    TimePoint getTimestamp() const;
    unsigned long long getId() const;
    bool isMalicious() const;

    // ─── Setters ───
    void setMalicious(bool malicious);
    void setTTL(int ttl);

    // ─── Utility ───
    std::string toString() const;
    std::string getTimestampString() const;

    // ─── Operator Overloading (OOP Concept) ───
    friend std::ostream& operator<<(std::ostream& os, const Packet& packet);
    bool operator==(const Packet& other) const;
    bool operator!=(const Packet& other) const;
    bool operator<(const Packet& other) const;  // Sort by timestamp

private:
    static unsigned long long s_nextId;

    unsigned long long m_id;
    std::string m_sourceIP;
    std::string m_destIP;
    PacketType m_type;
    int m_size;         // bytes
    int m_ttl;          // Time To Live
    TimePoint m_timestamp;
    bool m_malicious;
};

} // namespace ddos
