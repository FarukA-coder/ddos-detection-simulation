#pragma once

// ============================================================
//  DDoS Detection Simulation - Server Node
//  Simulates target server receiving traffic
// ============================================================

#include "NetworkNode.h"
#include <deque>
#include <mutex>

namespace ddos {

class ServerNode : public NetworkNode {
public:
    // ─── Constructor ───
    ServerNode(const std::string& name, const std::string& ipAddress,
               int maxCapacity = 1000, double maxBandwidth = 10000.0);

    // ─── Override virtual methods ───
    std::vector<Packet> generatePackets() override;
    void update(double deltaTime) override;
    std::string getNodeType() const override { return "Server"; }

    // ─── Server operations ───
    void receivePacket(const Packet& packet);
    void processPackets();

    // ─── Status ───
    double getLoad() const;
    bool isOverloaded() const;
    int getQueueSize() const;
    int getMaxCapacity() const;
    double getBandwidthUsage() const;
    double getMaxBandwidth() const;
    int getActiveConnections() const;
    int getTotalProcessed() const;
    int getTotalDropped() const;

    // ─── Override toString ───
    std::string toString() const override;

    // ─── Recovery ───
    void clearQueue();

private:
    int m_maxCapacity;          // Max queue size
    double m_maxBandwidth;      // Max bandwidth (bytes/sec)
    double m_currentBandwidth;  // Current bandwidth usage
    int m_activeConnections;
    int m_totalProcessed;
    int m_totalDropped;
    double m_load;              // 0.0 to 1.0

    std::deque<Packet> m_incomingQueue;
    mutable std::mutex m_mutex;
};

} // namespace ddos
