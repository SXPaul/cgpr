#ifndef BALL_H
#define BALL_H

#include <glm/glm.hpp>

// Forward declaration
class Level;

/**
 * Ball class - Physics simulation for the marble.
 */
class Ball {
public:
  glm::vec3 position = glm::vec3(0.0f);
  glm::vec3 velocity = glm::vec3(0.0f);
  float radius = 0.35f;

  bool isFalling = false;
  float fallProgress = 0.0f;

  Ball() = default;

  void reset(const Level &level);
  void update(float dt, glm::vec2 tiltRadians, Level &level);
  bool hasFallenInHole() const { return isFalling && fallProgress >= 1.0f; }
};

#endif // BALL_H
