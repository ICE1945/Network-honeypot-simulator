# 🛠️ Technology Stack

A minimal yet purpose-built stack focused on performance, control, and realistic attacker interaction.

---

## Core Implementation

- **C++**  
  High-performance systems programming for low-level control over networking, memory, and execution flow.

---

## Networking Layer

- **Winsock2 API**  
  Native Windows socket interface for TCP server implementation  
- **TCP/IP Protocol**  
  Reliable communication channel for attacker interaction simulation

---

## System Architecture Components

- **Session Handler**  
  Manages client lifecycle from connection to termination  

- **Command Processing Engine**  
  Parses attacker input and routes commands to controlled handlers  

- **Fake Filesystem Layer**  
  Simulates a believable Linux-like environment to sustain attacker engagement  

---

## Observability & Logging

- **Event Logging System**  
  Captures credentials, commands, and session activity  

- **Timestamping Mechanism**  
  Enables behavioral timeline reconstruction and forensic analysis  

---

## Design Philosophy

- Lightweight by design  
- Realistic interaction over complexity  
- Focused on behavioral data collection rather than prevention  
