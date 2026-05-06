#pragma once

enum class Direction { North, East, South, West };
enum class Motion    { Idle, Forward, Backward, RotateLeft, RotateRight };

struct Point { int x, y; };

class RvcState {
public:
    RvcState(int x, int y, Direction dir);

    Point     position()  const { return m_pos; }
    Direction direction() const { return m_dir; }

    void setPosition(Point p) { m_pos = p; }
    void setDirection(Direction d) { m_dir = d; }

    void rotateLeft();
    void rotateRight();

    Point cellInFront()  const;
    Point cellBehind()   const;
    Point cellLeft()     const;
    Point cellRight()    const;

    bool isCleanerOn()      const { return m_cleanerOn; }
    bool isBoostMode()      const { return m_boostMode; }
    bool isDustSensorOn()     const { return m_dustSensorOn; }
    bool isObstacleSensorOn() const { return m_obstacleSensorOn; }
    void setCleanerOn(bool on)          { m_cleanerOn = on; }
    void setBoostMode(bool boost)       { m_boostMode = boost; }
    void setDustSensorOn(bool on)       { m_dustSensorOn = on; }
    void setObstacleSensorOn(bool on)   { m_obstacleSensorOn = on; }

    Motion motion()           const { return m_motion; }
    void   setMotion(Motion m)      { m_motion = m; }

private:
    Point     m_pos;
    Direction m_dir;
    bool      m_cleanerOn        = false;
    bool      m_boostMode        = false;
    bool      m_dustSensorOn     = false;
    bool      m_obstacleSensorOn = false;
    Motion    m_motion           = Motion::Idle;

    Point step(Direction d) const;
};
