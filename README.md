<div align="center">

<img src="assets/ice-factory-hero.svg" alt="OS Ice Cream Factory" width="100%" />

# OS Ice Cream Factory

A C and POSIX threads simulation of an ice cream production line with a live raylib diagnostic dashboard.

</div>

## Main Demonstration

<video src="main.webm" controls muted width="100%"></video>

[Open the main demonstration video](main.webm)

An additional recording is available in [second.webm](second.webm).

## Overview

This project models a factory pipeline with three stages:

```text
MIXERS  →  FREEZERS  →  PACKAGERS  →  FINISHED BATCHES
```

Worker threads compete for limited resources while the GUI displays their current state, progress, resource availability, production count, efficiency, and audit log.

## Features

- 5 mixer threads, 5 freezer threads, and 5 packager threads
- POSIX threads with independent worker lifecycles
- Semaphores for vats, freezers, packaging stations, and pipeline handoff
- Mutex-protected shared factory state
- 60 FPS raylib diagnostic dashboard
- Worker statuses: `READY`, `WAITING`, `DISPATCHING`, `RUNNING`, and `CRITICAL_SECTION`
- Resource utilization indicators
- Total production and batches-per-minute metrics
- Rolling narrative audit log
- State snapshot before rendering to keep GUI access separate from worker updates

## OS Concepts Demonstrated

| Concept | Where it appears |
| --- | --- |
| **Threads** | Mixer, freezer, and packager workers run concurrently with the GUI loop. |
| **Mutex** | `gui_mutex` protects `global_state` and log updates. |
| **Semaphores** | Resource pools limit vats, freezers, and packaging stations. |
| **Pipeline synchronization** | `sem_ready_to_freeze` and `sem_ready_to_pack` connect production stages. |
| **Critical sections** | Workers lock shared state while updating counters and resource values. |
| **Scheduling visibility** | Waiting and dispatch states make contention visible in the dashboard. |
| **Snapshot pattern** | The main thread copies state under a lock, then renders the local copy. |

## Build and Run

### Requirements

- Linux or another POSIX-compatible environment
- GCC
- `make`
- raylib development files
- POSIX threads, OpenGL, X11, math, `dl`, and realtime libraries

The root Makefile links:

```text
-lraylib -lpthread -lGL -lm -ldl -lrt -lX11
```

### Build the main dashboard

From the project root:

```bash
make
./ice_factory
```

Clean generated objects and the main binary with:

```bash
make clean
```

The default target builds:

```text
src/main.c
src/factory.c
src/gui.c
```

### Install raylib on Debian/Ubuntu

Package names vary by distribution. On systems where the packages are available:

```bash
sudo apt update
sudo apt install build-essential make libraylib-dev
```

If `libraylib-dev` is not available, install raylib from its official source or package manager, then rerun `make`.

## Dashboard

The main window is approximately `1200 x 950` at 60 FPS and contains:

- Mixer, freezer, and packager worker panels
- Status colors and progress bars
- Semaphore/resource monitor
- Status legend
- Production and BPM dashboard
- Narrative diagnostic audit log

### Status colors

| Status | Meaning |
| --- | --- |
| Gray | Ready or idle |
| Green | Worker is executing a production stage |
| Red | Worker is waiting for a pipeline/resource condition |
| Yellow | Worker is in a dispatch/acquisition delay |

## Production Pipeline

### Mixer stage

- Waits for one of three vats.
- Performs a simulated five-second mixing cycle.
- Releases the vat.
- Signals a freezer through `sem_ready_to_freeze`.

### Freezer stage

- Waits for a completed mixer batch.
- Waits for one of two freezer slots.
- Performs a simulated eight-second cooling cycle.
- Releases the slot.
- Signals a packager through `sem_ready_to_pack`.

### Packager stage

- Waits for a completed freezer batch.
- Waits for one of two packaging stations.
- Performs a simulated three-second packaging cycle.
- Increments `total_produced`.
- Updates batches per minute.

## Project Structure

```text
OS_ICECREAM_FACTORY/
├── README.md                    # Project documentation
├── assets/
│   └── ice-factory-hero.svg     # Project banner artwork
├── Makefile                     # Main GCC build rules
├── main.webm                    # Main project demonstration video
├── second.webm                  # Additional project recording
├── include/
│   └── factory.h                # Shared state, enums, constants, declarations
├── src/
│   ├── main.c                   # Raylib window, semaphores, threads, render loop
│   ├── factory.c                # Worker logic and synchronization
│   └── gui.c                    # Diagnostic dashboard rendering
├── thread/
│   ├── ser.c                    # Standalone semaphore/thread experiment
│   ├── cli.c                    # Additional CLI experiment
│   └── ...                      # Built variants and supporting files
├── tcp/
│   ├── Finalserver.c            # TCP server experiment
│   ├── cli1.c, cli2.c, ...      # TCP client experiments
│   └── ...                      # Built variants and supporting files
├── raylib/                      # Bundled raylib source and examples
├── icefacv1.c                   # Earlier standalone thread prototype
├── ice_factory                  # Existing main dashboard binary, if present
├── ice_sim                      # Existing simulation artifact, if present
└── icefacv1                     # Existing prototype artifact, if present
```

The recommended application is the root Makefile target `ice_factory`. The `thread/`, `tcp/`, and `icefacv1.c` files are supporting experiments and earlier variants rather than inputs to the main build.

## Shared State

`FactoryState` in [`include/factory.h`](include/factory.h) contains:

- Worker records and statuses
- Available vats, freezers, and packaging stations
- Total production count
- Start time and batches per minute
- Current mutex owner/lock state
- Recent log messages

The worker count is capped by:

```c
#define MAX_WORKERS 15
#define MAX_LOG_ENTRIES 10
```

## Troubleshooting

### `raylib.h: No such file or directory`

Install raylib development headers or update the compiler include path. The Makefile currently expects raylib to be discoverable by GCC.

### `cannot find -lraylib`

Install raylib's development library and ensure its library directory is in the linker search path.

### `pthread` or semaphore linker errors

Build through the supplied Makefile so `-lpthread` and the other required system libraries are included.

### The window opens but the dashboard is empty

Allow the worker threads to run for a few seconds. The pipeline intentionally includes delays while workers acquire resources and complete each stage.

### Closing the window

The main loop exits when raylib reports that the window should close. The current worker threads are detached and run continuously while the process is alive.

## Notes

- The application is a simulation, not a real manufacturing controller.
- Timing values are intentionally visible so scheduling and contention can be observed.
- The dashboard uses a copied state snapshot for rendering instead of reading worker-mutated state directly.
- The bundled `raylib/` directory is a third-party dependency tree and is not part of the project-specific source summary.

## License

No license file is currently included. Add an appropriate license before distributing the project.
