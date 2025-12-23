#ifndef MODEL_H
#define MODEL_H

#include "Mesh.h"
#include <string>
#include <vector>

/**
 * Model class - Represents a 3D model loaded from file.
 *
 * This is a simple OBJ loader that parses vertex positions, normals,
 * and texture coordinates. It does NOT use Assimp to keep dependencies minimal.
 *
 * Supports:
 *   - Triangulated OBJ files
 *   - Vertex positions (v)
 *   - Texture coordinates (vt)
 *   - Normals (vn)
 *   - Faces (f) with format: v/vt/vn or v//vn or v/vt or v
 */
class Model {
public:
  std::vector<Mesh> meshes;
  std::string directory;

  Model() = default;

  // Load a model from file
  bool loadFromFile(const std::string &path);

  // Export model to OBJ format
  bool exportToOBJ(const std::string &path) const;

  // Draw all meshes
  void draw() const;

  // Cleanup
  void cleanup();
};

#endif // MODEL_H
