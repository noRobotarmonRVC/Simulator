#include "render/Renderer.hpp"

#include <iostream>

// ANSI colour helpers
static constexpr const char* RESET   = "\033[0m";
static constexpr const char* CYAN    = "\033[36m";   // RVC
static constexpr const char* YELLOW  = "\033[33m";   // Dust
static constexpr const char* RED     = "\033[31m";   // Obstacle
static constexpr const char* DARK    = "\033[90m";   // Wall

static char directionChar(Direction d) noexcept {
    switch (d) {
        case Direction::North: return '^';
        case Direction::East:  return '>';
        case Direction::South: return 'v';
        case Direction::West:  return '<';
    }
    return '?';
}

static const char* directionName(Direction d) noexcept {
    switch (d) {
        case Direction::North: return "North";
        case Direction::East:  return "East";
        case Direction::South: return "South";
        case Direction::West:  return "West";
    }
    return "?";
}

// ─────────────────────────────────────────────────────────────────────────────

std::string Renderer::buildFrame(const Grid& grid, const RvcState& state) {
    std::string out;
    out.reserve(static_cast<std::size_t>(
        (grid.width() * 10 + 2) * grid.height() + 256));

    // ── Header ───────────────────────────────────────────────────────────────
    out += "=== RVC Simulator ===\n";

    // ── Grid ─────────────────────────────────────────────────────────────────
    const Position rvcPos = state.position();
    for (int y = 0; y < grid.height(); ++y) {
        for (int x = 0; x < grid.width(); ++x) {
            if (x == rvcPos.x && y == rvcPos.y) {
                out += CYAN;
                out += directionChar(state.direction());
                out += RESET;
                continue;
            }
            const CellType cell = grid.cellAt(x, y);
            if (cell == CellType::Wall) {
                out += DARK;
                out += '#';
                out += RESET;
            } else if (cell == CellType::Obstacle) {
                out += RED;
                out += 'O';
                out += RESET;
            } else if (grid.hasDust(x, y)) {
                out += YELLOW;
                out += '.';
                out += RESET;
            } else {
                out += ' ';
            }
        }
        out += '\n';
    }

    // ── Status panel ─────────────────────────────────────────────────────────
    out += '\n';
    out += "Position : ("
         + std::to_string(rvcPos.x) + ", "
         + std::to_string(rvcPos.y) + ")\n";
    out += std::string("Direction: ") + directionName(state.direction()) + '\n';
    out += std::string("Cleaner  : ") + (state.isCleanerOn() ? "ON" : "OFF") + '\n';
    out += std::string("Mode     : ") + (state.isBoostMode() ? "Boost"  : "Normal") + '\n';
    out += std::string("Dust left: ") + std::to_string(grid.dustCount()) + '\n';

    return out;
}

// ─────────────────────────────────────────────────────────────────────────────

Renderer::Renderer(const Grid&  grid,
                   const RvcState& state,
                   std::mutex&  worldMutex,
                   std::chrono::milliseconds refreshInterval)
    : m_grid{grid}
    , m_state{state}
    , m_mutex{worldMutex}
    , m_interval{refreshInterval}
{}

Renderer::~Renderer() {
    stop();
}

void Renderer::start() {
    m_running = true;
    m_thread  = std::thread(&Renderer::renderLoop, this);
}

void Renderer::stop() {
    m_running = false;
    if (m_thread.joinable()) {
        m_thread.join();
    }
}

void Renderer::renderLoop() {
    while (m_running) {
        // Snapshot under lock
        std::string frame;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            frame = buildFrame(m_grid, m_state);
        }

        // Move cursor to top-left and overwrite; no flicker without clear
        std::cout << "\033[H" << frame << std::flush;

        std::this_thread::sleep_for(m_interval);
    }
}
