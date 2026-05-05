#include "command/CommandHandlers.hpp"
#include <string>

std::string FindDustHandler::execute(Grid& grid, RvcState& state) {
    auto [x, y] = state.position();
    bool hasDust = grid.hasDust(x, y);
    if (hasDust && state.isCleanerOn())
        grid.removeDust(x, y);
    return hasDust ? "DUST 1" : "DUST 0";
}

std::string FindObstacleHandler::execute(Grid& grid, RvcState& state) {
    auto front = state.cellInFront();
    auto left  = state.cellLeft();
    auto right = state.cellRight();
    auto back  = state.cellBehind();
    int f = grid.isPassable(front.x, front.y) ? 0 : 1;
    int l = grid.isPassable(left.x,  left.y)  ? 0 : 1;
    int r = grid.isPassable(right.x, right.y) ? 0 : 1;
    int b = grid.isPassable(back.x,  back.y)  ? 0 : 1;
    return "OBSTACLE " + std::to_string(f) + " " + std::to_string(l) +
           " " + std::to_string(r) + " " + std::to_string(b);
}

std::string MoveForwardHandler::execute(Grid& grid, RvcState& state) {
    auto next = state.cellInFront();
    if (!grid.isPassable(next.x, next.y)) return "BLOCKED";
    state.setPosition(next);
    return "OK";
}

std::string MoveBackwardHandler::execute(Grid& grid, RvcState& state) {
    auto next = state.cellBehind();
    if (!grid.isPassable(next.x, next.y)) return "BLOCKED";
    state.setPosition(next);
    return "OK";
}

std::string RotateLeftHandler::execute(Grid& /*grid*/, RvcState& state) {
    state.rotateLeft();
    return "OK";
}

std::string RotateRightHandler::execute(Grid& /*grid*/, RvcState& state) {
    state.rotateRight();
    return "OK";
}

std::string CleanerOnHandler::execute(Grid& /*grid*/, RvcState& state) {
    state.setCleanerOn(true);
    return "OK";
}

std::string CleanerOffHandler::execute(Grid& /*grid*/, RvcState& state) {
    state.setCleanerOn(false);
    return "OK";
}

std::string BoostModeHandler::execute(Grid& /*grid*/, RvcState& state) {
    state.setBoostMode(true);
    return "OK";
}

std::string NormalModeHandler::execute(Grid& /*grid*/, RvcState& state) {
    state.setBoostMode(false);
    return "OK";
}
