# 🚀 NombaCrypt Shell 
**Enterprise-grade, high-throughput backend middleware for Nomba.**

[![DevCareer x Nomba Hackathon](https://img.shields.io/badge/Hackathon-DevCareer_x_Nomba-7b61ff.svg?style=flat-square)](https://devcareer.io)
[![C++ Data Plane](https://img.shields.io/badge/Engine-C%2B%2B_Data_Plane-00ff88.svg?style=flat-square)](#)
[![TypeScript Control Plane](https://img.shields.io/badge/Dashboard-Next.js_Control_Plane-3399ff.svg?style=flat-square)](#)

## 📌 The Problem We Are Solving
During university course registration weeks, standard web servers buckle under massive traffic spikes when tens of thousands of students attempt to pay their departmental dues or tuition simultaneously. Webhooks get delayed, connections drop, and students are left stranded without receipts. Furthermore, heavy cryptographic verification (like Nomba's HMAC-SHA256 signatures) in high-level languages causes severe CPU throttling during mass traffic spikes, making the congestion worse.

## ⚡ Our Solution: NombaCrypt Shell
NombaCrypt Shell is an enterprise-grade, high-throughput backend middleware built to sit between a heavily congested organization system (like a university portal) and the standard Nomba API. It acts as a high-speed **shock absorber** to prevent server crashes and an impenetrable **cryptographic shield** to prevent webhook spoofing.

### 🏗 Architecture
To maximize raw performance, we split our architecture into two distinct environments:
- **The Data Plane (C++ Backend Engine):** The invisible muscle. Written in a low-level systems language, it handles active memory buffering, multithreaded payload encryption, and microsecond signature verification. It ensures the system operates with near-zero latency, dripping transactions to Nomba exactly as fast as the API rate limits allow.
- **The Control Plane (Frontend Admin Dashboard):** The visual command center. Built with Next.js, it allows an enterprise admin to monitor live RAM buffer usage, toggle security protocols, and enable/disable specific financial modules without writing new code.

## ⚙️ The 7 Core Modules
Admins can deploy seven distinct Nomba-powered modules from the dashboard:
1. **Congestion Control:** Asynchronous transaction buffering to eliminate timeouts.
2. **Security Shield:** Multi-threaded cryptographic pipelining to verify signatures in microseconds.
3. **Dynamic NUBANs:** Automatically provisioning persistent NUBANs tied to a student's identity.
4. **Auto-Reconciler:** Automatically handling misdirected, underpaid, or overpaid bank transfers.
5. **Subscription Matrix:** A recurring-billing state machine for custom installment plans.
6. **Dunning Engine:** A sophisticated retry system for failed installment payments.
7. **Downstream Forwarder:** Instantly syncing verified payments to accounting software (like Zoho/QuickBooks) or Zapier.

## 🛡️ Security Layer Architecture
The **Security Shield** is a dedicated multithreaded interceptor within the C++ pipeline that sanitizes and verifies incoming payloads *before* they touch the backend business logic. 
- **HMAC-SHA256 Verifier:** Validates payload authenticity using Nomba Webhook secret keys, utilizing thread-local caching for a massive throughput boost.
- **Anti-Replay Ledger:** A high-speed, lock-free (sharded mutex) tracking ledger that immediately drops identical `transaction_ids` or exact payload copies to thwart replay attacks. 
- **Background TTL Eviction:** Memory leaks are prevented via an automated `std::jthread` that sweeps the ledger for expired hashes without blocking the main event loop.
- **Event Ledger Telemetry:** Emits real-time JSON `SECURITY_ALERT` events directly to the Next.js control plane via a WebSocket IPC Bridge whenever a threat is neutralized.

## 🚀 Getting Started

### Prerequisites
- Docker & Docker Compose
- Node.js (v20+)
- CMake & C++20 Compiler (for local engine builds)

### One-Click Setup
1. Clone the repository and copy the environment template:
   ```bash
   cp .env.example .env
   ```
2. Start the full stack using Docker Compose:
   ```bash
   make start
   ```
3. Open your browser to `http://localhost:3000` to access the Control Plane Dashboard.

## 🧪 High-Traffic Portal Simulator
NombaCrypt Shell includes a built-in sandbox to demonstrate performance under load. Navigate to the **Simulator** tab in the dashboard, and spawn 5,000+ concurrent requests to watch the C++ buffer swallow the traffic spike perfectly while the legacy unbuffered pipeline drops connections.

---
*Built for the DevCareer x Nomba Hackathon (July 2026).*
