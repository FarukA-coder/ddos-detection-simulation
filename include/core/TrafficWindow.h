#pragma once

// ============================================================
//  DDoS Detection Simulation - Traffic Window
//  Sliding window for packet analysis and statistics
// ============================================================

#include "Packet.h"
#include <deque>
#include <map>
#include <set>
#include <vector>
#include <mutex>
#include <chrono>

namespace ddos {

class TrafficWindow {
public:
    // ─── Constructor ───
    explicit TrafficWindow(size_t maxSize = 5000,
                           std::chrono::seconds windowDuration = std::chrono::seconds(60));

    // ─── Packet Management ───
    void addPacket(const Packet& packet);
    void clear();
    void pruneOldPackets();

    // ─── Statistics ───
    size_t getPacketCount() const;
    double getPacketsPerSecond() const;
    std::set<std::string> getUniqueSourceIPs() const;
    int getUniqueSourceIPCount() const;

    // ─── Type-based queries ───
    std::vector<Packet> getPacketsByType(PacketType type) const;
    std::map<PacketType, int> getTypeDistribution() const;

    // ─── IP-based queries ───
    std::map<std::string, int> getIPFrequencyMap() const;
    std::vector<std::pair<std::string, int>> getTopSourceIPs(int count = 10) const;

    // ─── Time-series data (for graphs) ───
    std::vector<float> getTrafficHistory(int buckets = 60) const;

    // ─── Access ───
    const std::deque<Packet>& getPackets() const;
    bool isEmpty() const;

private:
    std::deque<Packet> m_packets;
    size_t m_maxSize;
    std::chrono::seconds m_windowDuration;
    mutable std::mutex m_mutex;
};

} // namespace ddos
