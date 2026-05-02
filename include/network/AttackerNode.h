#pragma once

// ============================================================
//  DDoS Detection Simulation - Attacker Node
//  Simulates various DDoS attack patterns
//  Demonstrates: Inheritance, Encapsulation
// ============================================================

#include "NetworkNode.h"
#include "core/Enums.h"
#include <random>
#include <vector>

namespace ddos {

class AttackerNode : public NetworkNode {
public:
    // ─── Constructor ───
    AttackerNode(const std::string& name, const std::string& ipAddress,
                 AttackType attackType = AttackType::SYN_FLOOD,
                 int intensity = 50);

    // ─── Override virtual methods ───
    std::vector<Packet> generatePackets() override;
    void update(double deltaTime) override;
    std::string getNodeType() const override { return "Attacker"; }

    // ─── Attack control ───
    void startAttack();
    void stopAttack();
    bool isAttacking() const;

    // ─── Configuration ───
    void setAttackType(AttackType type);
    AttackType getAttackType() const;
    void setIntensity(int intensity);
    int getIntensity() const;
    void setSpoofing(bool enabled);
    bool isSpoofing() const;

    // ─── Override toString ───
    std::string toString() const override;

private:
    AttackType m_attackType;
    int m_intensity;        // 1-100 scale
    bool m_attacking;
    bool m_spoofing;        // IP spoofing enabled
    std::mt19937 m_rng;
    std::string m_targetIP;

    // ─── Attack generators (Factory-like pattern) ───
    std::vector<Packet> generateSynFlood();
    std::vector<Packet> generateUdpFlood();
    std::vector<Packet> generateHttpFlood();
    std::vector<Packet> generateDnsAmplification();

    std::string generateSpoofedIP();
};

} // namespace ddos
