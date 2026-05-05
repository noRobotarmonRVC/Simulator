#include <chrono>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

#include "command/CommandDispatcher.hpp"
#include "command/CommandHandlers.hpp"
#include "demo/Demo.hpp"
#include "network/UdpServer.hpp"
#include "render/Renderer.hpp"
#include "world/Grid.hpp"
#include "world/RvcState.hpp"

// 20×10 ASCII room: # = wall, O = obstacle, . = dust, space = empty
static const char* ROOM_LAYOUT =
    "####################\n"
    "#  .  .   O  .     #\n"
    "#     O      .  O  #\n"
    "#  .     .         #\n"
    "#      O    .  O   #\n"
    "#  .      O     .  #\n"
    "#    O  .    .     #\n"
    "#  .     O      .  #\n"
    "#    .      .  O   #\n"
    "####################\n";

static volatile bool g_running = true;

static void sigHandler(int) { g_running = false; }

static void registerAllHandlers(CommandDispatcher& d) {
    d.registerHandler(std::make_unique<FindDustHandler>());
    d.registerHandler(std::make_unique<FindObstacleHandler>());
    d.registerHandler(std::make_unique<MoveForwardHandler>());
    d.registerHandler(std::make_unique<MoveBackwardHandler>());
    d.registerHandler(std::make_unique<RotateLeftHandler>());
    d.registerHandler(std::make_unique<RotateRightHandler>());
    d.registerHandler(std::make_unique<StopMotionHandler>());
    d.registerHandler(std::make_unique<CleanerOnHandler>());
    d.registerHandler(std::make_unique<CleanerOffHandler>());
    d.registerHandler(std::make_unique<BoostModeHandler>());
    d.registerHandler(std::make_unique<NormalModeHandler>());
}

static void motionTick(Grid& grid, RvcState& state) {
    switch (state.motion()) {
        case Motion::Forward: {
            auto next = state.cellInFront();
            if (grid.isPassable(next.x, next.y)) state.setPosition(next);
            else                                  state.setMotion(Motion::Idle);
            break;
        }
        case Motion::Backward: {
            auto next = state.cellBehind();
            if (grid.isPassable(next.x, next.y)) state.setPosition(next);
            else                                  state.setMotion(Motion::Idle);
            break;
        }
        case Motion::RotateLeft:  state.rotateLeft();  break;
        case Motion::RotateRight: state.rotateRight(); break;
        case Motion::Idle:                             break;
    }
}

int main(int argc, char* argv[]) {
    bool demoMode = false;
    int  port     = 9000;

    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--demo") {
            demoMode = true;
        } else {
            try { port = std::stoi(arg); }
            catch (...) {
                std::cerr << "Usage: " << argv[0] << " [--demo | <port>]\n";
                return 1;
            }
        }
    }

    Grid     grid  = Grid::fromString(ROOM_LAYOUT);
    RvcState state(1, 1, Direction::East);
    std::mutex worldMutex;

    CommandDispatcher dispatcher(grid, state, worldMutex);
    registerAllHandlers(dispatcher);

    Renderer renderer(grid, state, worldMutex);
    renderer.start();

    std::thread motionThread([&]() {
        while (g_running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            std::lock_guard<std::mutex> lock(worldMutex);
            motionTick(grid, state);
        }
    });

    if (demoMode) {
        Demo demo(dispatcher);
        demo.run(300);
    } else {
        std::signal(SIGINT, sigHandler);
        std::cout << "RVC Simulator listening on UDP port " << port << " (Ctrl+C to stop)\n";

        UdpServer server(dispatcher, port);
        server.start();

        while (g_running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }

        server.stop();
    }

    motionThread.join();

    renderer.stop();
    return 0;
}
