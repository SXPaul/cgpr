#include "Camera.h"
#include <algorithm>

Camera::Camera(glm::vec3 target, float distance)
    : Target(target), Distance(distance), Yaw(-90.0f), Pitch(20.0f),
      OrbitSpeed(1.0f), PanSpeed(0.0015f), ZoomSpeed(0.5f), Fov(45.0f),
      NearPlane(0.1f), FarPlane(100.0f) {}

glm::vec3 Camera::calculatePosition() const {
  // Convert spherical coordinates (yaw, pitch, distance) to Cartesian
  // This places the camera on a sphere around Target
  float x = Distance * cos(glm::radians(Pitch)) * cos(glm::radians(Yaw));
  float y = Distance * sin(glm::radians(Pitch));
  float z = Distance * cos(glm::radians(Pitch)) * sin(glm::radians(Yaw));
  return Target + glm::vec3(x, y, z);
}

glm::vec3 Camera::getPosition() const { return calculatePosition(); }

glm::mat4 Camera::getViewMatrix() const {
  glm::vec3 position = calculatePosition();
  return glm::lookAt(position, Target, glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::mat4 Camera::getProjectionMatrix(float aspectRatio) const {
  return glm::perspective(glm::radians(Fov), aspectRatio, NearPlane, FarPlane);
}

glm::vec3 Camera::getRightVector() const {
  // Right vector is perpendicular to the view direction and world up
  glm::vec3 position = calculatePosition();
  glm::vec3 front = glm::normalize(Target - position);
  return glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
}

glm::vec3 Camera::getUpVector() const {
  glm::vec3 position = calculatePosition();
  glm::vec3 front = glm::normalize(Target - position);
  glm::vec3 right = getRightVector();
  return glm::normalize(glm::cross(right, front));
}

void Camera::processOrbit(float xoffset, float yoffset) {
  Yaw += xoffset * OrbitSpeed;
  Pitch += yoffset * OrbitSpeed;

  // Constrain pitch to avoid gimbal lock and flipping
  Pitch = std::clamp(Pitch, -89.0f, 89.0f);
}

void Camera::processPan(float xoffset, float yoffset) {
  // Pan moves the target point in the camera's local right/up plane
  glm::vec3 right = getRightVector();
  glm::vec3 up = getUpVector();

  Target -= right * xoffset * PanSpeed * Distance;
  Target += up * yoffset * PanSpeed * Distance;
}

void Camera::processZoom(float yoffset) {
  Distance -= yoffset * ZoomSpeed;
  // Clamp to reasonable values
  Distance = std::clamp(Distance, 0.5f, 50.0f);
}

void Camera::reset() {
  Target = glm::vec3(0.0f);
  Distance = 5.0f;
  Yaw = -90.0f;
  Pitch = 20.0f;
}
