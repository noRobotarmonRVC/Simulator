#pragma once
#include <string>
#include <vector>

enum class CellType { Empty, Wall, Obstacle };

class Grid {
public:
    Grid(int width, int height);

    CellType cellAt(int x, int y) const;
    bool isPassable(int x, int y) const;
    bool hasDust(int x, int y) const;
    void placeDust(int x, int y);
    void removeDust(int x, int y);
    int  dustCount() const;
    int  width()  const { return m_width; }
    int  height() const { return m_height; }

    static Grid fromString(const std::string& layout);

private:
    int m_width;
    int m_height;
    std::vector<CellType> m_cells;
    std::vector<bool>     m_dust;

    int idx(int x, int y) const { return y * m_width + x; }
    bool inBounds(int x, int y) const;
};
