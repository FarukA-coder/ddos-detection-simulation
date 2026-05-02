#pragma once

// ============================================================
//  DDoS Detection Simulation - Enum Definitions
//  Core type enumerations used across the simulation
// ============================================================

#include <string>
#include <vector>

namespace ddos {

// ─── Packet Types ───
enum class PacketType {
    SYN,            // TCP SYN (connection request)
    SYN_ACK,        // TCP SYN-ACK (connection acknowledgment)
    ACK,            // TCP ACK
    UDP,            // UDP datagram
    HTTP_GET,       // HTTP GET request
    HTTP_POST,      // HTTP POST request
    DNS_QUERY,      // DNS query
    DNS_RESPONSE,   // DNS response
    ICMP_ECHO,      // ICMP echo (ping)
    ICMP_REPLY,     // ICMP echo reply
    FIN,            // TCP FIN (connection termination)
    RST             // TCP RST (connection reset)
};

// ─── Attack Types ───
enum class AttackType {
    NONE,
    SYN_FLOOD,          // TCP SYN flood attack
    UDP_FLOOD,          // UDP flood attack
    HTTP_FLOOD,         // HTTP request flood
    DNS_AMPLIFICATION   // DNS amplification attack
};

// ─── Log Levels ───
enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ALERT,
    CRITICAL
};

// ─── Threat Level ───
enum class ThreatLevel {
    SAFE,           // No threat detected
    LOW,            // Minor anomaly
    MEDIUM,         // Suspicious activity
    HIGH,           // Likely attack
    CRITICAL        // Confirmed attack
};

// ─── Simulation State ───
enum class SimulationState {
    STOPPED,
    RUNNING,
    PAUSED
};

// ─── Detection Result ───
struct DetectionResult {
    bool detected = false;
    double confidence = 0.0;        // 0.0 to 1.0
    ThreatLevel threatLevel = ThreatLevel::SAFE;
    std::string description;
    std::string detectorName;
    std::vector<std::string> suspiciousIPs;
};

// ─── Simulation Statistics ───
struct SimulationStats {
    // Traffic stats
    unsigned long long totalPackets = 0;
    unsigned long long totalBlocked = 0;
    unsigned long long totalNormalPackets = 0;
    unsigned long long totalAttackPackets = 0;

    // Rate stats
    double packetsPerSecond = 0.0;
    double peakPacketsPerSecond = 0.0;

    // Server stats
    double serverLoad = 0.0;
    double peakServerLoad = 0.0;
    int activeConnections = 0;

    // Detection stats
    int alertsTriggered = 0;
    int falsePositives = 0;
    int truePositives = 0;

    // Node stats
    int normalClients = 0;
    int attackerNodes = 0;
    int blockedIPs = 0;

    // Unique IPs
    int uniqueSourceIPs = 0;
};

// ─── Helper: Convert enums to strings ───

inline std::string packetTypeToString(PacketType type) {
    switch (type) {
        case PacketType::SYN:          return "SYN";
        case PacketType::SYN_ACK:      return "SYN-ACK";
        case PacketType::ACK:          return "ACK";
        case PacketType::UDP:          return "UDP";
        case PacketType::HTTP_GET:     return "HTTP GET";
        case PacketType::HTTP_POST:    return "HTTP POST";
        case PacketType::DNS_QUERY:    return "DNS QUERY";
        case PacketType::DNS_RESPONSE: return "DNS RESP";
        case PacketType::ICMP_ECHO:    return "ICMP ECHO";
        case PacketType::ICMP_REPLY:   return "ICMP REPLY";
        case PacketType::FIN:          return "FIN";
        case PacketType::RST:          return "RST";
        default:                       return "UNKNOWN";
    }
}

inline std::string attackTypeToString(AttackType type) {
    switch (type) {
        case AttackType::NONE:              return "None";
        case AttackType::SYN_FLOOD:         return "SYN Flood";
        case AttackType::UDP_FLOOD:         return "UDP Flood";
        case AttackType::HTTP_FLOOD:        return "HTTP Flood";
        case AttackType::DNS_AMPLIFICATION: return "DNS Amplification";
        default:                            return "Unknown";
    }
}

inline std::string logLevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG:    return "DEBUG";
        case LogLevel::INFO:     return "INFO";
        case LogLevel::WARNING:  return "WARNING";
        case LogLevel::ALERT:    return "ALERT";
        case LogLevel::CRITICAL: return "CRITICAL";
        default:                 return "UNKNOWN";
    }
}

inline std::string threatLevelToString(ThreatLevel level) {
    switch (level) {
        case ThreatLevel::SAFE:     return "SAFE";
        case ThreatLevel::LOW:      return "LOW";
        case ThreatLevel::MEDIUM:   return "MEDIUM";
        case ThreatLevel::HIGH:     return "HIGH";
        case ThreatLevel::CRITICAL: return "CRITICAL";
        default:                    return "UNKNOWN";
    }
}

} // namespace ddos
