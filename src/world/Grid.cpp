#include "world/Grid.hpp"
#include <sstream>
#include <stdexcept>

Grid::Grid(int width, int height)
    : m_width(width), m_height(height),
      m_cells(width * height, CellType::Empty),
      m_dust(width * height, false)
{}

bool Grid::inBounds(int x, int y) const {
    return x >= 0 && x < m_width && y >= 0 && y < m_height;
}

CellType Grid::cellAt(int x, int y) const {
    if (!inBounds(x, y)) return CellType::Wall;
    return m_cells[idx(x, y)];
}

bool Grid::isPassable(int x, int y) const {
    return inBounds(x, y) && m_cells[idx(x, y)] == CellType::Empty;
}

bool Grid::hasDust(int x, int y) const {
    if (!inBounds(x, y)) return false;
    return m_dust[idx(x, y)];
}

void Grid::placeDust(int x, int y) {
    if (inBounds(x, y) && m_cells[idx(x, y)] == CellType::Empty)
        m_dust[idx(x, y)] = true;
}

void Grid::removeDust(int x, int y) {
    if (inBounds(x, y))
        m_dust[idx(x, y)] = false;
}

int Grid::dustCount() const {
    int count = 0;
    for (bool d : m_dust) if (d) ++count;
    return count;
}

Grid Grid::fromString(const std::string& layout) {
    std::istringstream ss(layout);
    std::string line;
    std::vector<std::string> rows;
    while (std::getline(ss, line))
        if (!line.empty()) rows.push_back(line);

    if (rows.empty()) throw std::invalid_argument("empty layout");

    int height = static_cast<int>(rows.size());
    int width  = 0;
    for (const auto& r : rows)
        if (static_cast<int>(r.size()) > width) width = static_cast<int>(r.size());

    Grid g(width, height);
    for (int y = 0; y < height; ++y) {
        const auto& row = rows[y];
        for (int x = 0; x < static_cast<int>(row.size()); ++x) {
            char ch = row[x];
            if      (ch == '#') g.m_cells[g.idx(x, y)] = CellType::Wall;
            else if (ch == 'O') g.m_cells[g.idx(x, y)] = CellType::Obstacle;
            else if (ch == '.') g.m_dust[g.idx(x, y)]  = true;
        }
    }
    return g;
}
