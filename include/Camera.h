#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

/**
 * Camera class - Provides an Orbit-style camera for 3D scene navigation.
 *
 * This camera orbits around a target point, which is common in 3D modeling
 * and viewing applications. It supports:
 *   - Orbit: Rotate around the target (Yaw/Pitch)
 *   - Pan: Move the target point
 *   - Zoom: Adjust distance to target
 */
class Camera {
public:
  // Camera state
  glm::vec3 Target; // Point the camera orbits around
  float Distance;   // Distance from target
  float Yaw;        // Horizontal angle (degrees)
  float Pitch;      // Vertical angle (degrees)

  // Camera options
  float OrbitSpeed;
  float PanSpeed;
  float ZoomSpeed;
  float Fov;
  float NearPlane;
  float FarPlane;

  Camera(glm::vec3 target = glm::vec3(0.0f), float distance = 5.0f);

  // Get the view matrix calculated using the orbit parameters
  glm::mat4 getViewMatrix() const;

  // Get the projection matrix
  glm::mat4 getProjectionMatrix(float aspectRatio) const;

  // Get camera position in world space
  glm::vec3 getPosition() const;

  // Input processing
  void processOrbit(float xoffset, float yoffset);
  void processPan(float xoffset, float yoffset);
  void processZoom(float yoffset);

  // Utility: Reset camera to default position
  void reset();

private:
  glm::vec3 calculatePosition() const;
  glm::vec3 getRightVector() const;
  glm::vec3 getUpVector() const;
};

#endif // CAMERA_H
