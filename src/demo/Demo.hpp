#pragma once
#include "command/CommandDispatcher.hpp"

class Demo {
public:
    explicit Demo(CommandDispatcher& dispatcher);
    void run(int steps = 300);

private:
    CommandDispatcher& m_dispatcher;

    std::string send(const std::string& cmd);
};
