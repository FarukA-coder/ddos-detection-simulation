# DDoS Detection Simulation v1.0

## Professional Network Security Monitoring Tool

A real-time DDoS (Distributed Denial of Service) attack detection and mitigation simulation built with C++ using advanced Object-Oriented Programming concepts.

---

## Features

### Attack Simulation
- **SYN Flood** — High-volume TCP SYN packet flooding
- **UDP Flood** — Large UDP datagram flooding  
- **HTTP Flood** — Fake HTTP GET/POST request flooding
- **DNS Amplification** — DNS reflection/amplification attack

### Detection Algorithms
- **Threshold Detector** — Volume-based detection with configurable limits
- **Entropy Detector** — Shannon entropy analysis of traffic distribution
- **Rate Limit Detector** — Per-IP request rate monitoring

### Real-time Dashboard
- Live server load and bandwidth gauges
- Traffic volume graph (60-second window)
- Detection status with confidence scoring
- Security alert log
- Interactive controls (F1-F5 keys)

### Security Features
- Automatic IP blocking (auto-mitigation)
- Firewall with whitelist/blacklist
- Detailed security report generation
- Comprehensive logging system

---

## OOP Concepts Demonstrated

| Concept | Implementation |
|---------|---------------|
| Encapsulation | Private data + public interface in all classes |
| Inheritance | NetworkNode → ClientNode, AttackerNode, ServerNode |
| Polymorphism | IDetector interface with virtual dispatch |
| Abstraction | Abstract base classes (NetworkNode, IDetector) |
| Composition | NetworkSimulator owns all components |
| Operator Overloading | Packet: <<, ==, !=, < operators |
| Templates | Logger::logValue<T>() |
| Strategy Pattern | Swappable detection algorithms |
| Singleton Pattern | Logger class |
| Factory Pattern | Attack packet generators |
| Observer Pattern | Simulator → Detector notification |
| RAII | Smart pointers, lock_guard |
| Rule of Five | Packet class |

---

## Building

### Prerequisites
- Visual Studio 2022 (Community or higher)
- CMake 3.14+ (included with VS 2022)
- Internet connection (for FTXUI download on first build)

### Build Steps

1. Double-click `build.bat` or run from command line:
```batch
build.bat
```

2. The build script will:
   - Set up the Visual Studio environment
   - Configure CMake
   - Download FTXUI library (first time only)
   - Build the Release executable

3. The executable will be at: `build/DDoSDetectionSimulation.exe`

---

## Controls

| Key | Action |
|-----|--------|
| F1 | Start/Stop Simulation |
| F2 | Start/Stop Attack |
| F3 | Cycle Detection Algorithm |
| F4 | Generate Security Report |
| F5 | Change Attack Type |
| ESC | Exit Application |

---

## Architecture

```
src/
├── core/           # Data structures (Packet, TrafficWindow, Enums)
├── network/        # Network nodes (Client, Attacker, Server)
├── detection/      # Detection algorithms (Threshold, Entropy, RateLimit)
├── security/       # Firewall and mitigation
├── simulation/     # Main simulation engine
├── logging/        # Logging system
├── ui/             # FTXUI Dashboard
└── main.cpp        # Entry point
```

---

## Author

**Faruk** — OOP Final Project

## License

This project is for educational purposes only.
