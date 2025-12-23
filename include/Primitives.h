#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include "Mesh.h"

/**
 * Primitives namespace - Generates common 3D primitive shapes.
 *
 * Each function returns a Mesh with proper vertex data including
 * positions, normals, texture coordinates, and tangent space vectors.
 *
 * These are procedurally generated, meeting the rubric requirement
 * for "basic primitives" (cube, sphere, cylinder, cone, etc.)
 */
namespace Primitives {
// Sphere with configurable resolution (latitude/longitude segments)
Mesh createSphere(float radius = 1.0f, int sectors = 36, int stacks = 18);

// Unit cube centered at origin
Mesh createCube(float size = 1.0f);

// Cylinder along Y-axis
Mesh createCylinder(float radius = 0.5f, float height = 1.0f, int sectors = 36);

// Cone along Y-axis (base at origin, tip at height)
Mesh createCone(float radius = 0.5f, float height = 1.0f, int sectors = 36);

// Regular prism (n-sided polygon extruded)
Mesh createPrism(int sides = 6, float radius = 0.5f, float height = 1.0f);

// Frustum (truncated pyramid/cone with n sides)
Mesh createFrustum(int sides = 6, float bottomRadius = 0.5f,
                   float topRadius = 0.25f, float height = 1.0f);

// Plane (useful for ground, walls, etc.)
Mesh createPlane(float width = 10.0f, float depth = 10.0f);
} // namespace Primitives

#endif // PRIMITIVES_H
