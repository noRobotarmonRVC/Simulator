#pragma once
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include "command/ICommandHandler.hpp"
#include "world/Grid.hpp"
#include "world/RvcState.hpp"

class CommandDispatcher {
public:
    CommandDispatcher(Grid& grid, RvcState& state, std::mutex& mutex);

    void registerHandler(std::unique_ptr<ICommandHandler> handler);
    std::string dispatch(const std::string& line);

private:
    Grid&     m_grid;
    RvcState& m_state;
    std::mutex& m_mutex;
    std::unordered_map<std::string, std::unique_ptr<ICommandHandler>> m_handlers;
};
