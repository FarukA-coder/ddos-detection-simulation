// ============================================================
//  DDoS Detection Simulation - Traffic Window Implementation
// ============================================================

#include "core/TrafficWindow.h"
#include <algorithm>
#include <numeric>

namespace ddos {

// ─── Constructor ───
TrafficWindow::TrafficWindow(size_t maxSize, std::chrono::seconds windowDuration)
    : m_maxSize(maxSize)
    , m_windowDuration(windowDuration)
{
}

// ─── Add packet to window ───
void TrafficWindow::addPacket(const Packet& packet) {
    std::lock_guard<std::mutex> lock(m_mutex);

    m_packets.push_back(packet);

    // Remove oldest if over capacity
    while (m_packets.size() > m_maxSize) {
        m_packets.pop_front();
    }
}

// ─── Clear all packets ───
void TrafficWindow::clear() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_packets.clear();
}

// ─── Remove packets older than window duration ───
void TrafficWindow::pruneOldPackets() {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto now = std::chrono::steady_clock::now();

    while (!m_packets.empty()) {
        auto age = std::chrono::duration_cast<std::chrono::seconds>(
            now - m_packets.front().getTimestamp()
        );
        if (age > m_windowDuration) {
            m_packets.pop_front();
        } else {
            break;
        }
    }
}

// ─── Get total packet count ───
size_t TrafficWindow::getPacketCount() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_packets.size();
}

// ─── Calculate packets per second ───
double TrafficWindow::getPacketsPerSecond() const {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_packets.size() < 2) return 0.0;

    auto timeSpan = std::chrono::duration_cast<std::chrono::milliseconds>(
        m_packets.back().getTimestamp() - m_packets.front().getTimestamp()
    );

    double seconds = timeSpan.count() / 1000.0;
    if (seconds <= 0.0) return static_cast<double>(m_packets.size());

    return static_cast<double>(m_packets.size()) / seconds;
}

// ─── Get unique source IPs ───
std::set<std::string> TrafficWindow::getUniqueSourceIPs() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::set<std::string> ips;
    for (const auto& pkt : m_packets) {
        ips.insert(pkt.getSourceIP());
    }
    return ips;
}

// ─── Get unique source IP count ───
int TrafficWindow::getUniqueSourceIPCount() const {
    return static_cast<int>(getUniqueSourceIPs().size());
}

// ─── Filter packets by type ───
std::vector<Packet> TrafficWindow::getPacketsByType(PacketType type) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<Packet> result;
    for (const auto& pkt : m_packets) {
        if (pkt.getType() == type) {
            result.push_back(pkt);
        }
    }
    return result;
}

// ─── Get packet type distribution ───
std::map<PacketType, int> TrafficWindow::getTypeDistribution() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::map<PacketType, int> dist;
    for (const auto& pkt : m_packets) {
        dist[pkt.getType()]++;
    }
    return dist;
}

// ─── Get IP frequency map ───
std::map<std::string, int> TrafficWindow::getIPFrequencyMap() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::map<std::string, int> freqMap;
    for (const auto& pkt : m_packets) {
        freqMap[pkt.getSourceIP()]++;
    }
    return freqMap;
}

// ─── Get top N source IPs by frequency ───
std::vector<std::pair<std::string, int>> TrafficWindow::getTopSourceIPs(int count) const {
    auto freqMap = getIPFrequencyMap();

    std::vector<std::pair<std::string, int>> sorted(freqMap.begin(), freqMap.end());
    std::sort(sorted.begin(), sorted.end(),
        [](const auto& a, const auto& b) { return a.second > b.second; });

    if (static_cast<int>(sorted.size()) > count) {
        sorted.resize(count);
    }
    return sorted;
}

// ─── Get traffic history for graphing ───
std::vector<float> TrafficWindow::getTrafficHistory(int buckets) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<float> history(buckets, 0.0f);

    if (m_packets.empty()) return history;

    auto now = std::chrono::steady_clock::now();
    double bucketDuration = static_cast<double>(m_windowDuration.count()) / buckets;

    for (const auto& pkt : m_packets) {
        auto age = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - pkt.getTimestamp()
        ).count() / 1000.0;

        int bucketIdx = buckets - 1 - static_cast<int>(age / bucketDuration);
        if (bucketIdx >= 0 && bucketIdx < buckets) {
            history[bucketIdx] += 1.0f;
        }
    }

    return history;
}

// ─── Get all packets ───
const std::deque<Packet>& TrafficWindow::getPackets() const {
    return m_packets;
}

// ─── Check if empty ───
bool TrafficWindow::isEmpty() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_packets.empty();
}

} // namespace ddos
