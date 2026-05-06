#include <gtest/gtest.h>
#include "world/Grid.hpp"

TEST(GridTest, DefaultCellsAreEmpty) {
    Grid g(5, 5);
    EXPECT_EQ(g.cellAt(2, 2), CellType::Empty);
}

TEST(GridTest, OutOfBoundsIsWall) {
    Grid g(5, 5);
    EXPECT_EQ(g.cellAt(-1, 0), CellType::Wall);
    EXPECT_EQ(g.cellAt(5, 5),  CellType::Wall);
}

TEST(GridTest, IsPassableRespectsCellType) {
    Grid g(5, 5);
    EXPECT_TRUE(g.isPassable(1, 1));
    EXPECT_FALSE(g.isPassable(-1, 0));
}

TEST(GridTest, DustPlaceAndRemove) {
    Grid g(5, 5);
    EXPECT_FALSE(g.hasDust(2, 2));
    g.placeDust(2, 2);
    EXPECT_TRUE(g.hasDust(2, 2));
    g.removeDust(2, 2);
    EXPECT_FALSE(g.hasDust(2, 2));
}

TEST(GridTest, DustCount) {
    Grid g(5, 5);
    EXPECT_EQ(g.dustCount(), 0);
    g.placeDust(1, 1);
    g.placeDust(2, 2);
    EXPECT_EQ(g.dustCount(), 2);
    g.removeDust(1, 1);
    EXPECT_EQ(g.dustCount(), 1);
}

TEST(GridTest, FromStringParsesWallsObstaclesDust) {
    const std::string layout =
        "#####\n"
        "#. O#\n"
        "#####\n";
    Grid g = Grid::fromString(layout);
    EXPECT_EQ(g.cellAt(0, 0), CellType::Wall);
    EXPECT_EQ(g.cellAt(1, 1), CellType::Empty);
    EXPECT_TRUE(g.hasDust(1, 1));
    EXPECT_EQ(g.cellAt(3, 1), CellType::Obstacle);
    EXPECT_FALSE(g.isPassable(3, 1));
}

TEST(GridTest, DustNotPlacedOnWall) {
    Grid g(5, 5);
    // Can't place dust on wall via fromString
    const std::string layout = "#####\n#   #\n#####\n";
    Grid g2 = Grid::fromString(layout);
    g2.placeDust(0, 0); // wall cell — should have no effect
    EXPECT_FALSE(g2.hasDust(0, 0));
}
