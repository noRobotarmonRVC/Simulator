#include <gtest/gtest.h>
#include <mutex>
#include <thread>
#include <vector>
#include "command/CommandDispatcher.hpp"
#include "command/CommandHandlers.hpp"
#include "world/Grid.hpp"
#include "world/MotionTick.hpp"
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
        dispatcher.registerHandler(std::make_unique<DustSensorOnHandler>());
        dispatcher.registerHandler(std::make_unique<DustSensorOffHandler>());
        dispatcher.registerHandler(std::make_unique<ObstacleSensorOnHandler>());
        dispatcher.registerHandler(std::make_unique<ObstacleSensorOffHandler>());

        dispatcher.dispatch("DUST_SENSOR_ON");
        dispatcher.dispatch("OBSTACLE_SENSOR_ON");
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
    EXPECT_EQ(dispatcher.dispatch("ROTATE_LEFT"), "OK");
    motionTick(grid, state);
    EXPECT_EQ(state.direction(), Direction::North);

    EXPECT_EQ(dispatcher.dispatch("ROTATE_RIGHT"), "OK");
    motionTick(grid, state);
    EXPECT_EQ(state.direction(), Direction::East);
}

TEST_F(DispatcherFixture, MoveForwardOk) {
    // RVC at (1,1) facing East — (2,1) is passable
    EXPECT_EQ(dispatcher.dispatch("MOVE_FORWARD"), "OK");
    motionTick(grid, state);
    EXPECT_EQ(state.position().x, 2);
}

TEST_F(DispatcherFixture, MoveForwardBlocked) {
    // Face North from (1,1) — (1,0) is a wall
    dispatcher.dispatch("ROTATE_LEFT");
    motionTick(grid, state);
    EXPECT_EQ(dispatcher.dispatch("MOVE_FORWARD"), "OK");
    motionTick(grid, state);  // blocked by wall → stays at (1,1)
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

// ── FindObstacle 케이스별 테스트 ──────────────────────────────────────

// Case 1: 아무것도 없음 → (2,2) East
TEST_F(DispatcherFixture, FindObstacle_NothingBlocked) {
    dispatcher.dispatch("MOVE_FORWARD"); motionTick(grid, state); // (2,1)
    dispatcher.dispatch("ROTATE_RIGHT"); motionTick(grid, state); // face South
    dispatcher.dispatch("MOVE_FORWARD"); motionTick(grid, state); // (2,2)
    EXPECT_EQ(dispatcher.dispatch("FIND_OBSTACLE"), "OBSTACLE 0 0 0 0");
}

// Case 3: 앞 + 왼쪽 막힘 → (1,1) North
TEST_F(DispatcherFixture, FindObstacle_FrontAndLeft) {
    dispatcher.dispatch("ROTATE_LEFT"); motionTick(grid, state); // face North
    EXPECT_EQ(dispatcher.dispatch("FIND_OBSTACLE"), "OBSTACLE 1 1 0 0");
}

// Case 5: 뒤에만 막힘 → (1,2) East
TEST_F(DispatcherFixture, FindObstacle_OnlyBack) {
    dispatcher.dispatch("ROTATE_RIGHT"); motionTick(grid, state); // face South
    dispatcher.dispatch("MOVE_FORWARD"); motionTick(grid, state); // (1,2)
    dispatcher.dispatch("ROTATE_LEFT");  motionTick(grid, state); // face East
    EXPECT_EQ(dispatcher.dispatch("FIND_OBSTACLE"), "OBSTACLE 0 0 0 1");
}

// Case 2: 사방이 다 막힘 → 3x3 밀폐 방
static const char* ENCLOSED_ROOM = "###\n#.#\n###\n";

struct EnclosedFixture : ::testing::Test {
    Grid grid{Grid::fromString(ENCLOSED_ROOM)};
    RvcState state{1, 1, Direction::East};
    std::mutex mutex;
    CommandDispatcher dispatcher{grid, state, mutex};
    void SetUp() override {
        dispatcher.registerHandler(std::make_unique<FindObstacleHandler>());
        dispatcher.registerHandler(std::make_unique<ObstacleSensorOnHandler>());
        dispatcher.dispatch("OBSTACLE_SENSOR_ON");
    }
};

TEST_F(EnclosedFixture, FindObstacle_AllBlocked) {
    EXPECT_EQ(dispatcher.dispatch("FIND_OBSTACLE"), "OBSTACLE 1 1 1 1");
}

// Case 4: 앞 + 왼쪽 + 오른쪽 막힘, 뒤만 열림
// 방: row0=####, row1=####, row2=#..##, row3=####
// (2,2) East: front(3,2)=# left(2,1)=# right(2,3)=# back(1,2)=.
static const char* THREE_BLOCKED_ROOM =
    "#####\n"
    "#####\n"
    "#..##\n"
    "#####\n";

struct ThreeBlockedFixture : ::testing::Test {
    Grid grid{Grid::fromString(THREE_BLOCKED_ROOM)};
    RvcState state{2, 2, Direction::East};
    std::mutex mutex;
    CommandDispatcher dispatcher{grid, state, mutex};
    void SetUp() override {
        dispatcher.registerHandler(std::make_unique<FindObstacleHandler>());
        dispatcher.registerHandler(std::make_unique<ObstacleSensorOnHandler>());
        dispatcher.dispatch("OBSTACLE_SENSOR_ON");
    }
};

TEST_F(ThreeBlockedFixture, FindObstacle_FrontLeftRight) {
    EXPECT_EQ(dispatcher.dispatch("FIND_OBSTACLE"), "OBSTACLE 1 1 1 0");
}

// ─────────────────────────────────────────────────────────────────────

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
