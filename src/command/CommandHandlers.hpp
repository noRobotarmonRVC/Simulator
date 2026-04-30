#pragma once

#include "command/ICommandHandler.hpp"

// ─────────────────────────────────────────────────────────────────────────────
// Sensor handlers
// ─────────────────────────────────────────────────────────────────────────────

/// FIND_DUST → "DUST 1" or "DUST 0".
/// Removes dust from the current cell when the cleaner is on.
class FindDustHandler final : public ICommandHandler {
public:
    std::string execute(Grid& grid, RvcState& state) override {
        const Position pos = state.position();
        const bool found   = grid.hasDust(pos.x, pos.y);
        if (found && state.isCleanerOn()) {
            grid.removeDust(pos.x, pos.y);
        }
        return found ? "DUST 1" : "DUST 0";
    }
    std::string verb() const override { return "FIND_DUST"; }
};

/// FIND_OBSTACLE → "OBSTACLE <front> <left> <right> <back>"  (each 0 or 1)
class FindObstacleHandler final : public ICommandHandler {
public:
    std::string execute(Grid& grid, RvcState& state) override {
        auto blocked = [&](Position p) -> int {
            return grid.isPassable(p.x, p.y) ? 0 : 1;
        };
        const int front = blocked(state.cellInFront());
        const int left  = blocked(state.cellToLeft());
        const int right = blocked(state.cellToRight());
        const int back  = blocked(state.cellBehind());
        return "OBSTACLE "
             + std::to_string(front) + " "
             + std::to_string(left)  + " "
             + std::to_string(right) + " "
             + std::to_string(back);
    }
    std::string verb() const override { return "FIND_OBSTACLE"; }
};

// ─────────────────────────────────────────────────────────────────────────────
// Movement handlers
// ─────────────────────────────────────────────────────────────────────────────

/// MOVE_FORWARD → "OK" (moved) or "BLOCKED" (wall/obstacle/boundary)
class MoveForwardHandler final : public ICommandHandler {
public:
    std::string execute(Grid& grid, RvcState& state) override {
        const Position dest = state.cellInFront();
        if (!grid.isPassable(dest.x, dest.y)) return "BLOCKED";
        state.setPosition(dest);
        // Clean dust on arrival if cleaner is on
        if (state.isCleanerOn() && grid.hasDust(dest.x, dest.y)) {
            grid.removeDust(dest.x, dest.y);
        }
        return "OK";
    }
    std::string verb() const override { return "MOVE_FORWARD"; }
};

/// MOVE_BACKWARD → "OK" or "BLOCKED"
class MoveBackwardHandler final : public ICommandHandler {
public:
    std::string execute(Grid& grid, RvcState& state) override {
        const Position dest = state.cellBehind();
        if (!grid.isPassable(dest.x, dest.y)) return "BLOCKED";
        state.setPosition(dest);
        if (state.isCleanerOn() && grid.hasDust(dest.x, dest.y)) {
            grid.removeDust(dest.x, dest.y);
        }
        return "OK";
    }
    std::string verb() const override { return "MOVE_BACKWARD"; }
};

/// ROTATE_LEFT → "OK"
class RotateLeftHandler final : public ICommandHandler {
public:
    std::string execute(Grid& /*grid*/, RvcState& state) override {
        state.rotateLeft();
        return "OK";
    }
    std::string verb() const override { return "ROTATE_LEFT"; }
};

/// ROTATE_RIGHT → "OK"
class RotateRightHandler final : public ICommandHandler {
public:
    std::string execute(Grid& /*grid*/, RvcState& state) override {
        state.rotateRight();
        return "OK";
    }
    std::string verb() const override { return "ROTATE_RIGHT"; }
};

// ─────────────────────────────────────────────────────────────────────────────
// Cleaner handlers
// ─────────────────────────────────────────────────────────────────────────────

/// CLEANER_ON → "OK"
class CleanerOnHandler final : public ICommandHandler {
public:
    std::string execute(Grid& /*grid*/, RvcState& state) override {
        state.setCleanerOn(true);
        return "OK";
    }
    std::string verb() const override { return "CLEANER_ON"; }
};

/// CLEANER_OFF → "OK"
class CleanerOffHandler final : public ICommandHandler {
public:
    std::string execute(Grid& /*grid*/, RvcState& state) override {
        state.setCleanerOn(false);
        return "OK";
    }
    std::string verb() const override { return "CLEANER_OFF"; }
};

/// BOOST_MODE → "OK"
class BoostModeHandler final : public ICommandHandler {
public:
    std::string execute(Grid& /*grid*/, RvcState& state) override {
        state.setBoostMode(true);
        return "OK";
    }
    std::string verb() const override { return "BOOST_MODE"; }
};

/// NORMAL_MODE → "OK"
class NormalModeHandler final : public ICommandHandler {
public:
    std::string execute(Grid& /*grid*/, RvcState& state) override {
        state.setBoostMode(false);
        return "OK";
    }
    std::string verb() const override { return "NORMAL_MODE"; }
};
