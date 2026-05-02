#pragma once

// ============================================================
//  DDoS Detection Simulation - GUI Dashboard
//  Professional windowed application using Dear ImGui
// ============================================================

#include "simulation/NetworkSimulator.h"
#include "imgui.h"
#include "implot.h"
#include <vector>

struct GLFWwindow;

namespace ddos {

struct ScrollingBuffer {
    int maxSize;
    int offset;
    std::vector<float> dataX;
    std::vector<float> dataY;

    ScrollingBuffer(int max_size = 3000) : maxSize(max_size), offset(0) {
        dataX.reserve(maxSize);
        dataY.reserve(maxSize);
    }

    void addPoint(float x, float y) {
        if (static_cast<int>(dataX.size()) < maxSize) {
            dataX.push_back(x);
            dataY.push_back(y);
        } else {
            dataX[offset] = x;
            dataY[offset] = y;
            offset = (offset + 1) % maxSize;
        }
    }

    void clear() { dataX.clear(); dataY.clear(); offset = 0; }
    int size() const { return static_cast<int>(dataX.size()); }
};

class Dashboard {
public:
    explicit Dashboard(NetworkSimulator& simulator);
    ~Dashboard();

    bool initialize();
    void run();
    void shutdown();

private:
    void setupStyle();
    void renderFrame();
    void renderHeaderBar();
    void renderServerMetrics();
    void renderDetectionPanel();
    void renderStatsPanel();
    void renderTrafficGraph();
    void renderSecurityLog();
    void renderControlPanel();

    void drawGaugeBar(const char* label, float value, float maxVal,
                      ImU32 color, const char* overlay = nullptr);

    NetworkSimulator& m_simulator;
    GLFWwindow* m_window = nullptr;
    ImFont* m_fontRegular = nullptr;
    ImFont* m_fontBold = nullptr;
    ImFont* m_fontMono = nullptr;
    ImFont* m_fontLarge = nullptr;

    int m_selectedAttackType = 0;
    int m_attackIntensity = 60;
    bool m_autoMitigate = true;

    ScrollingBuffer m_ppsBuffer;
    ScrollingBuffer m_loadBuffer;
    float m_graphTime = 0.0f;
};

} // namespace ddos
