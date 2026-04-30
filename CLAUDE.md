# RVC Simulator — Project Guide

## What This Project Is

This is a **2D CLI Simulator** for a Robot Vacuum Cleaner (RVC), written in C++.

It is **not** the RVC embedded SW itself. The simulator acts as:
1. **Virtual hardware stub** — responds to sensor queries (dust detection, obstacle detection) and motor/cleaner commands sent by the RVC embedded SW, simulating what real hardware would do.
2. **Visualizer** — renders the current world state to the terminal in real time as a 2D grid.
3. **Communication partner** — connects to the RVC embedded SW via **TCP/UDP socket**.

The RVC embedded SW drives all logic (movement decisions, cleaning, avoidance). The simulator provides the environment and shows what is happening.

---

## Team

| Name | Student ID |
|---|---|
| 강병완 | 202211248 |
| 강현준 | 202211251 |
| 박완  | 202211301 |
| 정민수 | 202211365 |

---

## Tech Stack

| Tool | Purpose |
|---|---|
| C++ | Implementation language |
| CMake | Build management |
| GoogleTest | Unit testing |
| clang-tidy | Static code analysis |
| gcovr | Code coverage |
| GitHub Actions | CI (build → clang-tidy → test → coverage) |
| Oracle Cloud | CI server |
| TCP/UDP socket | Communication with RVC embedded SW |

---

## CLI Visualization

The terminal output consists of:
- **2D grid map** — shows the room, walls, obstacles, dust locations, and the RVC's current position and facing direction
- **Status panel** — shows cleaner power mode (normal / boost), sensor states (on/off), and other runtime info

---

## Communication Protocol (Simulator ↔ RVC Embedded SW)

The simulator listens on a TCP/UDP socket. The RVC embedded SW sends commands/queries; the simulator responds with results.

Key interactions (derived from system operations):

| RVC SW sends | Simulator responds |
|---|---|
| `findDust()` | Returns whether dust is at RVC's current position |
| `findObstacle()` | Returns obstacle presence in 4 directions: `int[4]` = [front, left, right, back] |
| `moveForward()` | Moves RVC one cell forward on grid; returns result |
| `moveBackward()` | Moves RVC one cell backward on grid |
| `rotateLeft()` | Rotates RVC 90° left |
| `rotateRight()` | Rotates RVC 90° right |
| `stop()` | Stops RVC movement |
| `turnOn()` (Cleaner) | Activates cleaner; removes dust at current cell |
| `turnOff()` (Cleaner) | Deactivates cleaner |
| `boostMode()` | Sets cleaner to boost power |
| `normalMode()` | Sets cleaner to normal power |

---

## RVC Embedded SW — Key Classes (for reference)

The RVC embedded SW (separate project) follows this architecture.
Understanding it is necessary to implement the correct simulator response behavior.

### Core Classes

```
MainSystem
  ├── RunningSystem
  │     ├── MotorController  ──→  IMotor (interface)  ──→  Motor
  │     ├── CleanerController ──→ ICleaner / BaseCleaner ──→ Cleaner
  │     └── IDeviceController ──→ BaseDevice [ Cleaner, DustSensor, ObstacleSensor ]
  │           ├── BaseDustSensor ──→ DustSensor
  │           └── BaseObstacleSensor ──→ ObstacleSensor
  └── UI
```

### Key Interfaces / Abstractions

- **`IMotor`** (interface): `moveForward()`, `moveBackward()`, `stop()`, `rotateLeft()`, `rotateRight()`
- **`BaseCleaner`** (abstract): `#power: int`, `clean(is_dust_detected)`, `boostMode()`, `normalMode()`
- **`BaseDevice`** (abstract): `#isOn: boolean`, `isOn()`, `turnOn()`, `turnOff()`
- **`BaseDustSensor`** (abstract): `#is_dust_detected: boolean`, `findDust(): boolean`
- **`BaseObstacleSensor`** (abstract): `#obstacle_info: int[4]`, `findObstacle(): int[4]`

---

## Use Cases (7 total)

| # | Name | Description |
|---|---|---|
| 1 | Power On | User powers on the RVC system |
| 2 | Power Off | User powers off the RVC system |
| 3 | Start Cleaning | RVC turns on Cleaner, DustSensor, ObstacleSensor and begins |
| 4 | Stop Cleaning | RVC turns off all devices and stops |
| 5 | Move Forward (cleaning) | While cleaning, motor moves forward if cleaner is on |
| 6 | Boost Power | If dust detected, cleaner switches to boost mode; otherwise normal mode |
| 7 | Obstacle Avoidance | If obstacle in front: stop, turn off cleaner, rotate/reverse to avoid; if surrounded: stop cleaning |

---

## Functional Requirements

| ID | Requirement | Priority |
|---|---|---|
| 1.1 | Power On | Primary |
| 1.2 | Power Off | Secondary |
| 2.1 | RVC moves forward | Primary |
| 2.2 | RVC stops movement | Primary |
| 2.3 | RVC moves backward | Secondary |
| 2.4 | RVC rotates left/right | Secondary |
| 2.5 | RVC resumes cleaning after obstacle avoidance | Secondary |
| 3.1 | Cleaning operation | Primary |
| 3.2 | Stop cleaning | Secondary |
| 3.3 | Boost cleaner power on dust detection | Secondary |

---

## Non-Functional Requirements

- **Performance**: User command → system response < 0.4s; Obstacle detected → avoidance starts < 1s; RVC must never collide with static obstacles
- **Maintainability**: Adding new sensors/hardware must not require changes to movement logic (use abstract interfaces)
- **Expandability**: Must store data needed for efficient cleaning
- **Operating Environment**: Linux-based OS
- **Interface**: Extensible interface structure required

---

## Development Notes

- `obstacle_info` is `int[4]` representing obstacle presence in `[front, left, right, back]` order
- Cleaner operates in two modes: **normal** and **boost** (switched by dust detection result)
- The simulator owns the world state (grid, RVC position/direction, dust, obstacles)
- The RVC embedded SW owns all decision logic; the simulator only reacts to commands and answers queries
