# 🚀 NombaCrypt Shell - Technical Documentation

**Enterprise-grade, high-throughput backend middleware for Nomba.**

Built for the **DevCareer x Nomba Hackathon**, NombaCrypt Shell is designed to act as an ultra-fast shock absorber and cryptographic shield between a high-traffic organization portal (like a university payment system) and the Nomba APIs.

---

## 🏗 System Architecture

NombaCrypt Shell is split into two distinct, highly optimized layers:

### 1. The Data Plane (C++ Backend Engine)
The core transaction engine is written entirely in C++ to achieve near-zero latency and handle massive concurrency without CPU throttling. 
* **Role:** Acts as the invisible muscle of the system.
* **Responsibilities:** 
  * Asynchronous memory buffering of incoming traffic spikes.
  * Multi-threaded payload encryption and HMAC-SHA256 signature verification.
  * Drip-feeding transactions to Nomba APIs at optimal rate limits.
* **Ports:** Listens on `8080` (HTTP) and `8081` (WebSockets for telemetry).

### 2. The Control Plane (Next.js Admin Dashboard)
A modern, React-based web dashboard that provides administrators with visual and command-line control over the C++ engine.
* **Role:** The visual command center.
* **Features:**
  * **GUI Mode:** Real-time visual telemetry, transaction streaming, and one-click toggles for system configuration.
  * **Bash Mode:** A built-in virtual terminal for advanced admins to query logs, ping the engine, and manually override configurations via commands (e.g., `set multi_api_mode=true`).
* **Ports:** Runs on `3000`.

---

## ⚙️ Core Modules & Features

### 1. Multi-API Load Balancing (BETA)
A new feature introduced to completely bypass standard rate-limiting. When enabled, the C++ engine dynamically distributes incoming traffic across multiple Nomba API channels using different sets of API keys (Primary, Channel B, Channel C). 

### 2. Congestion Control Buffer
A dedicated C++ memory buffer that catches 10,000+ simultaneous requests and holds them securely in RAM, slowly trickling them downstream to prevent timeouts and dropped connections during peak traffic hours (like university course registration deadlines).

### 3. Dynamic NUBAN Provisioning
Automatically assigns persistent, identity-tied Virtual Accounts to students, allowing them to pay via bank transfer without generating temporary accounts for every single transaction.

### 4. Auto-Reconciler
A programmatic ledger that instantly identifies and flags misdirected, underpaid, or overpaid bank transfers, eliminating manual accounting hours.

---

## 🖥 Deployment & Server Management

NombaCrypt Shell is designed to be deployed on an Ubuntu Linux server (e.g., AWS EC2).

### Prerequisites on Server
* Node.js (v20+)
* PM2 (Global install)
* Docker & Docker Compose
* Git

### Step-by-Step Deployment

**1. Clone the Repository:**
```bash
git clone https://github.com/Xrime/NombaCrypt.git
cd NombaCrypt
```

**2. Start the C++ Engine (via Docker):**
Because the Data Plane requires complex compilation, it is containerized for instant deployment.
```bash
cd backend-engine
sudo docker build -t nombacrypt-engine .
sudo docker run -d -p 8080:8080 --name backend nombacrypt-engine
```

**3. Start the Next.js Control Plane (via PM2):**
We use PM2 to keep the dashboard permanently running in the background.
```bash
cd ../UI
npm install
npm run build
sudo npm install -g pm2
pm2 start npm --name "nombacrypt-control" -- start
```

### Updating the Code on the Server
If you push new changes to GitHub from your local machine, run the following sequence on your server to apply the updates:
```bash
cd ~/NombaCrypt
git pull origin main
cd UI
npm run build
pm2 restart nombacrypt-control
```
*(Always remember to do a Hard Refresh in your browser to clear the Next.js cache).*

---

## 🛠 Shell Commands Reference

When accessing the **Bash Mode** in the Control Plane dashboard, you can use the following commands:
* `help` - Lists all available commands.
* `status` - Fetches live telemetry (Total Processed, Engine Health) from the C++ backend.
* `config` - Displays the current system configuration and API keys.
* `set [key]=[value]` - Manually update system configuration (e.g., `set multi_api_mode=true`).
* `logs` - Fetches the last 5 security/transaction events from the engine.
* `ping` - Tests latency between the Control Plane and the Data Plane.
* `clear` - Wipes the terminal screen.
