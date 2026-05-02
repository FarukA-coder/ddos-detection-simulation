#pragma once

// ============================================================
//  DDoS Detection Simulation - Firewall
//  IP-based packet filtering and blocking
// ============================================================

#include "core/Packet.h"
#include <set>
#include <vector>
#include <mutex>
#include <string>

namespace ddos {

class Firewall {
public:
    // ─── Constructor ───
    Firewall();

    // ─── IP Management ───
    void blockIP(const std::string& ip);
    void unblockIP(const std::string& ip);
    void whitelistIP(const std::string& ip);
    void removeFromWhitelist(const std::string& ip);

    // ─── Packet filtering ───
    bool filterPacket(const Packet& packet);  // Returns true if packet is ALLOWED

    // ─── Queries ───
    bool isBlocked(const std::string& ip) const;
    bool isWhitelisted(const std::string& ip) const;
    int getBlockedIPCount() const;
    int getTotalBlocked() const;
    int getTotalAllowed() const;

    // ─── Access blocked list ───
    std::set<std::string> getBlockedIPs() const;
    std::set<std::string> getWhitelistedIPs() const;

    // ─── Reset ───
    void clearBlockedIPs();
    void clearAll();

private:
    std::set<std::string> m_blockedIPs;
    std::set<std::string> m_whitelistedIPs;
    int m_totalBlocked;
    int m_totalAllowed;
    mutable std::mutex m_mutex;
};

} // namespace ddos
