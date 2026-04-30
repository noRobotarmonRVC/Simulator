#include "command/CommandDispatcher.hpp"

CommandDispatcher::CommandDispatcher(Grid& grid, RvcState& state, std::mutex& mutex)
    : m_grid{grid}
    , m_state{state}
    , m_mutex{mutex}
{}

void CommandDispatcher::registerHandler(std::unique_ptr<ICommandHandler> handler) {
    const std::string v = handler->verb();
    m_handlers[v] = std::move(handler);
}

std::string CommandDispatcher::dispatch(const std::string& line) {
    // Extract verb (first whitespace-delimited token)
    const auto spacePos = line.find(' ');
    const std::string verb =
        (spacePos == std::string::npos) ? line : line.substr(0, spacePos);

    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_handlers.find(verb);
    if (it == m_handlers.end()) {
        return "ERROR unknown command: " + verb;
    }
    return it->second->execute(m_grid, m_state);
}
