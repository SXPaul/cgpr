#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

/**
 * Vertex structure - Contains all per-vertex attributes.
 *
 * For PBR rendering, we need:
 *   - Position: vertex location in model space
 *   - Normal: for lighting calculations
 *   - TexCoords: for texture mapping
 *   - Tangent/Bitangent: for normal mapping (tangent space basis)
 */
struct Vertex {
  glm::vec3 Position;
  glm::vec3 Normal;
  glm::vec2 TexCoords;
  glm::vec3 Tangent;
  glm::vec3 Bitangent;
};

/**
 * Mesh class - Represents a single drawable mesh.
 *
 * Handles VAO/VBO/EBO setup and rendering. A Model can contain
 * multiple meshes, and each mesh has its own vertex data.
 */
class Mesh {
public:
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  unsigned int VAO;

  Mesh() : VAO(0), VBO(0), EBO(0) {}
  Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);

  // Set up the mesh (create VAO, VBO, EBO)
  void setupMesh();

  // Render the mesh
  void draw() const;

  // Clean up GPU resources
  void cleanup();

private:
  unsigned int VBO, EBO;
};

#endif // MESH_H
