#pragma once
#include "command/ICommandHandler.hpp"

class FindDustHandler final : public ICommandHandler {
public:
    std::string execute(Grid& grid, RvcState& state) override;
    std::string verb() const override { return "FIND_DUST"; }
};

class FindObstacleHandler final : public ICommandHandler {
public:
    std::string execute(Grid& grid, RvcState& state) override;
    std::string verb() const override { return "FIND_OBSTACLE"; }
};

class MoveForwardHandler final : public ICommandHandler {
public:
    std::string execute(Grid& grid, RvcState& state) override;
    std::string verb() const override { return "MOVE_FORWARD"; }
};

class MoveBackwardHandler final : public ICommandHandler {
public:
    std::string execute(Grid& grid, RvcState& state) override;
    std::string verb() const override { return "MOVE_BACKWARD"; }
};

class RotateLeftHandler final : public ICommandHandler {
public:
    std::string execute(Grid& grid, RvcState& state) override;
    std::string verb() const override { return "ROTATE_LEFT"; }
};

class RotateRightHandler final : public ICommandHandler {
public:
    std::string execute(Grid& grid, RvcState& state) override;
    std::string verb() const override { return "ROTATE_RIGHT"; }
};

class CleanerOnHandler final : public ICommandHandler {
public:
    std::string execute(Grid& grid, RvcState& state) override;
    std::string verb() const override { return "CLEANER_ON"; }
};

class CleanerOffHandler final : public ICommandHandler {
public:
    std::string execute(Grid& grid, RvcState& state) override;
    std::string verb() const override { return "CLEANER_OFF"; }
};

class BoostModeHandler final : public ICommandHandler {
public:
    std::string execute(Grid& grid, RvcState& state) override;
    std::string verb() const override { return "BOOST_MODE"; }
};

class NormalModeHandler final : public ICommandHandler {
public:
    std::string execute(Grid& grid, RvcState& state) override;
    std::string verb() const override { return "NORMAL_MODE"; }
};

class StopMotionHandler final : public ICommandHandler {
public:
    std::string execute(Grid& grid, RvcState& state) override;
    std::string verb() const override { return "STOP_MOTOR"; }
};

class DustSensorOnHandler final : public ICommandHandler {
public:
    std::string execute(Grid& grid, RvcState& state) override;
    std::string verb() const override { return "DUST_SENSOR_ON"; }
};

class DustSensorOffHandler final : public ICommandHandler {
public:
    std::string execute(Grid& grid, RvcState& state) override;
    std::string verb() const override { return "DUST_SENSOR_OFF"; }
};

class ObstacleSensorOnHandler final : public ICommandHandler {
public:
    std::string execute(Grid& grid, RvcState& state) override;
    std::string verb() const override { return "OBSTACLE_SENSOR_ON"; }
};

class ObstacleSensorOffHandler final : public ICommandHandler {
public:
    std::string execute(Grid& grid, RvcState& state) override;
    std::string verb() const override { return "OBSTACLE_SENSOR_OFF"; }
};
