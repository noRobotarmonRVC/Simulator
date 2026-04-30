#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include "command/ICommandHandler.hpp"
#include "world/Grid.hpp"
#include "world/RvcState.hpp"

/// Thread-safe command dispatcher.
///
/// Owns a map of verb → handler.  Every call to dispatch() locks the shared
/// world mutex, looks up the matching handler, calls execute(), and returns
/// the response string.
class CommandDispatcher {
public:
    /// @param grid      Room grid (shared world state).
    /// @param state     RVC state (shared world state).
    /// @param mutex     External mutex that also guards the renderer reads.
    CommandDispatcher(Grid& grid, RvcState& state, std::mutex& mutex);

    /// Register a handler.  Must not be called concurrently with dispatch().
    void registerHandler(std::unique_ptr<ICommandHandler> handler);

    /// Parse @p line, find the matching handler, execute it under the mutex,
    /// and return the response (without trailing '\n').
    std::string dispatch(const std::string& line);

private:
    Grid&     m_grid;
    RvcState& m_state;
    std::mutex& m_mutex;
    std::unordered_map<std::string, std::unique_ptr<ICommandHandler>> m_handlers;
};
