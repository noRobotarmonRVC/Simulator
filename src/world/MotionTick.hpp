#pragma once
#include "world/Grid.hpp"
#include "world/RvcState.hpp"

inline void motionTick(Grid& grid, RvcState& state) {
    switch (state.motion()) {
        case Motion::Forward: {
            auto next = state.cellInFront();
            if (grid.isPassable(next.x, next.y)) state.setPosition(next);
            else                                  state.setMotion(Motion::Idle);
            break;
        }
        case Motion::Backward: {
            auto next = state.cellBehind();
            if (grid.isPassable(next.x, next.y)) state.setPosition(next);
            else                                  state.setMotion(Motion::Idle);
            break;
        }
        case Motion::RotateLeft:  state.rotateLeft();  state.setMotion(Motion::Idle); break;
        case Motion::RotateRight: state.rotateRight(); state.setMotion(Motion::Idle); break;
        case Motion::Idle:                             break;
    }
}
