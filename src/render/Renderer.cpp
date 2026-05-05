#include "render/Renderer.hpp"
#include <chrono>
#include <cstdio>
#include <sstream>
#include <string>
#include <thread>

// ANSI colour helpers
static const char* RESET    = "\033[0m";
static const char* CYAN     = "\033[36m";
static const char* YELLOW   = "\033[33m";
static const char* RED      = "\033[31m";
static const char* DARK_GREY= "\033[90m";

Renderer::Renderer(Grid& grid, RvcState& state, std::mutex& mutex)
    : m_grid(grid), m_state(state), m_mutex(mutex)
{}

Renderer::~Renderer() {
    stop();
}

void Renderer::start() {
    m_running = true;
    m_thread = std::thread(&Renderer::runLoop, this);
}

void Renderer::stop() {
    m_running = false;
    if (m_thread.joinable()) m_thread.join();
}

void Renderer::runLoop() {
    while (m_running) {
        std::string frame = buildFrame();
        // cursor home + clear screen
        std::fputs("\033[H\033[2J", stdout);
        std::fputs(frame.c_str(), stdout);
        std::fflush(stdout);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

std::string Renderer::buildFrame() const {
    // Snapshot under lock
    Grid      gridSnap  = [&]{ std::lock_guard<std::mutex> lk(m_mutex); return m_grid; }();
    RvcState  stateSnap = [&]{ std::lock_guard<std::mutex> lk(m_mutex); return m_state; }();

    auto [rx, ry] = stateSnap.position();
    Direction dir = stateSnap.direction();

    char rvcChar = '?';
    switch (dir) {
        case Direction::North: rvcChar = '^'; break;
        case Direction::East:  rvcChar = '>'; break;
        case Direction::South: rvcChar = 'v'; break;
        case Direction::West:  rvcChar = '<'; break;
    }

    std::ostringstream oss;
    oss << "=== RVC Simulator ===\n";

    for (int y = 0; y < gridSnap.height(); ++y) {
        for (int x = 0; x < gridSnap.width(); ++x) {
            if (x == rx && y == ry) {
                oss << CYAN << rvcChar << RESET;
            } else {
                CellType ct = gridSnap.cellAt(x, y);
                if (ct == CellType::Wall) {
                    oss << DARK_GREY << '#' << RESET;
                } else if (ct == CellType::Obstacle) {
                    oss << RED << 'O' << RESET;
                } else if (gridSnap.hasDust(x, y)) {
                    oss << YELLOW << '.' << RESET;
                } else {
                    oss << ' ';
                }
            }
        }
        oss << '\n';
    }

    oss << '\n';
    oss << "Position : (" << rx << ", " << ry << ")\n";

    const char* dirName = "?";
    switch (dir) {
        case Direction::North: dirName = "North"; break;
        case Direction::East:  dirName = "East";  break;
        case Direction::South: dirName = "South"; break;
        case Direction::West:  dirName = "West";  break;
    }
    oss << "Direction: " << dirName << "\n";
    oss << "Cleaner  : " << (stateSnap.isCleanerOn() ? "ON" : "OFF");
    if (stateSnap.isCleanerOn())
        oss << (stateSnap.isBoostMode() ? "  [BOOST]" : "  [NORMAL]");
    oss << "\n";
    oss << "Dust left: " << gridSnap.dustCount() << "\n";

    return oss.str();
}
