#pragma once
#include <string>
#include "world/Grid.hpp"
#include "world/RvcState.hpp"

class ICommandHandler {
public:
    virtual ~ICommandHandler() = default;
    virtual std::string execute(Grid& grid, RvcState& state) = 0;
    virtual std::string verb() const = 0;
};
