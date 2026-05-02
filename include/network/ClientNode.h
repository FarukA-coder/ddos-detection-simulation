#pragma once

// ============================================================
//  DDoS Detection Simulation - Client Node
//  Simulates normal user traffic patterns
// ============================================================

#include "NetworkNode.h"
#include <random>

namespace ddos {

class ClientNode : public NetworkNode {
public:
    // ─── Constructor ───
    ClientNode(const std::string& name, const std::string& ipAddress,
               double requestRate = 1.0);

    // ─── Override virtual methods ───
    std::vector<Packet> generatePackets() override;
    void update(double deltaTime) override;
    std::string getNodeType() const override { return "Client"; }

    // ─── Client-specific ───
    void setRequestRate(double rate);
    double getRequestRate() const;

private:
    double m_requestRate;       // Requests per second
    double m_accumulator;       // Time accumulator for packet generation
    std::mt19937 m_rng;
    std::string m_targetIP;     // Server IP

    Packet generateNormalPacket();
};

} // namespace ddos
