// ============================================================
//  DDoS Detection Simulation - Firewall Implementation
// ============================================================

#include "security/Firewall.h"

namespace ddos {

// ─── Constructor ───
Firewall::Firewall()
    : m_totalBlocked(0)
    , m_totalAllowed(0)
{
}

// ─── Block an IP ───
void Firewall::blockIP(const std::string& ip) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_whitelistedIPs.find(ip) == m_whitelistedIPs.end()) {
        m_blockedIPs.insert(ip);
    }
}

// ─── Unblock an IP ───
void Firewall::unblockIP(const std::string& ip) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_blockedIPs.erase(ip);
}

// ─── Whitelist an IP ───
void Firewall::whitelistIP(const std::string& ip) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_whitelistedIPs.insert(ip);
    m_blockedIPs.erase(ip); // Remove from blocked if present
}

// ─── Remove from whitelist ───
void Firewall::removeFromWhitelist(const std::string& ip) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_whitelistedIPs.erase(ip);
}

// ─── Filter packet: returns true if ALLOWED ───
bool Firewall::filterPacket(const Packet& packet) {
    std::lock_guard<std::mutex> lock(m_mutex);

    const std::string& srcIP = packet.getSourceIP();

    // Whitelisted IPs always pass
    if (m_whitelistedIPs.find(srcIP) != m_whitelistedIPs.end()) {
        m_totalAllowed++;
        return true;
    }

    // Check if blocked
    if (m_blockedIPs.find(srcIP) != m_blockedIPs.end()) {
        m_totalBlocked++;
        return false;
    }

    m_totalAllowed++;
    return true;
}

// ─── Queries ───
bool Firewall::isBlocked(const std::string& ip) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_blockedIPs.find(ip) != m_blockedIPs.end();
}

bool Firewall::isWhitelisted(const std::string& ip) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_whitelistedIPs.find(ip) != m_whitelistedIPs.end();
}

int Firewall::getBlockedIPCount() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return static_cast<int>(m_blockedIPs.size());
}

int Firewall::getTotalBlocked() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_totalBlocked;
}

int Firewall::getTotalAllowed() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_totalAllowed;
}

std::set<std::string> Firewall::getBlockedIPs() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_blockedIPs;
}

std::set<std::string> Firewall::getWhitelistedIPs() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_whitelistedIPs;
}

// ─── Reset ───
void Firewall::clearBlockedIPs() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_blockedIPs.clear();
}

void Firewall::clearAll() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_blockedIPs.clear();
    m_whitelistedIPs.clear();
    m_totalBlocked = 0;
    m_totalAllowed = 0;
}

} // namespace ddos
