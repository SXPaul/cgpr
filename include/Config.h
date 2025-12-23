#ifndef CONFIG_H
#define CONFIG_H

/**
 * Game Configuration
 *
 * All tunable parameters are defined here for easy adjustment.
 * Modify these values to change the game feel.
 */

namespace Config {

// ============================================================================
// BALL PHYSICS
// ============================================================================

// Ball size
constexpr float BALL_RADIUS = 0.25f;

// Gravity strength (affects acceleration when tilted)
constexpr float BALL_GRAVITY = 60.0f; // Increase for faster acceleration

// Friction/damping (0.0 = instant stop, 1.0 = no friction)
constexpr float BALL_FRICTION = 0.985f; // Higher = more slippery

// Maximum speed the ball can reach
constexpr float BALL_MAX_SPEED = 50.0f; // Increase for faster rolling

// Bounce energy retention when hitting walls (0.0 = no bounce, 1.0 = full
// bounce)
constexpr float BALL_BOUNCE = 0.3f;

// ============================================================================
// BOARD TILT
// ============================================================================

// Maximum tilt angle in degrees
constexpr float MAX_TILT_DEGREES = 10.0f;

// How fast the board tilts (degrees per second)
constexpr float TILT_SPEED_DEGREES = 45.0f;

// How fast the board returns to center when not pressing keys (degrees per
// second)
constexpr float TILT_RETURN_SPEED_DEGREES = 30.0f;

// ============================================================================
// CAMERA
// ============================================================================

// Initial camera distance from target
constexpr float CAMERA_INITIAL_DISTANCE = 14.0f;

// Initial camera pitch (looking down angle)
constexpr float CAMERA_INITIAL_PITCH = 55.0f;

// Camera pan speed (when using WASD)
constexpr float CAMERA_PAN_SPEED = 0.008f; // Lower = slower pan

// Camera orbit speed (when using Q/E)
constexpr float CAMERA_ORBIT_SPEED = 60.0f; // Degrees per second

// Camera zoom speed (scroll wheel)
constexpr float CAMERA_ZOOM_SPEED = 1.0f;

// ============================================================================
// LEVEL GRID
// ============================================================================

// Size of each grid cell in world units
constexpr float CELL_SIZE = 1.0f;

// Wall height relative to cell size
constexpr float WALL_HEIGHT_RATIO = 0.6f;

// Floor thickness relative to cell size
constexpr float FLOOR_THICKNESS_RATIO = 0.15f;

// Hole radius relative to cell size (for detection)
constexpr float HOLE_DETECTION_RATIO = 0.3f;

// Goal detection radius relative to cell size
constexpr float GOAL_DETECTION_RATIO = 0.4f;

// ============================================================================
// RENDERING
// ============================================================================

// Light positions
constexpr float LIGHT1_X = 5.0f;
constexpr float LIGHT1_Y = 12.0f;
constexpr float LIGHT1_Z = 5.0f;
constexpr float LIGHT1_INTENSITY = 4000.0f;

constexpr float LIGHT2_X = -5.0f;
constexpr float LIGHT2_Y = 10.0f;
constexpr float LIGHT2_Z = -5.0f;
constexpr float LIGHT2_INTENSITY = 2500.0f;

// Ball material (metallic chrome)
constexpr float BALL_METALLIC = 1.0f;
constexpr float BALL_ROUGHNESS = 0.1f; // Lower = shinier

// Wood material (when not using textures)
constexpr float WOOD_METALLIC = 0.0f;
constexpr float WOOD_ROUGHNESS = 0.65f;

} // namespace Config

#endif // CONFIG_H
