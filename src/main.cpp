// ============================================================
//  DDoS Detection Simulation v1.0
//  Professional GUI Application (Dear ImGui + OpenGL)
//
//  OOP Concepts: Encapsulation, Inheritance, Polymorphism,
//  Abstraction, Composition, Operator Overloading, Templates,
//  Strategy/Singleton/Factory Patterns, RAII, Rule of Five
//
//  Author: Faruk | Course: OOP Final Project
// ============================================================

#include "simulation/NetworkSimulator.h"
#include "ui/Dashboard.h"
#include "logging/Logger.h"

#include <windows.h>

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    try {
        auto& logger = ddos::Logger::getInstance();
        logger.setMinLevel(ddos::LogLevel::INFO);
        logger.setFileLogging(true, "ddos_simulation.log");
        logger.info("=== DDoS Detection Simulation Starting ===", "Main");

        ddos::NetworkSimulator simulator;
        ddos::Dashboard dashboard(simulator);
        dashboard.run();

        logger.info("=== Simulation Terminated ===", "Main");
        return 0;
    }
    catch (const std::exception& e) {
        MessageBoxA(NULL, e.what(), "Fatal Error", MB_OK | MB_ICONERROR);
        return 1;
    }
}
