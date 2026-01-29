# Discrete Event System Simulator

This repository presents a **Discrete Event System (DES) simulator** implemented in C.  
The project focuses on **event-driven modeling, modular system design, and deterministic simulation execution**.

---

## Problem Description
The objective of the project was to design and implement a simulator capable of modeling a system whose behavior evolves through **discrete events** over time.

Rather than continuous execution, the system state changes only when scheduled events occur. The simulator processes these events sequentially while maintaining system consistency and determinism.

---

## System Design

The system is composed of two main components:

### 1. System Definition (SDD)
- Defines the system states
- Describes valid state transitions
- Encapsulates system-specific logic
- Isolated from the simulation engine for modularity

### 2. Simulation Engine
- Maintains an event queue
- Processes events in chronological order
- Triggers state transitions based on event type
- Advances simulation time discretely

This separation allows the simulation engine to remain generic while the system behavior is defined independently.

---

## Event-Driven Execution Model
- Events are scheduled with associated timestamps
- The simulator repeatedly:
  1. Fetches the next event
  2. Updates the system state
  3. Schedules new events if required
- Execution terminates when the event queue is empty or a stopping condition is reached

This model ensures predictable and reproducible simulation results.

---

## Implementation Details
- Implemented entirely in **C**
- Modular design using header/source separation
- Deterministic event processing
- Clear abstraction between system logic and simulation control

---

