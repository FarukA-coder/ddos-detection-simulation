#pragma once

// ============================================================
//  DDoS Detection Simulation - Network Node (Abstract Base)
//  Demonstrates: Abstraction, Inheritance, Polymorphism
// ============================================================

#include "core/Packet.h"
#include <string>
#include <vector>
#include <memory>

namespace ddos {

class NetworkNode {
public:
    // ─── Constructor / Virtual Destructor ───
    NetworkNode(const std::string& name, const std::string& ipAddress)
        : m_name(name), m_ipAddress(ipAddress), m_active(true) {}

    virtual ~NetworkNode() = default;

    // ─── Pure Virtual Methods (Abstract) ───
    virtual std::vector<Packet> generatePackets() = 0;
    virtual void update(double deltaTime) = 0;
    virtual std::string getNodeType() const = 0;

    // ─── Common Interface ───
    const std::string& getName() const { return m_name; }
    const std::string& getIP() const { return m_ipAddress; }
    bool isActive() const { return m_active; }
    void setActive(bool active) { m_active = active; }

    // ─── String representation ───
    virtual std::string toString() const {
        return "[" + getNodeType() + "] " + m_name + " (" + m_ipAddress + ")";
    }

protected:
    std::string m_name;
    std::string m_ipAddress;
    bool m_active;
};

} // namespace ddos
