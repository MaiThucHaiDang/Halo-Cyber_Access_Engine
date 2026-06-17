<div align="center">
  <h1>🛡️ Halo: Cyber Access Engine</h1>
  <p><i>A High-Performance, Zero-Overhead Access Log Analysis and Threat Detection Engine</i></p>
</div>

---

## 📖 Overview

In modern enterprise environments, identifying security breaches and analyzing massive streams of user access logs can be daunting. **Halo** is a purpose-built Cyber Access Engine designed to ingest, process, and analyze millions of log events with blazing speed and extreme memory efficiency. 

By eliminating the overhead of standard library containers and utilizing highly optimized, custom memory-managed data structures in pure C++, Halo delivers sub-second analysis for complex threat hunting scenarios.

## ✨ Key Capabilities

### ⚡ Lightning-Fast Processing
- **Massive Ingestion:** Capable of loading and indexing >1,000,000 access log entries in a fraction of a second.
- **Zero-Overhead Parsing:** Custom CSV parsing logic directly maps unstructured log data into actionable memory blocks without redundant allocations.

### 🕵️ Advanced Threat Detection
Halo is equipped with a robust rule engine that automatically detects various categories of anomalies and cyber threats:

* **Threshold-based Anomalies:**
  - Rapid multi-device logins (Account Compromise / Credential Sharing).
  - Continuous failed login attempts (Brute-force attacks).
  - Sudden spikes in resource access from a single device (Data exfiltration / Scraping).
  - Off-hours / abnormal access patterns.

* **Behavioral & Geolocation Anomalies:**
  - "Impossible Travel" detection (Users appearing in multiple countries within an impossible timeframe).
  - Erratic and continuous geographical location changes (Proxy/VPN hopping).

* **Session & Privilege Anomalies:**
  - Unnaturally long-lasting sessions (Stale session hijacking).
  - High-frequency session generation.
  - Dangerous action chains (e.g., privilege escalation `ADMIN_ACTION` immediately followed by mass `DOWNLOAD` operations).

* **Advanced Heuristics:**
  - Sleeper account activation (Accounts dormant for > 7 days suddenly becoming highly active).
  - Successful logins immediately following a brute-force pattern.

## 🛠️ Technical Architecture

To achieve maximum performance and predictability, Halo was built with strict engineering constraints:
- **Zero Standard Containers:** The engine avoids `std::vector`, `std::map`, or `std::set`. All data structures (dynamic arrays, index maps) are custom-built for the specific access patterns of security logs.
- **Pointer-based Indexing:** Instead of moving large objects in memory, Halo utilizes pointer-based indexing and stable sorting algorithms to create multi-dimensional views of the data in real-time.
- **Strict Memory Management:** Every byte allocated dynamically is strictly tracked and freed, ensuring zero memory leaks even under continuous heavy loads.

## 🚀 Getting Started

### Prerequisites
- C++14 or higher compiler (MSVC, GCC, Clang)
- CMake or Visual Studio (Solution file provided)

### Build & Run
If you are using Visual Studio:
1. Open `24120276.sln`.
2. Select `Release` mode for optimal performance.
3. Build the solution.
4. Provide the access log CSV file (`data.csv`) in the executable directory and run the engine.

### Usage
Once launched, Halo offers an interactive console for threat hunting:
1. Trace user journeys across devices and resources.
2. Trace resource access origins.
3. Extract Top 10 hottest resources.
4. **Execute Full Threat Detection** to scan the entire dataset for anomalies.

## 📄 Log Data Format

The engine expects a CSV file with the following column structure:
`user_id, device_id, app_id, resource_id, event_type, location, timestamp`

*(Note: Timestamps should be in standard Epoch/Unix format).*

---
*Built for security teams who need raw speed and absolute control over their data.*