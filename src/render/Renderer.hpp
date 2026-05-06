#pragma once
#include <atomic>
#include <mutex>
#include <string>
#include <thread>
#include "world/Grid.hpp"
#include "world/RvcState.hpp"

class Renderer {
public:
    Renderer(Grid& grid, RvcState& state, std::mutex& mutex);
    ~Renderer();

    void start();
    void stop();

private:
    void runLoop();
    std::string buildFrame() const;

    Grid&      m_grid;
    RvcState&  m_state;
    std::mutex& m_mutex;
    std::atomic<bool> m_running{false};
    std::thread m_thread;
};
