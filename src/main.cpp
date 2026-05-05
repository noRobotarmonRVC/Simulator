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
    d.registerHandler(std::make_unique<CleanerOnHandler>());
    d.registerHandler(std::make_unique<CleanerOffHandler>());
    d.registerHandler(std::make_unique<BoostModeHandler>());
    d.registerHandler(std::make_unique<NormalModeHandler>());
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

    renderer.stop();
    return 0;
}
