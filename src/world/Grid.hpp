#pragma once

#include <cstdint>
#include <string>
#include <vector>

/// Physical content of a single cell in the room grid.
enum class CellType : uint8_t {
    Empty    = 0,
    Wall     = 1,
    Obstacle = 2
};

/// 2-D room grid.  Tracks cell types and dust independently.
class Grid {
public:
    Grid(int width, int height);

    int width()  const noexcept { return m_width; }
    int height() const noexcept { return m_height; }

    /// Returns the type of cell (x, y).  Asserts that (x,y) is in bounds.
    CellType cellAt(int x, int y) const;

    /// Returns true when (x, y) is in bounds AND the cell is Empty.
    bool isPassable(int x, int y) const noexcept;

    bool hasDust(int x, int y) const noexcept;
    void placeDust(int x, int y);
    void removeDust(int x, int y);
    void setCell(int x, int y, CellType type);

    /// Count of cells that still have dust.
    int dustCount() const noexcept;

    /// Build a Grid from a multi-line ASCII string.
    ///   '#' → Wall,  'O' → Obstacle,  anything else → Empty
    static Grid fromString(const std::string& layout);

private:
    int m_width{0};
    int m_height{0};
    std::vector<CellType> m_cells;
    std::vector<bool>     m_dust;

    bool inBounds(int x, int y) const noexcept;
    int  index   (int x, int y) const noexcept { return y * m_width + x; }
};
