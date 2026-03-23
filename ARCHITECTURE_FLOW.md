# Design Flow

This document presents the high-level workflow of the interactive honeypot system, from initial connection to session logging and termination.

```mermaid
flowchart TD
    A[Incoming TCP Connection] --> B[Client Session Accepted]
    B --> C[Fake Login Prompt Presented]
    C --> D[Username Captured]
    D --> E[Password Captured]
    E --> F[Authentication Failure Simulated]
    F --> G[Attacker Redirected to Fake Shell]

    G --> H[Command Input Received]
    H --> I[Command Parser & Handler]

    I --> J[pwd / ls / cd / cat / echo / sudo / exit]
    J --> K[Fake Filesystem Response Generated]
    K --> L[Response Sent Back to Client]

    D --> M[Credential Logging]
    E --> M
    H --> N[Command Logging]
    B --> O[Connection Logging]
    L --> P{Session Continues?}

    P -->|Yes| H
    P -->|No / exit / disconnect| Q[Session Terminated]
    Q --> R[Disconnect Event Logged]

    style A fill:#0f172a,stroke:#38bdf8,stroke-width:2px,color:#ffffff
    style B fill:#111827,stroke:#60a5fa,stroke-width:2px,color:#ffffff
    style C fill:#1e293b,stroke:#22c55e,stroke-width:2px,color:#ffffff
    style D fill:#1f2937,stroke:#f59e0b,stroke-width:2px,color:#ffffff
    style E fill:#1f2937,stroke:#f59e0b,stroke-width:2px,color:#ffffff
    style F fill:#3f1d2e,stroke:#fb7185,stroke-width:2px,color:#ffffff
    style G fill:#172554,stroke:#818cf8,stroke-width:2px,color:#ffffff
    style H fill:#0f172a,stroke:#38bdf8,stroke-width:2px,color:#ffffff
    style I fill:#1e293b,stroke:#22d3ee,stroke-width:2px,color:#ffffff
    style J fill:#111827,stroke:#a78bfa,stroke-width:2px,color:#ffffff
    style K fill:#052e16,stroke:#4ade80,stroke-width:2px,color:#ffffff
    style L fill:#052e16,stroke:#4ade80,stroke-width:2px,color:#ffffff
    style M fill:#3f3f46,stroke:#facc15,stroke-width:2px,color:#ffffff
    ````markdown


## Flow Summary

The honeypot begins by accepting an incoming TCP connection and presenting a fake authentication interface. Submitted usernames and passwords are captured and logged, after which the client is intentionally moved into a simulated shell environment. Each command is parsed, mapped to a controlled fake response, and returned to the client while command activity is recorded. The session continues until the attacker exits or disconnects, at which point the termination event is also logged.
    style N fill:#3f3f46,stroke:#facc15,stroke-width:2px,color:#ffffff
    style O fill:#3f3f46,stroke:#facc15,stroke-width:2px,color:#ffffff
    style P fill:#1f2937,stroke:#e879f9,stroke-width:2px,color:#ffffff
    style Q fill:#3f1d2e,stroke:#fb7185,stroke-width:2px,color:#ffffff
    style R fill:#3f3f46,stroke:#facc15,stroke-width:2px,color:#ffffff
