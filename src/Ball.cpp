#include "Ball.h"
#include "Config.h"
#include "Level.h"
#include <cmath>

void Ball::reset(const Level &level) {
  position = level.gridToWorld(level.startPos);
  position.y = radius;
  velocity = glm::vec3(0.0f);
  isFalling = false;
  fallProgress = 0.0f;
}

void Ball::update(float dt, glm::vec2 tiltRadians, Level &level) {
  if (isFalling) {
    fallProgress += dt * 3.0f;
    position.y = radius * (1.0f - fallProgress * 2.0f);
    return;
  }

  // Acceleration from tilt
  float ax = -Config::BALL_GRAVITY * std::sin(tiltRadians.x);
  float az = Config::BALL_GRAVITY * std::sin(tiltRadians.y);

  velocity.x += ax * dt;
  velocity.z += az * dt;
  velocity *= Config::BALL_FRICTION;

  float speed = std::sqrt(velocity.x * velocity.x + velocity.z * velocity.z);
  if (speed > Config::BALL_MAX_SPEED) {
    velocity.x = (velocity.x / speed) * Config::BALL_MAX_SPEED;
    velocity.z = (velocity.z / speed) * Config::BALL_MAX_SPEED;
  }

  position.x += velocity.x * dt;
  position.z += velocity.z * dt;

  glm::vec3 oldPos = position;
  position = level.resolveWallCollision(position, radius);

  if (std::abs(position.x - oldPos.x) > 0.001f) {
    velocity.x = -velocity.x * Config::BALL_BOUNCE;
  }
  if (std::abs(position.z - oldPos.z) > 0.001f) {
    velocity.z = -velocity.z * Config::BALL_BOUNCE;
  }

  if (level.isOverHole(position, radius)) {
    isFalling = true;
    fallProgress = 0.0f;
    velocity = glm::vec3(0.0f);
  }

  position.y = radius;
}
