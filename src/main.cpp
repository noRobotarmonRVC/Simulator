#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <mutex>
#include <random>
#include <string>

#include "command/CommandDispatcher.hpp"
#include "command/CommandHandlers.hpp"
#include "demo/Demo.hpp"
#include "network/TcpServer.hpp"
#include "render/Renderer.hpp"
#include "world/Grid.hpp"
#include "world/RvcState.hpp"

// ── Default room layout ───────────────────────────────────────────────────
//  '#' = wall, 'O' = obstacle, ' ' = empty
static const std::string DEFAULT_MAP =
    "####################\n"
    "#                  #\n"
    "#  O         O     #\n"
    "#                  #\n"
    "#         O        #\n"
    "#   O              #\n"
    "#              O   #\n"
    "#                  #\n"
    "#      O     O     #\n"
    "#                  #\n"
    "####################\n";

/// Seed ~20 % of passable cells with dust, skipping the RVC start position.
static void seedDust(Grid& grid, const Position& skip, unsigned seed = 42) {
    std::mt19937 rng{seed};
    std::uniform_real_distribution<float> chance{0.0F, 1.0F};

    for (int y = 0; y < grid.height(); ++y) {
        for (int x = 0; x < grid.width(); ++x) {
            if (!grid.isPassable(x, y)) continue;
            if (x == skip.x && y == skip.y) continue;
            if (chance(rng) < 0.20F) {
                grid.placeDust(x, y);
            }
        }
    }
}

/// Register all command handlers into the dispatcher.
static void registerHandlers(CommandDispatcher& d) {
    d.registerHandler(std::make_unique<FindDustHandler>());
    d.registerHandler(std::make_unique<FindObstacleHandler>());
    d.registerHandler(std::make_unique<MoveForwardHandler>());
    d.registerHandler(std::make_unique<MoveBackwardHandler>());
    d.registerHandler(std::make_unique<RotateLeftHandler>());
    d.registerHandler(std::make_unique<RotateRightHandler>());
    d.registerHandler(std::make_unique<CleanerOnHandler>());
    d.registerHandler(std::make_unique<CleanerOffHandler>());
    d.registerHandler(std::make_unique<BoostModeHandler>());
    d.registerHandler(std::make_unique<NormalModeHandler>());
}

int main(int argc, char* argv[]) {
    // ── Parse arguments ───────────────────────────────────────────────────
    bool     demoMode = false;
    uint16_t port     = 9000;

    for (int i = 1; i < argc; ++i) {
        const std::string arg{argv[i]};
        if (arg == "--demo") {
            demoMode = true;
        } else {
            const int p = std::atoi(argv[i]);
            if (p > 0 && p < 65536) {
                port = static_cast<uint16_t>(p);
            }
        }
    }

    // ── Build world ───────────────────────────────────────────────────────
    Grid     grid  = Grid::fromString(DEFAULT_MAP);
    RvcState state {2, 1, Direction::East};
    seedDust(grid, state.position());

    std::mutex        worldMutex;
    CommandDispatcher dispatcher{grid, state, worldMutex};
    registerHandlers(dispatcher);

    // ── Clear screen; start renderer ──────────────────────────────────────
    std::cout << "\033[2J\033[H" << std::flush;
    Renderer renderer{grid, state, worldMutex, std::chrono::milliseconds{100}};
    renderer.start();

    // ── Demo mode: built-in RVC logic, no TCP needed ──────────────────────
    if (demoMode) {
        // Print label below the grid
        std::cout << "\033[" << (grid.height() + 2) << ";1H"
                  << "\033[33m[DEMO MODE]\033[0m  "
                     "Watch the RVC clean the room automatically.\n"
                  << std::flush;
        runDemo(grid, state, worldMutex, dispatcher);
        std::this_thread::sleep_for(std::chrono::seconds{2});
        renderer.stop();
        return 0;
    }

    // ── Server mode: wait for RVC SW over TCP ────────────────────────────
    std::cout << "\033[" << (grid.height() + 2) << ";1H"
              << "Listening on port " << port
              << " \xe2\x80\x94 waiting for RVC SW connection...\n"
              << std::flush;

    TcpServer server{port, [&dispatcher](const std::string& line) {
        return dispatcher.dispatch(line);
    }};
    server.start();   // blocks until Ctrl-C

    renderer.stop();
    return 0;
}
