#include "world/RvcState.hpp"

RvcState::RvcState(int startX, int startY, Direction dir)
    : m_pos{startX, startY}
    , m_dir{dir}
{}

void RvcState::rotateLeft() noexcept {
    // N→W→S→E→N  ≡  (dir + 3) % 4
    m_dir = static_cast<Direction>((static_cast<uint8_t>(m_dir) + 3U) % 4U);
}

void RvcState::rotateRight() noexcept {
    // N→E→S→W→N  ≡  (dir + 1) % 4
    m_dir = static_cast<Direction>((static_cast<uint8_t>(m_dir) + 1U) % 4U);
}

// Direction offsets: {dx, dy}
//   North(0): ( 0, -1)
//   East (1): (+1,  0)
//   South(2): ( 0, +1)
//   West (3): (-1,  0)
static constexpr int DX[4] = { 0, +1,  0, -1};
static constexpr int DY[4] = {-1,  0, +1,  0};

static Position offset(Position pos, Direction dir) noexcept {
    auto d = static_cast<int>(dir);
    return {pos.x + DX[d], pos.y + DY[d]};
}

// "Left of North" is West, etc. → rotate direction counterclockwise once
static Direction leftOf(Direction dir) noexcept {
    return static_cast<Direction>((static_cast<uint8_t>(dir) + 3U) % 4U);
}
static Direction rightOf(Direction dir) noexcept {
    return static_cast<Direction>((static_cast<uint8_t>(dir) + 1U) % 4U);
}
static Direction opposite(Direction dir) noexcept {
    return static_cast<Direction>((static_cast<uint8_t>(dir) + 2U) % 4U);
}

Position RvcState::cellInFront()  const noexcept { return offset(m_pos, m_dir); }
Position RvcState::cellBehind()   const noexcept { return offset(m_pos, opposite(m_dir)); }
Position RvcState::cellToLeft()   const noexcept { return offset(m_pos, leftOf(m_dir)); }
Position RvcState::cellToRight()  const noexcept { return offset(m_pos, rightOf(m_dir)); }
