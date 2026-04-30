#pragma once

#include <mutex>
#include "command/CommandDispatcher.hpp"
#include "world/Grid.hpp"
#include "world/RvcState.hpp"

/// Run a self-contained RVC cleaning demo that drives CommandDispatcher
/// directly (no TCP connection needed).
///
/// The loop mimics the RVC SW logic:
///   1. Turn cleaner on.
///   2. Each tick: sense obstacles → navigate → sense dust → adjust mode.
///   3. Stop when all dust is collected or maxTicks is reached.
void runDemo(Grid& grid, RvcState& state,
             std::mutex& worldMutex, CommandDispatcher& dispatcher);
