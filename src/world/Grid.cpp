#include "world/Grid.hpp"

#include <algorithm>
#include <cassert>
#include <numeric>
#include <sstream>

Grid::Grid(int width, int height)
    : m_width{width}
    , m_height{height}
    , m_cells(static_cast<std::size_t>(width * height), CellType::Empty)
    , m_dust (static_cast<std::size_t>(width * height), false)
{
    assert(width > 0 && height > 0);
}

bool Grid::inBounds(int x, int y) const noexcept {
    return x >= 0 && x < m_width && y >= 0 && y < m_height;
}

CellType Grid::cellAt(int x, int y) const {
    assert(inBounds(x, y));
    return m_cells[static_cast<std::size_t>(index(x, y))];
}

bool Grid::isPassable(int x, int y) const noexcept {
    return inBounds(x, y) && m_cells[static_cast<std::size_t>(index(x, y))] == CellType::Empty;
}

bool Grid::hasDust(int x, int y) const noexcept {
    if (!inBounds(x, y)) return false;
    return m_dust[static_cast<std::size_t>(index(x, y))];
}

void Grid::placeDust(int x, int y) {
    assert(inBounds(x, y));
    m_dust[static_cast<std::size_t>(index(x, y))] = true;
}

void Grid::removeDust(int x, int y) {
    assert(inBounds(x, y));
    m_dust[static_cast<std::size_t>(index(x, y))] = false;
}

void Grid::setCell(int x, int y, CellType type) {
    assert(inBounds(x, y));
    m_cells[static_cast<std::size_t>(index(x, y))] = type;
}

int Grid::dustCount() const noexcept {
    int cnt = 0;
    for (bool d : m_dust) {
        if (d) ++cnt;
    }
    return cnt;
}

Grid Grid::fromString(const std::string& layout) {
    std::vector<std::string> rows;
    std::string row;
    for (char c : layout) {
        if (c == '\n') {
            if (!row.empty()) {
                rows.push_back(row);
                row.clear();
            }
        } else {
            row += c;
        }
    }
    if (!row.empty()) rows.push_back(row);

    if (rows.empty()) return Grid{1, 1};

    int height = static_cast<int>(rows.size());
    int width  = 0;
    for (const auto& r : rows) {
        width = std::max(width, static_cast<int>(r.size()));
    }

    Grid g{width, height};
    for (int y = 0; y < height; ++y) {
        const auto& r = rows[static_cast<std::size_t>(y)];
        for (int x = 0; x < static_cast<int>(r.size()); ++x) {
            char c = r[static_cast<std::size_t>(x)];
            if (c == '#') {
                g.setCell(x, y, CellType::Wall);
            } else if (c == 'O') {
                g.setCell(x, y, CellType::Obstacle);
            }
            // else: remains Empty
        }
    }
    return g;
}
