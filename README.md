# RVC Simulator

**2D CLI Simulator for a Robot Vacuum Cleaner (RVC)**  
OOAD Course Project — Konkuk University 2026, Team 2

---

## Outline

This project is **not** the RVC embedded software itself.  
It is a separate program that acts as the **virtual environment** the RVC embedded SW runs inside.

The simulator has three responsibilities:

| Role                      | Description                                                                            |
| ------------------------- | -------------------------------------------------------------------------------------- |
| **Virtual hardware stub** | Responds to sensor queries and motor/cleaner commands as real hardware would           |
| **Visualizer**            | Renders the room, RVC position, dust, and obstacles as a live 2D grid in the terminal  |
| **Communication partner** | Listens for the RVC embedded SW over a TCP socket and exchanges commands and responses |

```
┌─────────────────────┐        TCP socket        ┌─────────────────────┐
│   RVC Embedded SW   │ ──── commands/queries ──► │   RVC Simulator     │
│  (your controller)  │ ◄─── sensor responses ─── │  (this project)     │
└─────────────────────┘                           └─────────────────────┘
                                                          │
                                                   renders to terminal
                                                          │
                                                   ┌──────▼──────┐
                                                   │  2D CLI UI  │
                                                   └─────────────┘
```

---

## What This Simulator Does

### 2D Room Grid

The simulator maintains a 2D grid representing the room:

```
=== RVC Simulator ===
####################
#                  #
#  O         O     #
#         >        #      ← RVC facing East (cyan)
#         O        #
#   O              #
#              O   #
#                  #
#      O     O     #
#                  #
####################

Position : (9, 3)
Direction: East
Cleaner  : ON
Mode     : Boost
Dust left: 12
```

| Symbol    | Colour    | Meaning                      |
| --------- | --------- | ---------------------------- |
| `^ > v <` | Cyan      | RVC and its facing direction |
| `.`       | Yellow    | Dust on the floor            |
| `O`       | Red       | Fixed obstacle               |
| `#`       | Dark grey | Wall                         |
| ` `       | —         | Empty passable cell          |

### Sensor Simulation

- **Dust sensor** — reports whether dust is present at the RVC's current cell; removes it if the cleaner is on
- **Obstacle sensor** — reports obstacle presence in all 4 relative directions (front, left, right, back)

### Cleaner Simulation

- Tracks cleaner on/off state and boost/normal power mode
- Removes dust from the current cell when `FIND_DUST` is called with the cleaner on

### Motor Simulation

- Moves the RVC one cell at a time (forward, backward)
- Rotates the RVC 90° left or right
- Blocks movement into walls, obstacles, or out-of-bounds cells

---

## How It Communicates

### Protocol

The simulator listens on a **TCP socket** (default port `9000`).  
The RVC embedded SW connects as a client.

- Each **request** is a single UTF-8 line terminated with `\n`
- Each **response** is a single UTF-8 line terminated with `\n`
- Send one command, wait for the response, then send the next

### Command Table

| RVC SW sends    | Simulator responds   | Description                                            |
| --------------- | -------------------- | ------------------------------------------------------ |
| `FIND_DUST`     | `DUST 1` or `DUST 0` | Dust at current position (removes it if cleaner is on) |
| `FIND_OBSTACLE` | `OBSTACLE f l r b`   | Obstacle in each direction: `1`=blocked, `0`=clear     |
| `MOVE_FORWARD`  | `OK` or `BLOCKED`    | Move one cell forward                                  |
| `MOVE_BACKWARD` | `OK` or `BLOCKED`    | Move one cell backward                                 |
| `ROTATE_LEFT`   | `OK`                 | Rotate 90° left                                        |
| `ROTATE_RIGHT`  | `OK`                 | Rotate 90° right                                       |
| `CLEANER_ON`    | `OK`                 | Turn cleaner on                                        |
| `CLEANER_OFF`   | `OK`                 | Turn cleaner off                                       |
| `BOOST_MODE`    | `OK`                 | Switch cleaner to boost power                          |
| `NORMAL_MODE`   | `OK`                 | Switch cleaner to normal power                         |

`OBSTACLE f l r b` — four space-separated integers in **[front, left, right, back]** order relative to the RVC's current facing direction.

### Connection Details

| Property  | Value                            |
| --------- | -------------------------------- |
| Host      | `127.0.0.1` (local) or server IP |
| Port      | `9000` (default, configurable)   |
| Transport | TCP                              |
| Encoding  | UTF-8, newline-terminated lines  |
| Clients   | One at a time                    |

### Minimal Client Example (C++)

```cpp
SimulatorClient client("127.0.0.1", 9000);

// Query dust
std::string resp = client.send("FIND_DUST");   // → "DUST 1" or "DUST 0"
bool hasDust = (resp == "DUST 1");

// Query obstacles
resp = client.send("FIND_OBSTACLE");           // → "OBSTACLE 0 1 0 1"
int f, l, r, b;
sscanf(resp.c_str(), "OBSTACLE %d %d %d %d", &f, &l, &r, &b);

// Move
resp = client.send("MOVE_FORWARD");            // → "OK" or "BLOCKED"
```

---

## Core Architecture

```
src/
├── world/                   ← simulation state (no I/O)
│   ├── Grid                 ← 2D room: cell types + dust
│   └── RvcState             ← RVC position, direction, cleaner/boost flags
├── command/                 ← protocol command handlers
│   ├── ICommandHandler      ← pure interface
│   ├── CommandHandlers      ← 10 concrete handlers (one per command)
│   └── CommandDispatcher    ← thread-safe verb → handler routing
├── network/                 ← TCP server
│   ├── TcpServer            ← accept loop with select() timeout
│   └── Session              ← single client connection, read/write loop
├── render/                  ← terminal visualizer
│   └── Renderer             ← background thread, ANSI 2D grid + status panel
├── demo/
│   └── Demo                 ← built-in RVC logic for standalone demo
└── main.cpp                 ← wires all layers, seeds the map
```

### Core Classes

#### `Grid`

Owns the room layout and dust state.

| Method               | Description                                 |
| -------------------- | ------------------------------------------- |
| `cellAt(x, y)`       | Returns `Empty`, `Wall`, or `Obstacle`      |
| `isPassable(x, y)`   | `true` if in-bounds and `Empty`             |
| `hasDust(x, y)`      | Whether the cell has dust                   |
| `placeDust(x, y)`    | Place dust (used at startup)                |
| `removeDust(x, y)`   | Remove dust (called by `FindDustHandler`)   |
| `dustCount()`        | Total dust remaining in the room            |
| `fromString(layout)` | Build a grid from a multi-line ASCII string |

#### `RvcState`

Owns the robot's runtime state.

| Method                            | Description                                      |
| --------------------------------- | ------------------------------------------------ |
| `position()`                      | Current `{x, y}` coordinate                      |
| `direction()`                     | Current facing: `North`, `East`, `South`, `West` |
| `rotateLeft() / rotateRight()`    | 90° turn, updates direction                      |
| `cellInFront/Behind/Left/Right()` | Adjacent cell in each relative direction         |
| `isCleanerOn() / isBoostMode()`   | Cleaner and power mode flags                     |

#### `CommandDispatcher`

Thread-safe command router. Locks the shared world mutex for the entire duration of each command so the renderer never reads a half-updated state.

```
received line → extract verb → lock mutex → handler.execute(grid, state) → response
```

#### `Renderer`

Background thread. Every 100 ms:

1. Lock mutex → snapshot grid + state → unlock
2. Build coloured ANSI frame string
3. Write `\033[H` (cursor home) + frame → terminal overwrites without flicker

#### `TcpServer` / `Session`

`TcpServer` loops on `select()` with a 1-second timeout (so `stop()` can interrupt cleanly). For each accepted connection, a `Session` reads lines, dispatches them, and writes responses until the client disconnects.

### Thread Model

```
main thread      render thread          network thread
    │                  │                      │
    │  start()         │                      │
    ├─────────────────►│                      │
    │                  │  lock → read → unlock│
    │  server.start()  │  render frame        │
    ├──────────────────┼─────────────────────►│
    │  (blocks)        │                      │  lock → mutate → unlock
    │                  │  ◄── shared mutex ──►│
```

---

## Interface

### ICommandHandler (pure interface)

All command handlers implement this interface. Adding a new command means adding one class — no other code changes.

```cpp
class ICommandHandler {
public:
    virtual std::string execute(Grid& grid, RvcState& state) = 0;
    virtual std::string verb() const = 0;
};
```

### Registering a custom command

```cpp
class MyHandler final : public ICommandHandler {
public:
    std::string execute(Grid& grid, RvcState& state) override {
        // read or mutate grid/state here
        return "OK";
    }
    std::string verb() const override { return "MY_COMMAND"; }
};

dispatcher.registerHandler(std::make_unique<MyHandler>());
```

---

## Build & Run

### Requirements

- Linux or WSL (Windows Subsystem for Linux)
- `cmake` ≥ 3.16
- `g++` with C++17 support

### Install dependencies (once)

```bash
sudo apt-get update && sudo apt-get install -y cmake g++
```

### Build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
```

### Run — Demo mode (no RVC SW needed)

```bash
./build/rvc_simulator --demo
```

The simulator drives itself with a built-in RVC control loop. Watch the RVC navigate the room and collect dust autonomously.

### Run — Server mode (with RVC SW)

```bash
# Terminal A: start simulator
./build/rvc_simulator 9000

# Terminal B: start RVC embedded SW (connect to 127.0.0.1:9000)
./rvc_sw
```

### Run — Manual test with netcat

```bash
# Terminal A
./build/rvc_simulator 9000

# Terminal B
nc 127.0.0.1 9000
CLEANER_ON          → OK
FIND_DUST           → DUST 1
BOOST_MODE          → OK
MOVE_FORWARD        → OK
FIND_OBSTACLE       → OBSTACLE 0 1 0 1
```

### Run unit tests

```bash
ctest --test-dir build --output-on-failure
```

---

## CI/CD

GitHub Actions runs on every push and pull request:

| Step              | Tool                 |
| ----------------- | -------------------- |
| Configure & Build | CMake + g++          |
| Static Analysis   | clang-tidy           |
| Unit Tests        | GoogleTest via ctest |
| Coverage Report   | gcovr → Codecov      |
