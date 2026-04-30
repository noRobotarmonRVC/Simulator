#pragma once

#include <cstdint>

/// Cardinal direction the RVC is facing.
enum class Direction : uint8_t {
    North = 0,  ///< negative-Y axis (up in grid)
    East  = 1,  ///< positive-X axis (right)
    South = 2,  ///< positive-Y axis (down)
    West  = 3   ///< negative-X axis (left)
};

/// 2-D grid coordinate.
struct Position {
    int x{0};
    int y{0};

    bool operator==(const Position& o) const noexcept { return x == o.x && y == o.y; }
    bool operator!=(const Position& o) const noexcept { return !(*this == o); }
};

/// All mutable state that belongs to the robot vacuum cleaner.
class RvcState {
public:
    RvcState(int startX, int startY, Direction dir = Direction::North);

    // ── Accessors ────────────────────────────────────────────────────────
    Position  position()    const noexcept { return m_pos; }
    Direction direction()   const noexcept { return m_dir; }
    bool      isCleanerOn() const noexcept { return m_cleanerOn; }
    bool      isBoostMode() const noexcept { return m_boostMode; }

    // ── Mutators ─────────────────────────────────────────────────────────
    void setPosition (Position  pos)   noexcept { m_pos       = pos;   }
    void setCleanerOn(bool      on)    noexcept { m_cleanerOn = on;    }
    void setBoostMode(bool      boost) noexcept { m_boostMode = boost; }

    void rotateLeft()  noexcept;
    void rotateRight() noexcept;

    // ── Relative-direction helpers (may return out-of-bounds positions) ──
    Position cellInFront()  const noexcept;
    Position cellBehind()   const noexcept;
    Position cellToLeft()   const noexcept;
    Position cellToRight()  const noexcept;

private:
    Position  m_pos;
    Direction m_dir;
    bool      m_cleanerOn{false};
    bool      m_boostMode{false};
};
