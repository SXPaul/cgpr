#ifndef SCENE_H
#define SCENE_H

#include "Mesh.h"
#include "Model.h"
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <string>
#include <vector>

/**
 * PBR Material - Physically Based Rendering material properties.
 *
 * The PBR workflow we use is "Metallic-Roughness" (as used in glTF).
 * - Albedo: base color of the surface
 * - Metallic: 0 = dielectric (non-metal), 1 = metal
 * - Roughness: 0 = smooth/mirror, 1 = rough/diffuse
 * - AO: ambient occlusion (pre-baked shadowing)
 */
struct PBRMaterial {
  glm::vec3 albedo = glm::vec3(1.0f);
  float metallic = 0.0f;
  float roughness = 0.5f;
  float ao = 1.0f;

  // Texture IDs (0 = use solid color from above)
  unsigned int albedoMap = 0;
  unsigned int normalMap = 0;
  unsigned int metallicMap = 0;
  unsigned int roughnessMap = 0;
  unsigned int aoMap = 0;
};

/**
 * SceneObject - An object in the scene with transform and material.
 */
struct SceneObject {
  std::string name;
  Mesh mesh;
  PBRMaterial material;

  // Transform
  glm::vec3 position = glm::vec3(0.0f);
  glm::vec3 rotation = glm::vec3(0.0f); // Euler angles in degrees
  glm::vec3 scale = glm::vec3(1.0f);

  bool visible = true;

  glm::mat4 getModelMatrix() const {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1, 0, 0));
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0, 1, 0));
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0, 0, 1));
    model = glm::scale(model, scale);
    return model;
  }
};

/**
 * Light types for the scene.
 */
enum class LightType { Point, Directional };

struct Light {
  LightType type = LightType::Point;
  glm::vec3 position = glm::vec3(0.0f, 5.0f, 0.0f);
  glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f);
  glm::vec3 color = glm::vec3(1.0f);
  float intensity = 10.0f;
  bool enabled = true;
};

/**
 * Scene class - Container for all scene objects and lights.
 */
class Scene {
public:
  std::vector<SceneObject> objects;
  std::vector<Light> lights;

  // Environment settings
  glm::vec3 ambientColor = glm::vec3(0.03f);

  // Selected object index (-1 = none)
  int selectedObjectIndex = -1;

  // Add a new object to the scene
  void addObject(const std::string &name, const Mesh &mesh,
                 const PBRMaterial &material = {});

  // Add a light to the scene
  void addLight(const Light &light);

  // Remove selected object
  void removeSelected();

  // Clear the entire scene
  void clear();

  // Get the currently selected object (or nullptr)
  SceneObject *getSelected();
};

#endif // SCENE_H
