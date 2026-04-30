#pragma once

#include <atomic>
#include <chrono>
#include <mutex>
#include <string>
#include <thread>

#include "world/Grid.hpp"
#include "world/RvcState.hpp"

/// Background rendering thread that draws the room grid and status panel to
/// stdout using ANSI escape codes.
///
/// Locks the shared world mutex for the minimum time needed to snapshot the
/// current state, then renders the buffered frame outside the lock.
class Renderer {
public:
    /// @param grid            Room grid (read-only access; guarded by mutex).
    /// @param state           RVC state (read-only access; guarded by mutex).
    /// @param worldMutex      Shared mutex also used by CommandDispatcher.
    /// @param refreshInterval Time between frames.
    Renderer(const Grid&  grid,
             const RvcState& state,
             std::mutex&  worldMutex,
             std::chrono::milliseconds refreshInterval = std::chrono::milliseconds{100});

    ~Renderer();

    Renderer(const Renderer&)            = delete;
    Renderer& operator=(const Renderer&) = delete;

    /// Spawn the render thread.  Call once.
    void start();

    /// Signal the render thread to stop and join it.
    void stop();

    /// Build a complete frame string from the given grid/state snapshot.
    /// Public so tests can call it without starting the background thread.
    static std::string buildFrame(const Grid& grid, const RvcState& state);

private:
    const Grid&     m_grid;
    const RvcState& m_state;
    std::mutex&     m_mutex;
    std::chrono::milliseconds m_interval;

    std::thread       m_thread;
    std::atomic<bool> m_running{false};

    void renderLoop();
};
