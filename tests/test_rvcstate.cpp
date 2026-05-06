#include <gtest/gtest.h>
#include "world/RvcState.hpp"

TEST(RvcStateTest, InitialState) {
    RvcState s(3, 4, Direction::East);
    EXPECT_EQ(s.position().x, 3);
    EXPECT_EQ(s.position().y, 4);
    EXPECT_EQ(s.direction(), Direction::East);
    EXPECT_FALSE(s.isCleanerOn());
    EXPECT_FALSE(s.isBoostMode());
}

TEST(RvcStateTest, RotateLeftCycle) {
    RvcState s(0, 0, Direction::North);
    s.rotateLeft(); EXPECT_EQ(s.direction(), Direction::West);
    s.rotateLeft(); EXPECT_EQ(s.direction(), Direction::South);
    s.rotateLeft(); EXPECT_EQ(s.direction(), Direction::East);
    s.rotateLeft(); EXPECT_EQ(s.direction(), Direction::North);
}

TEST(RvcStateTest, RotateRightCycle) {
    RvcState s(0, 0, Direction::North);
    s.rotateRight(); EXPECT_EQ(s.direction(), Direction::East);
    s.rotateRight(); EXPECT_EQ(s.direction(), Direction::South);
    s.rotateRight(); EXPECT_EQ(s.direction(), Direction::West);
    s.rotateRight(); EXPECT_EQ(s.direction(), Direction::North);
}

TEST(RvcStateTest, CellInFrontAllDirections) {
    {
        RvcState s(5, 5, Direction::North);
        EXPECT_EQ(s.cellInFront().x, 5); EXPECT_EQ(s.cellInFront().y, 4);
    }
    {
        RvcState s(5, 5, Direction::South);
        EXPECT_EQ(s.cellInFront().x, 5); EXPECT_EQ(s.cellInFront().y, 6);
    }
    {
        RvcState s(5, 5, Direction::East);
        EXPECT_EQ(s.cellInFront().x, 6); EXPECT_EQ(s.cellInFront().y, 5);
    }
    {
        RvcState s(5, 5, Direction::West);
        EXPECT_EQ(s.cellInFront().x, 4); EXPECT_EQ(s.cellInFront().y, 5);
    }
}

TEST(RvcStateTest, CellBehindOppositeOfFront) {
    RvcState s(5, 5, Direction::East);
    EXPECT_EQ(s.cellBehind().x, 4);
    EXPECT_EQ(s.cellBehind().y, 5);
}

TEST(RvcStateTest, CleanerAndBoostFlags) {
    RvcState s(0, 0, Direction::North);
    s.setCleanerOn(true);
    EXPECT_TRUE(s.isCleanerOn());
    s.setBoostMode(true);
    EXPECT_TRUE(s.isBoostMode());
    s.setCleanerOn(false);
    EXPECT_FALSE(s.isCleanerOn());
}
