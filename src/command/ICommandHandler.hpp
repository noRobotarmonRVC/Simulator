#pragma once

#include <string>
#include "world/Grid.hpp"
#include "world/RvcState.hpp"

/// Pure interface every command handler must satisfy.
class ICommandHandler {
public:
    virtual ~ICommandHandler() = default;

    /// Execute the command; may mutate grid and/or state.
    /// Returns the response line to send back to the RVC SW (without '\n').
    virtual std::string execute(Grid& grid, RvcState& state) = 0;

    /// The uppercase verb this handler owns (e.g. "FIND_DUST").
    virtual std::string verb() const = 0;
};
