# Ice Cream Factory Management System - Technical Diagnostic Tool

This project simulates a multi-threaded Ice Cream Factory production line, specifically designed to demonstrate core Operating Systems concepts for academic audit.

## Key OS Concepts Demonstrated

### 1. Mutual Exclusion (Mutexes)
- **Problem**: Concurrent access to shared variables (like `total_produced`) can cause race conditions.
- **Demonstration**: The UI tracks when threads enter a "Critical Section" (indicated by the **Red** status). A global `pthread_mutex_t` (`gui_mutex`) is used to protect the shared `FactoryState`, ensuring data integrity between worker threads and the rendering loop.

### 2. Resource Constraints (Semaphores)
- **Problem**: Finite physical resources (e.g., 3 Mixing Vats, 2 Freezers) must be shared among many workers.
- **Demonstration**: We use `sem_t` (POSIX semaphores) to manage resource pools.
    - **Vats**: `sem_init(&sem_vats, 0, 3)`
    - **Freezers**: `sem_init(&sem_freezers, 0, 2)`
- The UI displays the semaphore value as `[Available/Total]`. When a thread is **Blocked** (indicated by **Orange**), it is waiting for a semaphore to be released by another thread.

### 3. Thread Lifecycle & Scheduling
- **Demonstration**: Each production worker is a separate POSIX thread. The UI visualizes their transitions:
    - **IDLE (Gray)**: Thread is sleeping or resting between batches.
    - **BLOCKED (Orange)**: Thread is waiting for a resource. The tool tracks **Wait Time** in seconds to visualize potential starvation or scheduling delays.
    - **RUNNING (Green)**: Thread has acquired all necessary resources and is executing production logic.
    - **CRITICAL (Red)**: Thread is performing atomic updates to global counters.

### 4. System Throughput
- The dashboard calculates **Batches per Minute (BPM)** using real-time timing data, demonstrating how resource contention (visible via the Wait Counters) affects overall system performance.

## Architectural Detail: Monitor-State Pattern
To ensure graphics stability (60 FPS) without crashing the OS, the project uses a **State-Copy Snapshot** mechanism.
1. **Worker Threads** update a global struct.
2. **Main Thread** locks the mutex, `memcpy`s the state to a local variable, and unlocks.
3. **GUI** renders using only the local copy.

This pattern eliminates race conditions between the logic and the renderer, providing a stable, flicker-free diagnostic view.
