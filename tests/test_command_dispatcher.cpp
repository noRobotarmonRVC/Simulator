#include <gtest/gtest.h>
#include <mutex>
#include <memory>

#include "command/CommandDispatcher.hpp"
#include "command/CommandHandlers.hpp"
#include "world/Grid.hpp"
#include "world/RvcState.hpp"

// ── Fixture ───────────────────────────────────────────────────────────────────

class DispatcherTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 5×5 room: walls on border, obstacle at (2,2)
        grid = std::make_unique<Grid>(Grid::fromString(
            "#####\n"
            "#   #\n"
            "# O #\n"
            "#   #\n"
            "#####\n"));
        grid->placeDust(1, 1);   // dust at top-left interior cell

        state = std::make_unique<RvcState>(1, 1, Direction::East);
        disp  = std::make_unique<CommandDispatcher>(*grid, *state, mtx);

        disp->registerHandler(std::make_unique<FindDustHandler>());
        disp->registerHandler(std::make_unique<FindObstacleHandler>());
        disp->registerHandler(std::make_unique<MoveForwardHandler>());
        disp->registerHandler(std::make_unique<MoveBackwardHandler>());
        disp->registerHandler(std::make_unique<RotateLeftHandler>());
        disp->registerHandler(std::make_unique<RotateRightHandler>());
        disp->registerHandler(std::make_unique<CleanerOnHandler>());
        disp->registerHandler(std::make_unique<CleanerOffHandler>());
        disp->registerHandler(std::make_unique<BoostModeHandler>());
        disp->registerHandler(std::make_unique<NormalModeHandler>());
    }

    std::mutex mtx;
    std::unique_ptr<Grid>              grid;
    std::unique_ptr<RvcState>          state;
    std::unique_ptr<CommandDispatcher> disp;
};

// ── Dust handling ─────────────────────────────────────────────────────────────

TEST_F(DispatcherTest, FindDustDetected) {
    // RVC at (1,1), dust there, cleaner OFF → reported but not removed
    EXPECT_EQ(disp->dispatch("FIND_DUST"), "DUST 1");
    EXPECT_TRUE(grid->hasDust(1, 1));   // not removed (cleaner off)
}

TEST_F(DispatcherTest, FindDustRemovedWhenCleanerOn) {
    disp->dispatch("CLEANER_ON");
    EXPECT_EQ(disp->dispatch("FIND_DUST"), "DUST 1");
    EXPECT_FALSE(grid->hasDust(1, 1));  // removed because cleaner is on
}

TEST_F(DispatcherTest, FindDustNone) {
    grid->removeDust(1, 1);
    EXPECT_EQ(disp->dispatch("FIND_DUST"), "DUST 0");
}

// ── Obstacle detection ────────────────────────────────────────────────────────

TEST_F(DispatcherTest, FindObstacleAtBoundary) {
    // RVC at (1,1) facing East
    // front=(2,1) passable, left=(1,0) wall, right=(1,2) passable, back=(0,1) wall
    const std::string resp = disp->dispatch("FIND_OBSTACLE");
    EXPECT_EQ(resp, "OBSTACLE 0 1 0 1");
}

TEST_F(DispatcherTest, FindObstacleAllClear) {
    // Move to (1,3) facing East: all neighbours are interior
    state->setPosition({1, 3});
    // front=(2,3) passable, left=(1,2) passable, right=(1,4) passable, back=(0,3) wall
    const std::string resp = disp->dispatch("FIND_OBSTACLE");
    EXPECT_EQ(resp, "OBSTACLE 0 0 0 1");
}

// ── Movement ──────────────────────────────────────────────────────────────────

TEST_F(DispatcherTest, MoveForwardOk) {
    // facing East at (1,1) → move to (2,1)
    EXPECT_EQ(disp->dispatch("MOVE_FORWARD"), "OK");
    EXPECT_EQ(state->position(), (Position{2, 1}));
}

TEST_F(DispatcherTest, MoveForwardBlocked) {
    // face the wall on the left (West = (0,1))
    disp->dispatch("ROTATE_LEFT");   // East → North
    disp->dispatch("ROTATE_LEFT");   // North → West
    EXPECT_EQ(disp->dispatch("MOVE_FORWARD"), "BLOCKED");
    EXPECT_EQ(state->position(), (Position{1, 1}));  // didn't move
}

TEST_F(DispatcherTest, MoveForwardCleansDust) {
    grid->placeDust(2, 1);
    disp->dispatch("CLEANER_ON");
    disp->dispatch("MOVE_FORWARD");
    EXPECT_FALSE(grid->hasDust(2, 1));
}

// ── Rotation ─────────────────────────────────────────────────────────────────

TEST_F(DispatcherTest, RotateLeft) {
    EXPECT_EQ(disp->dispatch("ROTATE_LEFT"), "OK");
    EXPECT_EQ(state->direction(), Direction::North);
}

TEST_F(DispatcherTest, RotateRight) {
    EXPECT_EQ(disp->dispatch("ROTATE_RIGHT"), "OK");
    EXPECT_EQ(state->direction(), Direction::South);
}

// ── Cleaner & mode ────────────────────────────────────────────────────────────

TEST_F(DispatcherTest, CleanerOnOff) {
    EXPECT_EQ(disp->dispatch("CLEANER_ON"),  "OK"); EXPECT_TRUE(state->isCleanerOn());
    EXPECT_EQ(disp->dispatch("CLEANER_OFF"), "OK"); EXPECT_FALSE(state->isCleanerOn());
}

TEST_F(DispatcherTest, BoostAndNormalMode) {
    EXPECT_EQ(disp->dispatch("BOOST_MODE"),  "OK"); EXPECT_TRUE(state->isBoostMode());
    EXPECT_EQ(disp->dispatch("NORMAL_MODE"), "OK"); EXPECT_FALSE(state->isBoostMode());
}

// ── Unknown command ───────────────────────────────────────────────────────────

TEST_F(DispatcherTest, UnknownCommandReturnsError) {
    const std::string resp = disp->dispatch("FOOBAR");
    EXPECT_TRUE(resp.rfind("ERROR", 0) == 0);
}
