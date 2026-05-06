#include "world/RvcState.hpp"

RvcState::RvcState(int x, int y, Direction dir)
    : m_pos{x, y}, m_dir(dir)
{}

void RvcState::rotateLeft() {
    switch (m_dir) {
        case Direction::North: m_dir = Direction::West;  break;
        case Direction::West:  m_dir = Direction::South; break;
        case Direction::South: m_dir = Direction::East;  break;
        case Direction::East:  m_dir = Direction::North; break;
    }
}

void RvcState::rotateRight() {
    switch (m_dir) {
        case Direction::North: m_dir = Direction::East;  break;
        case Direction::East:  m_dir = Direction::South; break;
        case Direction::South: m_dir = Direction::West;  break;
        case Direction::West:  m_dir = Direction::North; break;
    }
}

Point RvcState::step(Direction d) const {
    switch (d) {
        case Direction::North: return {m_pos.x,     m_pos.y - 1};
        case Direction::South: return {m_pos.x,     m_pos.y + 1};
        case Direction::East:  return {m_pos.x + 1, m_pos.y};
        case Direction::West:  return {m_pos.x - 1, m_pos.y};
    }
    return m_pos;
}

Point RvcState::cellInFront() const { return step(m_dir); }

Point RvcState::cellBehind() const {
    switch (m_dir) {
        case Direction::North: return step(Direction::South);
        case Direction::South: return step(Direction::North);
        case Direction::East:  return step(Direction::West);
        case Direction::West:  return step(Direction::East);
    }
    return m_pos;
}

Point RvcState::cellLeft() const {
    switch (m_dir) {
        case Direction::North: return step(Direction::West);
        case Direction::West:  return step(Direction::South);
        case Direction::South: return step(Direction::East);
        case Direction::East:  return step(Direction::North);
    }
    return m_pos;
}

Point RvcState::cellRight() const {
    switch (m_dir) {
        case Direction::North: return step(Direction::East);
        case Direction::East:  return step(Direction::South);
        case Direction::South: return step(Direction::West);
        case Direction::West:  return step(Direction::North);
    }
    return m_pos;
}
