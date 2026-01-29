# Discrete Event System Simulator

This repository contains my implementation of a **multi-core scheduler simulator**
based on a **Discrete Event System (DES)** model.  
The project focuses on **concurrency, synchronization, load balancing, and cache-affinity–aware scheduling**.

---

## Problem Overview

Modern multi-core CPUs introduce challenges beyond single-core scheduling.
While classical schedulers optimize metrics such as average turnaround time
on a single core, multi-core systems must also consider:

- Load balancing across cores
- CPU utilization
- Cache affinity and cold-start penalties
- Synchronization between concurrently running schedulers

This project simulates a multi-core scheduling environment where tasks may
migrate between cores to improve utilization while accounting for the
performance cost of migration.

---

## System Architecture

The system consists of multiple simulated CPU cores, each represented by a
separate thread. Each core maintains its own task queue and cooperates with
other cores to dynamically balance workload.

### Sorted Dispatcher Database (SDD)

Each core owns a **SortedDispatcherDatabase (SDD)**, a concurrent collection
that stores tasks assigned to that core.

- Tasks are kept **sorted by remaining execution time**
- The owning core executes tasks using **Shortest Time to Completion First (STCF)**
- Other cores may steal tasks with the **largest remaining execution time**
  to reduce load imbalance

### Core Simulator Threads

Each core runs an independent scheduling loop that:

1. Fetches tasks from its own SDD
2. Steals tasks from other cores when underutilized
3. Executes tasks and updates their remaining duration
4. Resets cache affinity when a task migrates between cores
5. Terminates when all tasks are completed

---

## Event-Driven Execution Model

The simulator follows a discrete-event execution approach:

1. A core fetches a task from its SDD
2. The task is executed for a simulated time slice
3. Remaining execution time is updated using cache-affinity information
4. If unfinished, the task is reinserted into the core’s SDD
5. Execution continues until all tasks are completed

Task migration between cores resets cache affinity, simulating **cold starts**
and their performance impact.

---

## Load Balancing Strategy

To prevent idle cores and uneven workload distribution, the simulator implements
**dynamic load balancing**:

- Each core monitors the size of its own SDD
- Underutilized cores steal tasks from others using `fetchTaskFromOthers`
- Stolen tasks are selected from the **tail of another core’s SDD**
- Task ownership is transferred to ensure exclusive execution

This strategy improves CPU utilization while limiting unnecessary migrations
that could degrade cache locality.

---

## Synchronization & Concurrency

Multiple cores may access the same SDD concurrently, requiring careful
synchronization to avoid race conditions.

The implementation addresses:
- Duplicate task execution
- Lost insertions or deletions
- Queue corruption under concurrent access

Critical sections are protected using synchronization mechanisms that balance
correctness with performance.

---

## My Contribution

As part of this project, I independently implemented:

- The **SortedDispatcherDatabase (SDD)** concurrent data structure
- Thread-safe task insertion, removal, and sorted access
- The **processJobs** scheduling loop for each simulated core
- A dynamic load balancing algorithm based on queue utilization
- Task migration logic with cache-affinity reset handling
- Design analysis of synchronization and scheduling trade-offs

