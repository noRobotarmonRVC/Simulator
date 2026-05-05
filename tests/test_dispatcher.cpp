#include <gtest/gtest.h>
#include <mutex>
#include <thread>
#include <vector>
#include "command/CommandDispatcher.hpp"
#include "command/CommandHandlers.hpp"
#include "world/Grid.hpp"
#include "world/RvcState.hpp"

static const char* SIMPLE_ROOM =
    "#####\n"
    "#...#\n"
    "#...#\n"
    "#...#\n"
    "#####\n";

struct DispatcherFixture : ::testing::Test {
    Grid grid{Grid::fromString(SIMPLE_ROOM)};
    RvcState state{1, 1, Direction::East};
    std::mutex mutex;
    CommandDispatcher dispatcher{grid, state, mutex};

    void SetUp() override {
        dispatcher.registerHandler(std::make_unique<FindDustHandler>());
        dispatcher.registerHandler(std::make_unique<FindObstacleHandler>());
        dispatcher.registerHandler(std::make_unique<MoveForwardHandler>());
        dispatcher.registerHandler(std::make_unique<MoveBackwardHandler>());
        dispatcher.registerHandler(std::make_unique<RotateLeftHandler>());
        dispatcher.registerHandler(std::make_unique<RotateRightHandler>());
        dispatcher.registerHandler(std::make_unique<CleanerOnHandler>());
        dispatcher.registerHandler(std::make_unique<CleanerOffHandler>());
        dispatcher.registerHandler(std::make_unique<BoostModeHandler>());
        dispatcher.registerHandler(std::make_unique<NormalModeHandler>());
    }
};

TEST_F(DispatcherFixture, UnknownCommandReturnsError) {
    EXPECT_EQ(dispatcher.dispatch("SELF_DESTRUCT"), "UNKNOWN_COMMAND");
}

TEST_F(DispatcherFixture, CleanerOnOff) {
    EXPECT_EQ(dispatcher.dispatch("CLEANER_ON"),  "OK");
    EXPECT_TRUE(state.isCleanerOn());
    EXPECT_EQ(dispatcher.dispatch("CLEANER_OFF"), "OK");
    EXPECT_FALSE(state.isCleanerOn());
}

TEST_F(DispatcherFixture, BoostNormalMode) {
    EXPECT_EQ(dispatcher.dispatch("BOOST_MODE"),  "OK");
    EXPECT_TRUE(state.isBoostMode());
    EXPECT_EQ(dispatcher.dispatch("NORMAL_MODE"), "OK");
    EXPECT_FALSE(state.isBoostMode());
}

TEST_F(DispatcherFixture, RotateLeftAndRight) {
    EXPECT_EQ(dispatcher.dispatch("ROTATE_LEFT"),  "OK");
    EXPECT_EQ(state.direction(), Direction::North);
    EXPECT_EQ(dispatcher.dispatch("ROTATE_RIGHT"), "OK");
    EXPECT_EQ(state.direction(), Direction::East);
}

TEST_F(DispatcherFixture, MoveForwardOk) {
    // RVC at (1,1) facing East — (2,1) is passable
    EXPECT_EQ(dispatcher.dispatch("MOVE_FORWARD"), "OK");
    EXPECT_EQ(state.position().x, 2);
}

TEST_F(DispatcherFixture, MoveForwardBlocked) {
    // Face North from (1,1) — (1,0) is a wall
    dispatcher.dispatch("ROTATE_LEFT");
    EXPECT_EQ(dispatcher.dispatch("MOVE_FORWARD"), "BLOCKED");
    EXPECT_EQ(state.position().y, 1);
}

TEST_F(DispatcherFixture, FindDustWithCleanerOff) {
    // (1,1) has dust
    EXPECT_EQ(dispatcher.dispatch("FIND_DUST"), "DUST 1");
    // Cleaner is off — dust should still be there
    EXPECT_TRUE(grid.hasDust(1, 1));
}

TEST_F(DispatcherFixture, FindDustWithCleanerOn) {
    dispatcher.dispatch("CLEANER_ON");
    EXPECT_EQ(dispatcher.dispatch("FIND_DUST"), "DUST 1");
    // Cleaner is on — dust should be removed
    EXPECT_FALSE(grid.hasDust(1, 1));
}

TEST_F(DispatcherFixture, FindObstacleResponse) {
    // (1,1) facing East: front=(2,1)=empty, left=(1,0)=wall, right=(1,2)=empty, back=(0,1)=wall
    std::string obs = dispatcher.dispatch("FIND_OBSTACLE");
    EXPECT_EQ(obs, "OBSTACLE 0 1 0 1");
}

TEST_F(DispatcherFixture, ThreadSafety) {
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&] {
            for (int j = 0; j < 50; ++j) {
                dispatcher.dispatch("CLEANER_ON");
                dispatcher.dispatch("FIND_DUST");
                dispatcher.dispatch("CLEANER_OFF");
            }
        });
    }
    for (auto& t : threads) t.join();
    // No crash = pass
}
