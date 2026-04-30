#include <gtest/gtest.h>
#include "world/RvcState.hpp"

// ── Initial state ─────────────────────────────────────────────────────────────

TEST(RvcStateTest, InitialValues) {
    RvcState s{3, 5, Direction::East};
    EXPECT_EQ(s.position().x, 3);
    EXPECT_EQ(s.position().y, 5);
    EXPECT_EQ(s.direction(), Direction::East);
    EXPECT_FALSE(s.isCleanerOn());
    EXPECT_FALSE(s.isBoostMode());
}

// ── Rotation ──────────────────────────────────────────────────────────────────

TEST(RvcStateTest, RotateLeftFullCycle) {
    RvcState s{0, 0, Direction::North};
    s.rotateLeft();  EXPECT_EQ(s.direction(), Direction::West);
    s.rotateLeft();  EXPECT_EQ(s.direction(), Direction::South);
    s.rotateLeft();  EXPECT_EQ(s.direction(), Direction::East);
    s.rotateLeft();  EXPECT_EQ(s.direction(), Direction::North);
}

TEST(RvcStateTest, RotateRightFullCycle) {
    RvcState s{0, 0, Direction::North};
    s.rotateRight(); EXPECT_EQ(s.direction(), Direction::East);
    s.rotateRight(); EXPECT_EQ(s.direction(), Direction::South);
    s.rotateRight(); EXPECT_EQ(s.direction(), Direction::West);
    s.rotateRight(); EXPECT_EQ(s.direction(), Direction::North);
}

// ── Relative-position helpers ─────────────────────────────────────────────────

TEST(RvcStateTest, CellInFrontNorth) {
    RvcState s{5, 5, Direction::North};
    EXPECT_EQ(s.cellInFront(),  (Position{5, 4}));
    EXPECT_EQ(s.cellBehind(),   (Position{5, 6}));
    EXPECT_EQ(s.cellToLeft(),   (Position{4, 5}));
    EXPECT_EQ(s.cellToRight(),  (Position{6, 5}));
}

TEST(RvcStateTest, CellInFrontEast) {
    RvcState s{5, 5, Direction::East};
    EXPECT_EQ(s.cellInFront(),  (Position{6, 5}));
    EXPECT_EQ(s.cellBehind(),   (Position{4, 5}));
    EXPECT_EQ(s.cellToLeft(),   (Position{5, 4}));
    EXPECT_EQ(s.cellToRight(),  (Position{5, 6}));
}

TEST(RvcStateTest, CellInFrontSouth) {
    RvcState s{5, 5, Direction::South};
    EXPECT_EQ(s.cellInFront(),  (Position{5, 6}));
    EXPECT_EQ(s.cellBehind(),   (Position{5, 4}));
    EXPECT_EQ(s.cellToLeft(),   (Position{6, 5}));
    EXPECT_EQ(s.cellToRight(),  (Position{4, 5}));
}

TEST(RvcStateTest, CellInFrontWest) {
    RvcState s{5, 5, Direction::West};
    EXPECT_EQ(s.cellInFront(),  (Position{4, 5}));
    EXPECT_EQ(s.cellBehind(),   (Position{6, 5}));
    EXPECT_EQ(s.cellToLeft(),   (Position{5, 6}));
    EXPECT_EQ(s.cellToRight(),  (Position{5, 4}));
}

// ── Mutators ──────────────────────────────────────────────────────────────────

TEST(RvcStateTest, SetCleanerAndBoost) {
    RvcState s{0, 0};
    s.setCleanerOn(true);
    EXPECT_TRUE(s.isCleanerOn());
    s.setBoostMode(true);
    EXPECT_TRUE(s.isBoostMode());
    s.setCleanerOn(false);
    EXPECT_FALSE(s.isCleanerOn());
    EXPECT_TRUE(s.isBoostMode());   // independent
}
