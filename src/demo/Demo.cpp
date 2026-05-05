#include "demo/Demo.hpp"
#include <chrono>
#include <string>
#include <thread>

Demo::Demo(CommandDispatcher& dispatcher) : m_dispatcher(dispatcher) {}

std::string Demo::send(const std::string& cmd) {
    return m_dispatcher.dispatch(cmd);
}

void Demo::run(int steps) {
    // Turn cleaner on at start
    send("CLEANER_ON");

    for (int i = 0; i < steps; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(150));

        // Check dust at current position
        send("FIND_DUST");

        // Check obstacles: OBSTACLE f l r b
        std::string obs = send("FIND_OBSTACLE");
        // obs = "OBSTACLE f l r b"
        int f = 0, l = 0, r = 0, b = 0;
        std::sscanf(obs.c_str(), "OBSTACLE %d %d %d %d", &f, &l, &r, &b);

        if (f == 0) {
            // Front is clear — move forward
            send("MOVE_FORWARD");
        } else if (r == 0) {
            // Turn right and move
            send("ROTATE_RIGHT");
            send("MOVE_FORWARD");
        } else if (l == 0) {
            // Turn left and move
            send("ROTATE_LEFT");
            send("MOVE_FORWARD");
        } else {
            // Trapped — rotate 180
            send("ROTATE_RIGHT");
            send("ROTATE_RIGHT");
            send("MOVE_FORWARD");
        }
    }
}
