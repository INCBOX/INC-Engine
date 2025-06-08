#include "Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : Front(glm::vec3(0.0f, 0.0f, -1.0f)),
      MovementSpeed(5.0f),
      MouseSensitivity(0.1f),
      Position(position),
      WorldUp(up),
      Yaw(yaw),
      Pitch(pitch) {
    updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix() const {
    return glm::lookAt(Position, Position + Front, Up);
}

void Camera::ProcessKeyboard(const Uint8* state, float deltaTime) {
    float velocity = MovementSpeed * deltaTime;
    if (state[SDL_SCANCODE_W]) Position += Front * velocity;
    if (state[SDL_SCANCODE_S]) Position -= Front * velocity;
    if (state[SDL_SCANCODE_A]) Position -= Right * velocity;
    if (state[SDL_SCANCODE_D]) Position += Right * velocity;
}

void Camera::ProcessMouseMotion(int xrel, int yrel) {
    float xOffset = xrel * MouseSensitivity;
    float yOffset = yrel * MouseSensitivity;

    Yaw += xOffset;
    Pitch -= yOffset;

    Pitch = glm::clamp(Pitch, -89.0f, 89.0f);

    updateCameraVectors();
}

void Camera::updateCameraVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));
}