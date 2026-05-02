// DDoS Detection Simulation - ImGui Dashboard
#include "ui/Dashboard.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"
#include <GLFW/glfw3.h>
#include <cstdio>
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace ddos {

Dashboard::Dashboard(NetworkSimulator& sim) : m_simulator(sim) {}
Dashboard::~Dashboard() { shutdown(); }

bool Dashboard::initialize() {
    if (!glfwInit()) return false;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    m_window = glfwCreateWindow(1500, 900, "DDoS Detection Simulation v1.0", NULL, NULL);
    if (!m_window) { glfwTerminate(); return false; }
    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
    m_fontRegular = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    m_fontBold = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeuib.ttf", 18.0f);
    m_fontMono = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\consola.ttf", 15.0f);
    m_fontLarge = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeuib.ttf", 28.0f);
    if (!m_fontRegular) m_fontRegular = ImGui::GetIO().Fonts->AddFontDefault();
    if (!m_fontBold) m_fontBold = m_fontRegular;
    if (!m_fontMono) m_fontMono = m_fontRegular;
    if (!m_fontLarge) m_fontLarge = m_fontBold;
    setupStyle();
    return true;
}

void Dashboard::setupStyle() {
    ImGuiStyle& s = ImGui::GetStyle();
    s.WindowRounding = 8; s.FrameRounding = 5; s.GrabRounding = 4;
    s.ScrollbarRounding = 6; s.ChildRounding = 6; s.PopupRounding = 6;
    s.WindowPadding = ImVec2(12,12); s.FramePadding = ImVec2(8,5);
    s.ItemSpacing = ImVec2(8,6);
    auto& c = s.Colors;
    c[ImGuiCol_WindowBg] = ImVec4(0.04f,0.05f,0.10f,1);
    c[ImGuiCol_ChildBg] = ImVec4(0.06f,0.07f,0.13f,1);
    c[ImGuiCol_PopupBg] = ImVec4(0.08f,0.08f,0.15f,0.96f);
    c[ImGuiCol_Border] = ImVec4(0.10f,0.15f,0.25f,0.6f);
    c[ImGuiCol_FrameBg] = ImVec4(0.08f,0.10f,0.18f,1);
    c[ImGuiCol_FrameBgHovered] = ImVec4(0.12f,0.15f,0.25f,1);
    c[ImGuiCol_FrameBgActive] = ImVec4(0.10f,0.20f,0.35f,1);
    c[ImGuiCol_TitleBg] = ImVec4(0.05f,0.06f,0.12f,1);
    c[ImGuiCol_TitleBgActive] = ImVec4(0.06f,0.10f,0.20f,1);
    c[ImGuiCol_Header] = ImVec4(0.10f,0.14f,0.24f,1);
    c[ImGuiCol_HeaderHovered] = ImVec4(0.14f,0.20f,0.34f,1);
    c[ImGuiCol_HeaderActive] = ImVec4(0.06f,0.35f,0.55f,1);
    c[ImGuiCol_Button] = ImVec4(0.10f,0.18f,0.30f,1);
    c[ImGuiCol_ButtonHovered] = ImVec4(0.12f,0.28f,0.45f,1);
    c[ImGuiCol_ButtonActive] = ImVec4(0.05f,0.40f,0.65f,1);
    c[ImGuiCol_SliderGrab] = ImVec4(0.0f,0.65f,0.85f,1);
    c[ImGuiCol_SliderGrabActive] = ImVec4(0.0f,0.80f,1.0f,1);
    c[ImGuiCol_Text] = ImVec4(0.90f,0.93f,0.97f,1);
    c[ImGuiCol_TextDisabled] = ImVec4(0.45f,0.50f,0.58f,1);
    c[ImGuiCol_Separator] = ImVec4(0.12f,0.18f,0.28f,1);
    c[ImGuiCol_Tab] = ImVec4(0.08f,0.12f,0.22f,1);
    c[ImGuiCol_TabHovered] = ImVec4(0.12f,0.28f,0.48f,1);
    c[ImGuiCol_TabActive] = ImVec4(0.08f,0.32f,0.55f,1);
    c[ImGuiCol_PlotLines] = ImVec4(0.0f,0.75f,1.0f,1);
    c[ImGuiCol_PlotHistogram] = ImVec4(0.0f,0.75f,1.0f,0.7f);
    c[ImGuiCol_CheckMark] = ImVec4(0.0f,0.85f,0.65f,1);
    // ImPlot style
    ImPlot::StyleColorsAuto();
    ImPlotStyle& ps = ImPlot::GetStyle();
    ps.LineWeight = 2.0f;
    ps.PlotPadding = ImVec2(10,10);
    ps.FillAlpha = 0.18f;
}

void Dashboard::shutdown() {
    if (m_window) {
        ImPlot::DestroyContext();
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwDestroyWindow(m_window);
        glfwTerminate();
        m_window = nullptr;
    }
}

void Dashboard::drawGaugeBar(const char* label, float value, float maxVal,
                              unsigned int color, const char* overlay) {
    float ratio = (maxVal > 0) ? std::min(value / maxVal, 1.0f) : 0.0f;
    ImGui::Text("%s", label);
    ImGui::SameLine(140);
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, color);
    char buf[64];
    if (overlay) snprintf(buf, sizeof(buf), "%s", overlay);
    else snprintf(buf, sizeof(buf), "%.0f / %.0f", value, maxVal);
    ImGui::ProgressBar(ratio, ImVec2(-1, 22), buf);
    ImGui::PopStyleColor();
}

void Dashboard::renderHeaderBar() {
    auto stats = m_simulator.getStats();
    auto state = m_simulator.getState();
    auto det = m_simulator.getLastDetectionResult();
    ImGui::PushFont(m_fontLarge);
    ImGui::TextColored(ImVec4(0,0.8f,1,1), "  DDoS Detection Simulation");
    ImGui::PopFont();
    ImGui::SameLine(ImGui::GetWindowWidth() - 500);
    ImGui::PushFont(m_fontBold);
    if (state == SimulationState::RUNNING)
        ImGui::TextColored(ImVec4(0,0.9f,0.4f,1), "RUNNING");
    else if (state == SimulationState::PAUSED)
        ImGui::TextColored(ImVec4(1,0.8f,0,1), "PAUSED");
    else
        ImGui::TextColored(ImVec4(0.6f,0.6f,0.6f,1), "STOPPED");
    ImGui::SameLine();
    ImGui::Text("  |  Threat:");
    ImGui::SameLine();
    auto tl = det.threatLevel;
    ImVec4 tc = tl >= ThreatLevel::HIGH ? ImVec4(1,0.2f,0.2f,1) :
                tl >= ThreatLevel::MEDIUM ? ImVec4(1,0.7f,0,1) : ImVec4(0,0.9f,0.4f,1);
    ImGui::TextColored(tc, "%s", threatLevelToString(tl).c_str());
    ImGui::SameLine();
    ImGui::TextDisabled("  |  Time: %.0fs", m_simulator.getSimulationTime());
    ImGui::PopFont();
    ImGui::Separator();
}

void Dashboard::renderServerMetrics() {
    auto stats = m_simulator.getStats();
    auto& srv = m_simulator.getServer();
    ImGui::PushFont(m_fontBold);
    ImGui::TextColored(ImVec4(0,0.75f,1,1), "  Server Metrics");
    ImGui::PopFont();
    ImGui::Spacing();
    float load = (float)stats.serverLoad;
    unsigned int loadCol = load > 0.8f ? IM_COL32(240,60,60,255) :
                           load > 0.5f ? IM_COL32(240,180,0,255) : IM_COL32(0,200,100,255);
    char loadStr[32]; snprintf(loadStr, sizeof(loadStr), "%d%%", (int)(load*100));
    drawGaugeBar("Server Load", load, 1.0f, loadCol, loadStr);
    float pps = (float)stats.packetsPerSecond;
    float ppsMax = std::max(500.0f, (float)stats.peakPacketsPerSecond * 1.2f);
    unsigned int ppsCol = (pps/ppsMax) > 0.8f ? IM_COL32(240,60,60,255) :
                          (pps/ppsMax) > 0.5f ? IM_COL32(240,180,0,255) : IM_COL32(0,180,220,255);
    char ppsStr[32]; snprintf(ppsStr, sizeof(ppsStr), "%.0f pkt/s", pps);
    drawGaugeBar("Packets/sec", pps, ppsMax, ppsCol, ppsStr);
    float connMax = (float)srv.getMaxCapacity();
    float conn = (float)stats.activeConnections;
    char connStr[32]; snprintf(connStr, sizeof(connStr), "%d", stats.activeConnections);
    drawGaugeBar("Connections", conn, connMax, IM_COL32(100,140,230,255), connStr);
    float qRatio = (float)srv.getQueueSize() / (float)srv.getMaxCapacity();
    char qStr[32]; snprintf(qStr, sizeof(qStr), "%d / %d", srv.getQueueSize(), srv.getMaxCapacity());
    unsigned int qCol = qRatio > 0.8f ? IM_COL32(240,60,60,255) : IM_COL32(80,160,200,255);
    drawGaugeBar("Queue", (float)srv.getQueueSize(), (float)srv.getMaxCapacity(), qCol, qStr);
    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
    ImGui::Columns(2, "srvstats", false);
    ImGui::TextDisabled("Processed:"); ImGui::SameLine();
    ImGui::TextColored(ImVec4(0,0.85f,0.5f,1), "%d", srv.getTotalProcessed());
    ImGui::NextColumn();
    ImGui::TextDisabled("Dropped:"); ImGui::SameLine();
    ImGui::TextColored(ImVec4(1,0.3f,0.3f,1), "%d", srv.getTotalDropped());
    ImGui::Columns(1);
}

void Dashboard::renderDetectionPanel() {
    auto det = m_simulator.getLastDetectionResult();
    auto names = m_simulator.getDetectorNames();
    int idx = m_simulator.getActiveDetectorIndex();
    ImGui::PushFont(m_fontBold);
    ImGui::TextColored(ImVec4(1,0.85f,0,1), "  Detection Engine");
    ImGui::PopFont();
    ImGui::Spacing();
    ImGui::TextDisabled("Active Detector:");
    if (ImGui::BeginCombo("##detector", names[idx].c_str())) {
        for (int i = 0; i < (int)names.size(); i++) {
            bool sel = (i == idx);
            if (ImGui::Selectable(names[i].c_str(), sel))
                m_simulator.setActiveDetector(i);
            if (sel) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
    if (det.detected) {
        ImGui::PushFont(m_fontBold);
        ImGui::TextColored(ImVec4(1,0.2f,0.2f,1), "!! THREAT DETECTED !!");
        ImGui::PopFont();
        float conf = (float)det.confidence;
        ImVec4 confCol = conf > 0.7f ? ImVec4(1,0.2f,0.2f,1) :
                         conf > 0.4f ? ImVec4(1,0.7f,0,1) : ImVec4(0,0.8f,0.5f,1);
        ImGui::TextDisabled("Confidence:");
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImGui::ColorConvertFloat4ToU32(confCol));
        char confStr[16]; snprintf(confStr, sizeof(confStr), "%d%%", (int)(conf*100));
        ImGui::ProgressBar(conf, ImVec2(-1, 20), confStr);
        ImGui::PopStyleColor();
        auto tl = det.threatLevel;
        ImVec4 tlc = tl >= ThreatLevel::HIGH ? ImVec4(1,0.2f,0.2f,1) :
                     tl >= ThreatLevel::MEDIUM ? ImVec4(1,0.7f,0,1) : ImVec4(0,0.8f,1,1);
        ImGui::TextDisabled("Threat Level:"); ImGui::SameLine();
        ImGui::TextColored(tlc, "%s", threatLevelToString(tl).c_str());
        if (!det.suspiciousIPs.empty()) {
            ImGui::Spacing();
            ImGui::TextDisabled("Suspicious IPs:");
            for (int i = 0; i < std::min((int)det.suspiciousIPs.size(), 5); i++)
                ImGui::TextColored(ImVec4(1,0.5f,0.3f,1), "  %s", det.suspiciousIPs[i].c_str());
        }
    } else {
        ImGui::PushFont(m_fontBold);
        ImGui::TextColored(ImVec4(0,0.9f,0.5f,1), "  NORMAL");
        ImGui::PopFont();
        ImGui::TextDisabled("No threats detected.");
    }
}

void Dashboard::renderStatsPanel() {
    auto stats = m_simulator.getStats();
    ImGui::PushFont(m_fontBold);
    ImGui::TextColored(ImVec4(0,0.9f,0.5f,1), "  Statistics");
    ImGui::PopFont();
    ImGui::Spacing();
    auto row = [](const char* label, const char* val, ImVec4 col) {
        ImGui::TextDisabled("%s", label); ImGui::SameLine(130);
        ImGui::TextColored(col, "%s", val);
    };
    char buf[64];
    snprintf(buf, 64, "%llu", stats.totalPackets);
    row("Total Packets", buf, ImVec4(0.9f,0.9f,1,1));
    snprintf(buf, 64, "%llu", stats.totalNormalPackets);
    row("Normal", buf, ImVec4(0,0.9f,0.5f,1));
    snprintf(buf, 64, "%llu", stats.totalAttackPackets);
    row("Attack", buf, ImVec4(1,0.3f,0.3f,1));
    snprintf(buf, 64, "%llu", stats.totalBlocked);
    row("Blocked", buf, ImVec4(1,0.7f,0,1));
    ImGui::Separator();
    snprintf(buf, 64, "%.0f", stats.peakPacketsPerSecond);
    row("Peak PPS", buf, ImVec4(0.7f,0.8f,1,1));
    snprintf(buf, 64, "%d%%", (int)(stats.peakServerLoad*100));
    row("Peak Load", buf, ImVec4(0.7f,0.8f,1,1));
    snprintf(buf, 64, "%d", stats.alertsTriggered);
    row("Alerts", buf, ImVec4(1,0.5f,0.3f,1));
    snprintf(buf, 64, "%d", stats.blockedIPs);
    row("Blocked IPs", buf, ImVec4(1,0.7f,0,1));
    snprintf(buf, 64, "%d", stats.uniqueSourceIPs);
    row("Unique IPs", buf, ImVec4(0.5f,0.8f,1,1));
}

void Dashboard::renderTrafficGraph() {
    ImGui::PushFont(m_fontBold);
    ImGui::TextColored(ImVec4(0.6f,0.4f,1,1), "  Traffic Monitor");
    ImGui::PopFont();
    if (ImPlot::BeginPlot("##traffic", ImVec2(-1, -1), ImPlotFlags_NoTitle | ImPlotFlags_NoMenus)) {
        ImPlot::SetupAxes("Time (s)", "Packets/s", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
        ImPlot::SetupAxisLimits(ImAxis_X1, m_graphTime - 30, m_graphTime, ImGuiCond_Always);
        if (m_ppsBuffer.size() > 1) {
            ImPlot::SetNextLineStyle(ImVec4(0,0.8f,1,1), 2.0f);
            ImPlot::SetNextFillStyle(ImVec4(0,0.6f,1,0.15f));
            ImPlot::PlotLine("PPS", m_ppsBuffer.dataX.data(), m_ppsBuffer.dataY.data(), m_ppsBuffer.size(), 0, m_ppsBuffer.offset);
            ImPlot::PlotShaded("PPS", m_ppsBuffer.dataX.data(), m_ppsBuffer.dataY.data(), m_ppsBuffer.size(), 0, 0, m_ppsBuffer.offset);
        }
        if (m_loadBuffer.size() > 1) {
            ImPlot::SetNextLineStyle(ImVec4(1,0.4f,0.3f,0.8f), 1.5f);
            ImPlot::PlotLine("Load%", m_loadBuffer.dataX.data(), m_loadBuffer.dataY.data(), m_loadBuffer.size(), 0, m_loadBuffer.offset);
        }
        ImPlot::EndPlot();
    }
}

void Dashboard::renderSecurityLog() {
    ImGui::PushFont(m_fontBold);
    ImGui::TextColored(ImVec4(1,0.3f,0.3f,1), "  Security Log");
    ImGui::PopFont();
    ImGui::Spacing();
    auto logs = Logger::getInstance().getRecentLogs(30);
    ImGui::BeginChild("##logscroll", ImVec2(0, -1), false);
    ImGui::PushFont(m_fontMono);
    for (auto it = logs.rbegin(); it != logs.rend(); ++it) {
        ImVec4 col;
        const char* icon;
        switch (it->level) {
            case LogLevel::CRITICAL: col = ImVec4(1,0.15f,0.15f,1); icon = "[CRIT]"; break;
            case LogLevel::ALERT:    col = ImVec4(1,0.5f,0.2f,1);   icon = "[ALRT]"; break;
            case LogLevel::WARNING:  col = ImVec4(1,0.8f,0,1);      icon = "[WARN]"; break;
            case LogLevel::INFO:     col = ImVec4(0.4f,0.7f,1,1);   icon = "[INFO]"; break;
            default:                 col = ImVec4(0.5f,0.5f,0.5f,1); icon = "[DBG ]"; break;
        }
        ImGui::TextColored(ImVec4(0.5f,0.5f,0.5f,1), "%s", it->timestamp.c_str());
        ImGui::SameLine();
        ImGui::TextColored(col, "%s", icon);
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.8f,0.85f,0.9f,1), "%s: %s",
            it->source.c_str(), it->message.c_str());
    }
    ImGui::PopFont();
    ImGui::EndChild();
}

void Dashboard::renderControlPanel() {
    ImGui::PushFont(m_fontBold);
    bool running = m_simulator.isRunning();
    bool attacking = m_simulator.isUnderAttack();
    // Start/Stop
    if (running) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f,0.15f,0.15f,1));
        if (ImGui::Button("  Stop Simulation  ", ImVec2(180, 36))) m_simulator.stop();
        ImGui::PopStyleColor();
    } else {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f,0.45f,0.25f,1));
        if (ImGui::Button("  Start Simulation  ", ImVec2(180, 36))) m_simulator.start();
        ImGui::PopStyleColor();
    }
    ImGui::SameLine();
    // Attack
    const char* atkNames[] = {"SYN Flood","UDP Flood","HTTP Flood","DNS Amplification"};
    const AttackType atkTypes[] = {AttackType::SYN_FLOOD, AttackType::UDP_FLOOD,
                                   AttackType::HTTP_FLOOD, AttackType::DNS_AMPLIFICATION};
    if (attacking) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f,0.3f,0,1));
        if (ImGui::Button("  Stop Attack  ", ImVec2(150, 36))) m_simulator.stopAttack();
        ImGui::PopStyleColor();
    } else {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f,0.1f,0.1f,1));
        if (ImGui::Button("  Launch Attack  ", ImVec2(150, 36)))
            m_simulator.startAttack(atkTypes[m_selectedAttackType], m_attackIntensity);
        ImGui::PopStyleColor();
    }
    ImGui::SameLine();
    ImGui::SetNextItemWidth(170);
    ImGui::Combo("##atktype", &m_selectedAttackType, atkNames, 4);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(120);
    ImGui::SliderInt("##intensity", &m_attackIntensity, 10, 100, "Int: %d%%");
    if (attacking) { m_simulator.setAttackIntensity(m_attackIntensity); }
    ImGui::SameLine();
    ImGui::Spacing(); ImGui::SameLine();
    if (ImGui::Checkbox("Auto-Mitigate", &m_autoMitigate))
        m_simulator.setAutoMitigate(m_autoMitigate);
    ImGui::SameLine();
    ImGui::Spacing(); ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f,0.3f,0.5f,1));
    if (ImGui::Button("  Export Report  ", ImVec2(150, 36))) {
        m_simulator.generateReport("security_report.txt");
        Logger::getInstance().info("Report exported to security_report.txt", "Dashboard");
    }
    ImGui::PopStyleColor();
    ImGui::PopFont();
}

void Dashboard::renderFrame() {
    // Update graph data
    static auto lastUpdate = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    float dt = std::chrono::duration<float>(now - lastUpdate).count();
    if (dt >= 0.1f) {
        lastUpdate = now;
        m_graphTime += dt;
        auto stats = m_simulator.getStats();
        m_ppsBuffer.addPoint(m_graphTime, (float)stats.packetsPerSecond);
        m_loadBuffer.addPoint(m_graphTime, (float)(stats.serverLoad * 100.0));
    }
    // Full window
    ImGuiViewport* vp = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(vp->WorkPos);
    ImGui::SetNextWindowSize(vp->WorkSize);
    ImGui::Begin("##main", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoBringToFrontOnFocus);
    renderHeaderBar();
    ImGui::Spacing();
    float w = ImGui::GetContentRegionAvail().x;
    float h = ImGui::GetContentRegionAvail().y;
    // Top row: Server + Detection + Stats
    float topH = h * 0.38f;
    ImGui::BeginChild("##topcol", ImVec2(w, topH), false);
    float col1 = w * 0.38f, col2 = w * 0.32f, col3 = w * 0.28f;
    ImGui::BeginChild("##srv", ImVec2(col1, -1), true); renderServerMetrics(); ImGui::EndChild();
    ImGui::SameLine();
    ImGui::BeginChild("##det", ImVec2(col2, -1), true); renderDetectionPanel(); ImGui::EndChild();
    ImGui::SameLine();
    ImGui::BeginChild("##sts", ImVec2(0, -1), true); renderStatsPanel(); ImGui::EndChild();
    ImGui::EndChild();
    // Middle: Traffic graph
    float midH = h * 0.28f;
    ImGui::BeginChild("##graph", ImVec2(w, midH), true);
    renderTrafficGraph();
    ImGui::EndChild();
    // Bottom: Log + Controls
    float botH = ImGui::GetContentRegionAvail().y;
    ImGui::BeginChild("##logarea", ImVec2(w, botH - 55), true);
    renderSecurityLog();
    ImGui::EndChild();
    ImGui::BeginChild("##controls", ImVec2(w, 0), true);
    renderControlPanel();
    ImGui::EndChild();
    ImGui::End();
}

void Dashboard::run() {
    if (!initialize()) return;
    while (!glfwWindowShouldClose(m_window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        renderFrame();
        ImGui::Render();
        int dw, dh;
        glfwGetFramebufferSize(m_window, &dw, &dh);
        glViewport(0, 0, dw, dh);
        glClearColor(0.03f, 0.03f, 0.08f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(m_window);
    }
    shutdown();
}

} // namespace ddos
