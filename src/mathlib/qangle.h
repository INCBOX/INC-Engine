#pragma once

class QAngle {
public:
    float pitch, yaw, roll;

    QAngle() : pitch(0), yaw(0), roll(0) {}
    QAngle(float p, float y, float r) : pitch(p), yaw(y), roll(r) {}
};