#include "demo/Demo.hpp"

#include <array>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

// ── Parse helpers ─────────────────────────────────────────────────────────────

/// Parse "OBSTACLE f l r b" → array {front, left, right, back}
static std::array<int, 4> parseObstacle(const std::string& resp) {
    std::array<int, 4> obs{1, 1, 1, 1};  // default: blocked everywhere
    std::istringstream ss{resp};
    std::string tag;
    ss >> tag;  // "OBSTACLE"
    for (int i = 0; i < 4; ++i) {
        ss >> obs[static_cast<std::size_t>(i)];
    }
    return obs;
}

/// Parse "DUST x" → x
static int parseDust(const std::string& resp) {
    std::istringstream ss{resp};
    std::string tag;
    int val = 0;
    ss >> tag >> val;
    return val;
}

// ── Demo loop ─────────────────────────────────────────────────────────────────

void runDemo(Grid& grid, RvcState& /*state*/,
             std::mutex& /*worldMutex*/, CommandDispatcher& dispatcher)
{
    static constexpr int  MAX_TICKS      = 500;
    static constexpr auto TICK_INTERVAL  = std::chrono::milliseconds{200};

    // Turn devices on
    dispatcher.dispatch("CLEANER_ON");

    int  stallCount  = 0;      // consecutive ticks without forward movement
    bool turnedRight = false;  // alternate left/right when front is blocked

    for (int tick = 0; tick < MAX_TICKS; ++tick) {
        // ── 1. Check if cleaning is done ─────────────────────────────────
        if (grid.dustCount() == 0) {
            std::cout << "\n\033[32mAll dust collected! Demo complete.\033[0m\n"
                      << std::flush;
            break;
        }

        // ── 2. Sense obstacles ────────────────────────────────────────────
        const auto obs = parseObstacle(dispatcher.dispatch("FIND_OBSTACLE"));
        const int  front = obs[0];
        const int  left  = obs[1];
        const int  right = obs[2];
        const int  back  = obs[3];

        // ── 3. Navigate ───────────────────────────────────────────────────
        if (front == 0) {
            // Path ahead is clear — move forward
            dispatcher.dispatch("MOVE_FORWARD");
            stallCount = 0;
        } else {
            // Front blocked — choose a turn
            ++stallCount;

            if (left == 0 && right != 0) {
                dispatcher.dispatch("ROTATE_LEFT");
            } else if (right == 0 && left != 0) {
                dispatcher.dispatch("ROTATE_RIGHT");
            } else if (left == 0 && right == 0) {
                // Both sides free — alternate to avoid infinite spin
                if (turnedRight) {
                    dispatcher.dispatch("ROTATE_LEFT");
                    turnedRight = false;
                } else {
                    dispatcher.dispatch("ROTATE_RIGHT");
                    turnedRight = true;
                }
            } else {
                // All three directions blocked — back up if possible
                if (back == 0) {
                    dispatcher.dispatch("MOVE_BACKWARD");
                } else {
                    // Completely surrounded: stop cleaning, spin to escape
                    dispatcher.dispatch("CLEANER_OFF");
                    dispatcher.dispatch("ROTATE_RIGHT");
                    dispatcher.dispatch("ROTATE_RIGHT");
                    dispatcher.dispatch("CLEANER_ON");
                }
            }
        }

        // ── 4. Sense dust, adjust cleaner power ──────────────────────────
        const int dustHere = parseDust(dispatcher.dispatch("FIND_DUST"));
        if (dustHere != 0) {
            dispatcher.dispatch("BOOST_MODE");
        } else {
            dispatcher.dispatch("NORMAL_MODE");
        }

        // ── 5. Wait so the user can see the animation ─────────────────────
        std::this_thread::sleep_for(TICK_INTERVAL);
    }

    dispatcher.dispatch("CLEANER_OFF");
}
