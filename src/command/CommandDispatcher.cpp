#include "command/CommandDispatcher.hpp"
#include <algorithm>

CommandDispatcher::CommandDispatcher(Grid& grid, RvcState& state, std::mutex& mutex)
    : m_grid(grid), m_state(state), m_mutex(mutex)
{}

void CommandDispatcher::registerHandler(std::unique_ptr<ICommandHandler> handler) {
    m_handlers[handler->verb()] = std::move(handler);
}

std::string CommandDispatcher::dispatch(const std::string& line) {
    std::string verb = line;
    // strip trailing whitespace / CR
    while (!verb.empty() && (verb.back() == '\r' || verb.back() == '\n' || verb.back() == ' '))
        verb.pop_back();

    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_handlers.find(verb);
    if (it == m_handlers.end()) return "UNKNOWN_COMMAND";
    return it->second->execute(m_grid, m_state);
}
