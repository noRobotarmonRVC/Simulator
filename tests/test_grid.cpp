#include <gtest/gtest.h>
#include "world/Grid.hpp"

// ── Construction ─────────────────────────────────────────────────────────────

TEST(GridTest, DefaultCellsAreEmpty) {
    Grid g{5, 3};
    for (int y = 0; y < 3; ++y) {
        for (int x = 0; x < 5; ++x) {
            EXPECT_EQ(g.cellAt(x, y), CellType::Empty);
            EXPECT_TRUE(g.isPassable(x, y));
            EXPECT_FALSE(g.hasDust(x, y));
        }
    }
}

TEST(GridTest, DimensionsReported) {
    Grid g{10, 7};
    EXPECT_EQ(g.width(),  10);
    EXPECT_EQ(g.height(),  7);
}

// ── CellType ──────────────────────────────────────────────────────────────────

TEST(GridTest, SetWallNotPassable) {
    Grid g{3, 3};
    g.setCell(1, 1, CellType::Wall);
    EXPECT_EQ(g.cellAt(1, 1), CellType::Wall);
    EXPECT_FALSE(g.isPassable(1, 1));
}

TEST(GridTest, SetObstacleNotPassable) {
    Grid g{3, 3};
    g.setCell(2, 0, CellType::Obstacle);
    EXPECT_EQ(g.cellAt(2, 0), CellType::Obstacle);
    EXPECT_FALSE(g.isPassable(2, 0));
}

TEST(GridTest, OutOfBoundsNotPassable) {
    Grid g{3, 3};
    EXPECT_FALSE(g.isPassable(-1, 0));
    EXPECT_FALSE(g.isPassable(3,  0));
    EXPECT_FALSE(g.isPassable(0, -1));
    EXPECT_FALSE(g.isPassable(0,  3));
}

// ── Dust ──────────────────────────────────────────────────────────────────────

TEST(GridTest, PlaceAndRemoveDust) {
    Grid g{4, 4};
    EXPECT_FALSE(g.hasDust(2, 2));
    g.placeDust(2, 2);
    EXPECT_TRUE(g.hasDust(2, 2));
    g.removeDust(2, 2);
    EXPECT_FALSE(g.hasDust(2, 2));
}

TEST(GridTest, DustCountAccurate) {
    Grid g{4, 4};
    EXPECT_EQ(g.dustCount(), 0);
    g.placeDust(0, 0);
    g.placeDust(1, 1);
    EXPECT_EQ(g.dustCount(), 2);
    g.removeDust(0, 0);
    EXPECT_EQ(g.dustCount(), 1);
}

TEST(GridTest, OutOfBoundsHasDustReturnsFalse) {
    Grid g{3, 3};
    EXPECT_FALSE(g.hasDust(-1, 0));
    EXPECT_FALSE(g.hasDust(3, 3));
}

// ── fromString ────────────────────────────────────────────────────────────────

TEST(GridTest, FromStringParsesWallsAndObstacles) {
    const std::string layout =
        "#####\n"
        "#   #\n"
        "# O #\n"
        "#   #\n"
        "#####\n";

    Grid g = Grid::fromString(layout);
    EXPECT_EQ(g.width(),  5);
    EXPECT_EQ(g.height(), 5);

    // Corners are walls
    EXPECT_EQ(g.cellAt(0, 0), CellType::Wall);
    EXPECT_EQ(g.cellAt(4, 4), CellType::Wall);

    // Interior empty
    EXPECT_EQ(g.cellAt(1, 1), CellType::Empty);
    EXPECT_TRUE(g.isPassable(1, 1));

    // Obstacle at (2, 2)
    EXPECT_EQ(g.cellAt(2, 2), CellType::Obstacle);
    EXPECT_FALSE(g.isPassable(2, 2));
}
