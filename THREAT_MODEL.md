# Honeypot Threat Model

This document outlines the types of attacker behaviors, threat scenarios, and interaction patterns targeted by the interactive honeypot system.

---

## Threat Overview

The honeypot is designed to attract and observe unauthorized users attempting to:

- Gain system access
- Explore file structures
- Execute commands
- Escalate privileges
- Extract sensitive data

---

## Threat Interaction Flow

```mermaid
flowchart TD
    A[Attacker Connects to Honeypot] --> B[Fake Login Interface]

    B --> C[Username Attempt]
    B --> D[Password Attempt]

    C --> E[Credential Logging]
    D --> E

    E --> F[Authentication Failure Simulated]
    F --> G[Access to Fake Shell Granted]

    G --> H[Command Execution Attempt]
    H --> I[Command Monitoring and Logging]

    I --> J[File Exploration: ls, cd]
    I --> K[Data Access: cat]
    I --> L[Privilege Attempt: sudo]
    I --> M[General Commands: echo, others]

    J --> N[Behavior Analysis]
    K --> N
    L --> N
    M --> N

    N --> O[Session Profiling]
    O --> P[Threat Intelligence Collection]

    style A fill:#0f172a,stroke:#38bdf8,stroke-width:2px,color:#ffffff
    style B fill:#1e293b,stroke:#22c55e,stroke-width:2px,color:#ffffff
    style C fill:#1f2937,stroke:#f59e0b,stroke-width:2px,color:#ffffff
    style D fill:#1f2937,stroke:#f59e0b,stroke-width:2px,color:#ffffff
    style E fill:#3f3f46,stroke:#facc15,stroke-width:2px,color:#ffffff
    style F fill:#3f1d2e,stroke:#fb7185,stroke-width:2px,color:#ffffff
    style G fill:#172554,stroke:#818cf8,stroke-width:2px,color:#ffffff
    style H fill:#0f172a,stroke:#38bdf8,stroke-width:2px,color:#ffffff
    style I fill:#1e293b,stroke:#22d3ee,stroke-width:2px,color:#ffffff
    style J fill:#052e16,stroke:#4ade80,stroke-width:2px,color:#ffffff
    style K fill:#052e16,stroke:#4ade80,stroke-width:2px,color:#ffffff
    style L fill:#3f1d2e,stroke:#fb7185,stroke-width:2px,color:#ffffff
    style M fill:#111827,stroke:#a78bfa,stroke-width:2px,color:#ffffff
    style N fill:#3f3f46,stroke:#facc15,stroke-width:2px,color:#ffffff
    style O fill:#1f2937,stroke:#e879f9,stroke-width:2px,color:#ffffff
    style P fill:#052e16,stroke:#4ade80,stroke-width:2px,color:#ffffff
```

---

## Targeted Threat Behaviors

The honeypot captures and analyzes:

### Credential Attacks
- Brute-force login attempts  
- Weak password usage  
- Repeated authentication retries  

### Reconnaissance Activity
- Directory listing (`ls`)  
- Navigation (`cd`)  
- System exploration  

### Data Access Attempts
- Accessing sensitive files (`/etc/passwd`)  
- Reading private keys (`.ssh/id_rsa`)  
- Searching for valuable information  

### Command Execution Patterns
- Testing available commands  
- Checking system behavior  
- Identifying environment limitations  

### Privilege Escalation Attempts
- Use of `sudo`  
- Attempted elevation of access  

---

## Attacker Profiles

This honeypot is most effective against:

- Script kiddies  
- Automated scanning bots  
- Beginner attackers  
- Unauthorized internal users  

---

## 📊 Threat Model Summary

The honeypot does not prevent attacks — instead, it **intentionally attracts and observes malicious activity**.  

By simulating a realistic environment, it enables:

- Behavior tracking  
- Credential harvesting analysis  
- Command pattern monitoring  
- Threat intelligence generation  

This supports defensive cybersecurity strategies and real-world attack understanding.
