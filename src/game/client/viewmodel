#include "mathlib/vector.h"
#include <iostream>

class ViewModel {
    Vector m_position;
    float m_fov;

public:
    ViewModel() : m_position(0, 0, 0), m_fov(90.0f) {}

    void Update(float dt) {
        std::cout << "[ViewModel] Updating viewmodel position
";
    }

    void Render() {
        std::cout << "[ViewModel] Rendering viewmodel at "
                  << m_position.x << ", " << m_position.y << ", " << m_position.z << std::endl;
    }
};