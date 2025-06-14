#include "player.h"
#include <iostream>

CPlayer::CPlayer() : m_vecOrigin(0, 0, 0), m_angView(0, 0, 0) {}

void CPlayer::Spawn() {
    m_vecOrigin = Vector(0, 0, 0);
    m_angView = QAngle(0, 0, 0);
    std::cout << "[Player] Spawned at " << m_vecOrigin.x << ", " << m_vecOrigin.y << ", " << m_vecOrigin.z << std::endl;
}

void CPlayer::Think(float dt) {
    // Movement or look logic here
}

void CPlayer::MoveForward(float units) {
    m_vecOrigin.x += units;  // simplified forward move
}

void CPlayer::Turn(float yawDelta) {
    m_angView.yaw += yawDelta;
}