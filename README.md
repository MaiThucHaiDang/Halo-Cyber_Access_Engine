<div align="center">

# 🛡️ Halo: Cyber Access Engine

[![C++](https://img.shields.io/badge/C++-14-blue.svg?style=for-the-badge&logo=c%2B%2B)](https://isocpp.org/)
[![License](https://img.shields.io/badge/License-MIT-green.svg?style=for-the-badge)](LICENSE)
[![Build](https://img.shields.io/badge/Build-Passing-brightgreen.svg?style=for-the-badge)](https://github.com/)
[![Architecture](https://img.shields.io/badge/Architecture-x64-lightgrey.svg?style=for-the-badge)](https://github.com/)

*A High-Performance, Zero-Overhead Access Log Analysis and Threat Detection Engine designed for Enterprise Security Operations.*

</div>

---

## 📖 Overview

In modern enterprise environments, identifying security breaches and analyzing massive streams of user access logs can be a daunting challenge. **Halo** is a purpose-built Cyber Access Engine designed from the ground up to ingest, process, and analyze millions of log events with blazing speed and extreme memory efficiency. 

By strategically eliminating the overhead of standard library containers (`std::vector`, `std::map`, `std::set`) in favor of highly optimized, custom memory-managed data structures in pure C++, Halo delivers **sub-second analysis** for complex threat hunting scenarios.

Whether you are performing incident response, auditing user journeys, or identifying sophisticated cyber threats, Halo provides the raw performance required to analyze your infrastructure's footprint in real-time.

## ✨ Key Capabilities

### ⚡ Lightning-Fast Processing
- **Massive Data Ingestion:** Capable of loading and indexing millions of access log entries (80MB+ CSV files) in a fraction of a second.
- **Zero-Overhead Parsing:** Custom CSV parsing logic (`fast_stoull`) directly maps unstructured log data into contiguous memory blocks without redundant allocations or GC pauses.
- **Parallel Indexing:** Leverages multi-threading (`std::thread`) to construct pointer-based indices for User IDs and Resource IDs concurrently, maximizing CPU utilization during the preprocessing phase.

### 🕵️ Advanced Threat Detection Engine
Halo is equipped with a robust rule engine that automatically scans historical access data to detect **11 distinct categories of anomalies and cyber threats**:

**[ Identity & Access Compromise ]**
1. **Rapid Multi-Device Logins:** Identifies a single user logging in from multiple distinct devices within an impossibly short timeframe (Account Compromise / Credential Sharing).
2. **Brute-Force Detection:** Detects continuous `FAILED_LOGIN` attempts followed by eventual success or lockout.
3. **Successful Login Post-Brute-Force:** Flags instances where a user successfully authenticates immediately after a massive sequence of failed logins.

**[ Anomalous Behavior & Exfiltration ]**
4. **Resource Scraping / Probing:** Detects a single device abruptly accessing an unusually large number of distinct resources (Data Exfiltration / Network Probing).
5. **Off-Hours Operations:** Flags operations occurring strictly outside standard business hours (e.g., 18:00 to 07:00).
6. **Sleeper Account Activation:** Identifies accounts that have been dormant for over 7 days but suddenly become highly active.

**[ Geolocation Anomalies ]**
7. **Impossible Travel:** Detects users logging in from multiple distant geographic locations or countries within a timeframe that makes physical travel impossible.
8. **Proxy / VPN Hopping:** Identifies erratic and continuous geographical location changes within a single day.

**[ Session & Privilege Abuse ]**
9. **Stale Session Hijacking:** Detects unnaturally long-lasting active sessions (e.g., > 8 hours without re-authentication).
10. **High-Frequency Session Generation:** Detects automated scripts generating an abnormal number of discrete login sessions.
11. **Dangerous Action Chains:** Flags suspicious behavioral sequences, such as a privilege escalation (`ADMIN_ACTION`) immediately followed by mass data export operations (`DOWNLOAD`).

### 🔍 Interactive Threat Hunting
Beyond automated anomaly detection, Halo provides security analysts with an interactive console for manual investigation:
* **User Journey Tracing:** Map the complete lifecycle of a specific user across devices, applications, and resources over a given timeframe.
* **Resource Access Origins:** Reverse-engineer which users and devices accessed a specific critical infrastructure resource.
* **Hotspot Analysis:** Instantly extract the Top 10 most accessed resources to identify potential bottlenecks or targets.

## 🛠️ Technical Architecture

To achieve absolute maximum performance and predictability, Halo was built with strict system engineering constraints:

- **Zero Standard Containers:** The core engine actively avoids dynamic resizing containers. All data structures (dynamic arrays, index maps, `UniqueStringList`) are custom-built for the specific access patterns of security logs.
- **Pointer-Based Indexing:** Instead of physically moving large objects in memory, Halo utilizes pointer-based indexing (`UserIdx`, `DeviceIdx`, `StrIdx`) and stable sorting algorithms to create multi-dimensional views of the data in real-time.
- **Strict Memory Management:** Every byte allocated dynamically is strictly tracked and freed, ensuring zero memory leaks even under continuous, heavy analytical loads.
- **Cache Locality:** Data structures are aligned to maximize CPU L1/L2 cache hits during sequential scans.

## 🚀 Getting Started

### Prerequisites
- C++14 or higher compiler (MSVC, GCC, Clang)
- CMake or Visual Studio (Solution file `24120276.sln` provided)

### Build Instructions

**Using Visual Studio:**
1. Open the provided `24120276.sln` workspace.
2. Select the `Release` build configuration for optimal performance.
3. Build the solution (`Ctrl + Shift + B`).

**Using Command Line (MSBuild):**
```bash
msbuild 24120276.sln /p:Configuration=Release
```

### Usage
1. Place your access log CSV file named `data.csv` in the executable's root directory.
2. Launch the engine.
3. The system will automatically ingest the dataset and build concurrent indices.
4. Use the interactive menu to run Threat Detection algorithms or trace specific user/resource journeys.

## 📄 Log Data Format Specification

The engine is highly optimized to parse a specific CSV schema. Ensure your `data.csv` strictly follows this structure (no header required, or handle header appropriately):

```csv
user_id, device_id, app_id, resource_id, event_type, location, timestamp
```

**Field Types:**
- `user_id`, `device_id`, `app_id`, `resource_id`: Alphanumeric Identifiers
- `event_type`: Enumerated string (e.g., `LOGIN`, `LOGOUT`, `FAILED_LOGIN`, `ADMIN_ACTION`, `DOWNLOAD`)
- `location`: String (Country Code / Region)
- `timestamp`: Standard Epoch/Unix timestamp format (e.g., `1718640000`)

---
<div align="center">
<i>Built for security operations teams who require raw speed, zero-overhead execution, and absolute control over their data.</i>
</div>