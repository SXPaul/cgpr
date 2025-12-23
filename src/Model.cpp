#include "Model.h"
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>

bool Model::loadFromFile(const std::string &path) {
  std::ifstream file(path);
  if (!file.is_open()) {
    std::cerr << "Failed to open model file: " << path << std::endl;
    return false;
  }

  // Extract directory from path
  size_t lastSlash = path.find_last_of("/\\");
  directory =
      (lastSlash != std::string::npos) ? path.substr(0, lastSlash) : ".";

  // Temporary storage for OBJ data
  std::vector<glm::vec3> positions;
  std::vector<glm::vec2> texCoords;
  std::vector<glm::vec3> normals;

  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;

  // Map to avoid duplicate vertices: key = "posIdx/texIdx/normIdx"
  std::map<std::string, unsigned int> uniqueVertices;

  std::string line;
  while (std::getline(file, line)) {
    std::istringstream iss(line);
    std::string prefix;
    iss >> prefix;

    if (prefix == "v") {
      // Vertex position
      glm::vec3 pos;
      iss >> pos.x >> pos.y >> pos.z;
      positions.push_back(pos);
    } else if (prefix == "vt") {
      // Texture coordinate
      glm::vec2 tex;
      iss >> tex.x >> tex.y;
      texCoords.push_back(tex);
    } else if (prefix == "vn") {
      // Normal
      glm::vec3 norm;
      iss >> norm.x >> norm.y >> norm.z;
      normals.push_back(norm);
    } else if (prefix == "f") {
      // Face - parse each vertex of the face
      std::string vertexStr;
      std::vector<unsigned int> faceIndices;

      while (iss >> vertexStr) {
        // Check if this vertex combination already exists
        auto it = uniqueVertices.find(vertexStr);
        if (it != uniqueVertices.end()) {
          faceIndices.push_back(it->second);
        } else {
          // Parse the vertex indices
          Vertex vertex = {};

          // Replace '/' with space for easier parsing
          std::replace(vertexStr.begin(), vertexStr.end(), '/', ' ');
          std::istringstream viss(vertexStr);

          int posIdx = 0, texIdx = 0, normIdx = 0;
          viss >> posIdx;

          // Handle different OBJ face formats
          if (viss.peek() == ' ') {
            viss >> texIdx;
            if (viss.peek() == ' ') {
              viss >> normIdx;
            }
          }

          // OBJ indices are 1-based
          if (posIdx > 0 && posIdx <= (int)positions.size()) {
            vertex.Position = positions[posIdx - 1];
          }
          if (texIdx > 0 && texIdx <= (int)texCoords.size()) {
            vertex.TexCoords = texCoords[texIdx - 1];
          }
          if (normIdx > 0 && normIdx <= (int)normals.size()) {
            vertex.Normal = normals[normIdx - 1];
          }

          unsigned int newIndex = static_cast<unsigned int>(vertices.size());
          vertices.push_back(vertex);
          uniqueVertices[vertexStr] = newIndex;
          faceIndices.push_back(newIndex);
        }
      }

      // Triangulate the face (assuming convex polygon)
      for (size_t i = 1; i + 1 < faceIndices.size(); ++i) {
        indices.push_back(faceIndices[0]);
        indices.push_back(faceIndices[i]);
        indices.push_back(faceIndices[i + 1]);
      }
    }
  }

  file.close();

  if (vertices.empty()) {
    std::cerr << "No vertices found in model file: " << path << std::endl;
    return false;
  }

  // Compute tangents if we have normals and tex coords
  // (Simple approach: compute per-triangle, then average)
  for (size_t i = 0; i + 2 < indices.size(); i += 3) {
    Vertex &v0 = vertices[indices[i]];
    Vertex &v1 = vertices[indices[i + 1]];
    Vertex &v2 = vertices[indices[i + 2]];

    glm::vec3 edge1 = v1.Position - v0.Position;
    glm::vec3 edge2 = v2.Position - v0.Position;
    glm::vec2 deltaUV1 = v1.TexCoords - v0.TexCoords;
    glm::vec2 deltaUV2 = v2.TexCoords - v0.TexCoords;

    float f =
        1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y + 0.0001f);

    glm::vec3 tangent;
    tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

    v0.Tangent += tangent;
    v1.Tangent += tangent;
    v2.Tangent += tangent;
  }

  // Normalize tangents
  for (auto &v : vertices) {
    if (glm::length(v.Tangent) > 0.0001f) {
      v.Tangent = glm::normalize(v.Tangent);
      v.Bitangent = glm::cross(v.Normal, v.Tangent);
    }
  }

  meshes.push_back(Mesh(std::move(vertices), std::move(indices)));
  return true;
}

bool Model::exportToOBJ(const std::string &path) const {
  std::ofstream file(path);
  if (!file.is_open()) {
    std::cerr << "Failed to open file for writing: " << path << std::endl;
    return false;
  }

  file << "# Exported OBJ file\n";

  unsigned int vertexOffset = 0;
  for (const auto &mesh : meshes) {
    // Write vertices
    for (const auto &v : mesh.vertices) {
      file << "v " << v.Position.x << " " << v.Position.y << " " << v.Position.z
           << "\n";
    }

    // Write texture coordinates
    for (const auto &v : mesh.vertices) {
      file << "vt " << v.TexCoords.x << " " << v.TexCoords.y << "\n";
    }

    // Write normals
    for (const auto &v : mesh.vertices) {
      file << "vn " << v.Normal.x << " " << v.Normal.y << " " << v.Normal.z
           << "\n";
    }

    // Write faces (OBJ is 1-indexed)
    for (size_t i = 0; i + 2 < mesh.indices.size(); i += 3) {
      unsigned int i0 = mesh.indices[i] + vertexOffset + 1;
      unsigned int i1 = mesh.indices[i + 1] + vertexOffset + 1;
      unsigned int i2 = mesh.indices[i + 2] + vertexOffset + 1;
      file << "f " << i0 << "/" << i0 << "/" << i0 << " " << i1 << "/" << i1
           << "/" << i1 << " " << i2 << "/" << i2 << "/" << i2 << "\n";
    }

    vertexOffset += mesh.vertices.size();
  }

  file.close();
  return true;
}

void Model::draw() const {
  for (const auto &mesh : meshes) {
    mesh.draw();
  }
}

void Model::cleanup() {
  for (auto &mesh : meshes) {
    mesh.cleanup();
  }
  meshes.clear();
}
