#include "Primitives.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace Primitives {

// Helper: compute tangent and bitangent for a triangle
static void computeTangent(Vertex &v0, Vertex &v1, Vertex &v2) {
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
  tangent = glm::normalize(tangent);

  glm::vec3 bitangent;
  bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
  bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
  bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
  bitangent = glm::normalize(bitangent);

  v0.Tangent = v1.Tangent = v2.Tangent = tangent;
  v0.Bitangent = v1.Bitangent = v2.Bitangent = bitangent;
}

Mesh createSphere(float radius, int sectors, int stacks) {
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;

  float sectorStep = 2 * M_PI / sectors;
  float stackStep = M_PI / stacks;

  // Generate vertices
  for (int i = 0; i <= stacks; ++i) {
    float stackAngle = M_PI / 2 - i * stackStep; // from pi/2 to -pi/2
    float xy = radius * cosf(stackAngle);
    float z = radius * sinf(stackAngle);

    for (int j = 0; j <= sectors; ++j) {
      float sectorAngle = j * sectorStep;

      Vertex vertex;
      vertex.Position.x = xy * cosf(sectorAngle);
      vertex.Position.y = z;
      vertex.Position.z = xy * sinf(sectorAngle);

      // Normal is just the normalized position for a sphere
      vertex.Normal = glm::normalize(vertex.Position);

      // UV coordinates
      vertex.TexCoords.x = (float)j / sectors;
      vertex.TexCoords.y = (float)i / stacks;

      vertices.push_back(vertex);
    }
  }

  // Generate indices
  for (int i = 0; i < stacks; ++i) {
    int k1 = i * (sectors + 1);
    int k2 = k1 + sectors + 1;

    for (int j = 0; j < sectors; ++j, ++k1, ++k2) {
      // First triangle of quad (CCW winding from outside)
      if (i != 0) {
        indices.push_back(k1);
        indices.push_back(k1 + 1);
        indices.push_back(k2);
      }
      // Second triangle of quad (CCW winding from outside)
      if (i != (stacks - 1)) {
        indices.push_back(k1 + 1);
        indices.push_back(k2 + 1);
        indices.push_back(k2);
      }
    }
  }

  // Compute tangents
  for (size_t i = 0; i < indices.size(); i += 3) {
    computeTangent(vertices[indices[i]], vertices[indices[i + 1]],
                   vertices[indices[i + 2]]);
  }

  return Mesh(vertices, indices);
}

Mesh createCube(float size) {
  float s = size / 2.0f;

  std::vector<Vertex> vertices = {
      // Front face
      {{-s, -s, s}, {0, 0, 1}, {0, 0}, {}, {}},
      {{s, -s, s}, {0, 0, 1}, {1, 0}, {}, {}},
      {{s, s, s}, {0, 0, 1}, {1, 1}, {}, {}},
      {{-s, s, s}, {0, 0, 1}, {0, 1}, {}, {}},
      // Back face
      {{s, -s, -s}, {0, 0, -1}, {0, 0}, {}, {}},
      {{-s, -s, -s}, {0, 0, -1}, {1, 0}, {}, {}},
      {{-s, s, -s}, {0, 0, -1}, {1, 1}, {}, {}},
      {{s, s, -s}, {0, 0, -1}, {0, 1}, {}, {}},
      // Top face
      {{-s, s, s}, {0, 1, 0}, {0, 0}, {}, {}},
      {{s, s, s}, {0, 1, 0}, {1, 0}, {}, {}},
      {{s, s, -s}, {0, 1, 0}, {1, 1}, {}, {}},
      {{-s, s, -s}, {0, 1, 0}, {0, 1}, {}, {}},
      // Bottom face
      {{-s, -s, -s}, {0, -1, 0}, {0, 0}, {}, {}},
      {{s, -s, -s}, {0, -1, 0}, {1, 0}, {}, {}},
      {{s, -s, s}, {0, -1, 0}, {1, 1}, {}, {}},
      {{-s, -s, s}, {0, -1, 0}, {0, 1}, {}, {}},
      // Right face
      {{s, -s, s}, {1, 0, 0}, {0, 0}, {}, {}},
      {{s, -s, -s}, {1, 0, 0}, {1, 0}, {}, {}},
      {{s, s, -s}, {1, 0, 0}, {1, 1}, {}, {}},
      {{s, s, s}, {1, 0, 0}, {0, 1}, {}, {}},
      // Left face
      {{-s, -s, -s}, {-1, 0, 0}, {0, 0}, {}, {}},
      {{-s, -s, s}, {-1, 0, 0}, {1, 0}, {}, {}},
      {{-s, s, s}, {-1, 0, 0}, {1, 1}, {}, {}},
      {{-s, s, -s}, {-1, 0, 0}, {0, 1}, {}, {}},
  };

  std::vector<unsigned int> indices = {
      0,  1,  2,  0,  2,  3,  // front
      4,  5,  6,  4,  6,  7,  // back
      8,  9,  10, 8,  10, 11, // top
      12, 13, 14, 12, 14, 15, // bottom
      16, 17, 18, 16, 18, 19, // right
      20, 21, 22, 20, 22, 23  // left
  };

  // Compute tangents for each face
  for (size_t i = 0; i < indices.size(); i += 3) {
    computeTangent(vertices[indices[i]], vertices[indices[i + 1]],
                   vertices[indices[i + 2]]);
  }

  return Mesh(vertices, indices);
}

Mesh createCylinder(float radius, float height, int sectors) {
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;

  float sectorStep = 2 * M_PI / sectors;
  float halfHeight = height / 2.0f;

  // Side vertices (two rings)
  for (int i = 0; i <= 1; ++i) {
    float y = (i == 0) ? -halfHeight : halfHeight;
    for (int j = 0; j <= sectors; ++j) {
      float angle = j * sectorStep;
      Vertex v;
      v.Position = glm::vec3(radius * cosf(angle), y, radius * sinf(angle));
      v.Normal = glm::normalize(glm::vec3(cosf(angle), 0, sinf(angle)));
      v.TexCoords = glm::vec2((float)j / sectors, (float)i);
      vertices.push_back(v);
    }
  }

  // Side indices
  for (int j = 0; j < sectors; ++j) {
    int k1 = j;
    int k2 = j + sectors + 1;
    indices.push_back(k1);
    indices.push_back(k2);
    indices.push_back(k1 + 1);
    indices.push_back(k1 + 1);
    indices.push_back(k2);
    indices.push_back(k2 + 1);
  }

  // Top cap
  int topCenter = vertices.size();
  Vertex vc;
  vc.Position = glm::vec3(0, halfHeight, 0);
  vc.Normal = glm::vec3(0, 1, 0);
  vc.TexCoords = glm::vec2(0.5f, 0.5f);
  vertices.push_back(vc);

  for (int j = 0; j <= sectors; ++j) {
    float angle = j * sectorStep;
    Vertex v;
    v.Position =
        glm::vec3(radius * cosf(angle), halfHeight, radius * sinf(angle));
    v.Normal = glm::vec3(0, 1, 0);
    v.TexCoords =
        glm::vec2(cosf(angle) * 0.5f + 0.5f, sinf(angle) * 0.5f + 0.5f);
    vertices.push_back(v);
  }

  for (int j = 0; j < sectors; ++j) {
    indices.push_back(topCenter);
    indices.push_back(topCenter + j + 1);
    indices.push_back(topCenter + j + 2);
  }

  // Bottom cap
  int bottomCenter = vertices.size();
  vc.Position = glm::vec3(0, -halfHeight, 0);
  vc.Normal = glm::vec3(0, -1, 0);
  vertices.push_back(vc);

  for (int j = 0; j <= sectors; ++j) {
    float angle = j * sectorStep;
    Vertex v;
    v.Position =
        glm::vec3(radius * cosf(angle), -halfHeight, radius * sinf(angle));
    v.Normal = glm::vec3(0, -1, 0);
    v.TexCoords =
        glm::vec2(cosf(angle) * 0.5f + 0.5f, sinf(angle) * 0.5f + 0.5f);
    vertices.push_back(v);
  }

  for (int j = 0; j < sectors; ++j) {
    indices.push_back(bottomCenter);
    indices.push_back(bottomCenter + j + 2);
    indices.push_back(bottomCenter + j + 1);
  }

  // Compute tangents
  for (size_t i = 0; i < indices.size(); i += 3) {
    computeTangent(vertices[indices[i]], vertices[indices[i + 1]],
                   vertices[indices[i + 2]]);
  }

  return Mesh(vertices, indices);
}

Mesh createCone(float radius, float height, int sectors) {
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;

  float sectorStep = 2 * M_PI / sectors;
  float slopeAngle = atan2f(radius, height);

  // Apex
  Vertex apex;
  apex.Position = glm::vec3(0, height, 0);
  apex.Normal = glm::vec3(0, 1, 0);
  apex.TexCoords = glm::vec2(0.5f, 1.0f);
  vertices.push_back(apex);

  // Base ring
  for (int j = 0; j <= sectors; ++j) {
    float angle = j * sectorStep;
    Vertex v;
    v.Position = glm::vec3(radius * cosf(angle), 0, radius * sinf(angle));
    // Cone normal: blend between radial and vertical
    v.Normal = glm::normalize(glm::vec3(cosf(angle) * cosf(slopeAngle),
                                        sinf(slopeAngle),
                                        sinf(angle) * cosf(slopeAngle)));
    v.TexCoords = glm::vec2((float)j / sectors, 0);
    vertices.push_back(v);
  }

  // Side indices
  for (int j = 0; j < sectors; ++j) {
    indices.push_back(0);
    indices.push_back(j + 2);
    indices.push_back(j + 1);
  }

  // Bottom cap
  int bottomCenter = vertices.size();
  Vertex vc;
  vc.Position = glm::vec3(0, 0, 0);
  vc.Normal = glm::vec3(0, -1, 0);
  vc.TexCoords = glm::vec2(0.5f, 0.5f);
  vertices.push_back(vc);

  for (int j = 0; j <= sectors; ++j) {
    float angle = j * sectorStep;
    Vertex v;
    v.Position = glm::vec3(radius * cosf(angle), 0, radius * sinf(angle));
    v.Normal = glm::vec3(0, -1, 0);
    v.TexCoords =
        glm::vec2(cosf(angle) * 0.5f + 0.5f, sinf(angle) * 0.5f + 0.5f);
    vertices.push_back(v);
  }

  for (int j = 0; j < sectors; ++j) {
    indices.push_back(bottomCenter);
    indices.push_back(bottomCenter + j + 2);
    indices.push_back(bottomCenter + j + 1);
  }

  // Compute tangents
  for (size_t i = 0; i < indices.size(); i += 3) {
    computeTangent(vertices[indices[i]], vertices[indices[i + 1]],
                   vertices[indices[i + 2]]);
  }

  return Mesh(vertices, indices);
}

Mesh createPrism(int sides, float radius, float height) {
  return createFrustum(sides, radius, radius, height);
}

Mesh createFrustum(int sides, float bottomRadius, float topRadius,
                   float height) {
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;

  float sectorStep = 2 * M_PI / sides;
  float halfHeight = height / 2.0f;

  // Side vertices
  for (int i = 0; i <= 1; ++i) {
    float y = (i == 0) ? -halfHeight : halfHeight;
    float r = (i == 0) ? bottomRadius : topRadius;
    for (int j = 0; j <= sides; ++j) {
      float angle = j * sectorStep;
      Vertex v;
      v.Position = glm::vec3(r * cosf(angle), y, r * sinf(angle));
      // Approximate normal for frustum
      float slopeFactor = (bottomRadius - topRadius) / height;
      v.Normal =
          glm::normalize(glm::vec3(cosf(angle), slopeFactor, sinf(angle)));
      v.TexCoords = glm::vec2((float)j / sides, (float)i);
      vertices.push_back(v);
    }
  }

  // Side indices
  for (int j = 0; j < sides; ++j) {
    int k1 = j;
    int k2 = j + sides + 1;
    indices.push_back(k1);
    indices.push_back(k2);
    indices.push_back(k1 + 1);
    indices.push_back(k1 + 1);
    indices.push_back(k2);
    indices.push_back(k2 + 1);
  }

  // Top cap
  int topCenter = vertices.size();
  Vertex vc;
  vc.Position = glm::vec3(0, halfHeight, 0);
  vc.Normal = glm::vec3(0, 1, 0);
  vc.TexCoords = glm::vec2(0.5f, 0.5f);
  vertices.push_back(vc);

  for (int j = 0; j <= sides; ++j) {
    float angle = j * sectorStep;
    Vertex v;
    v.Position =
        glm::vec3(topRadius * cosf(angle), halfHeight, topRadius * sinf(angle));
    v.Normal = glm::vec3(0, 1, 0);
    v.TexCoords =
        glm::vec2(cosf(angle) * 0.5f + 0.5f, sinf(angle) * 0.5f + 0.5f);
    vertices.push_back(v);
  }

  for (int j = 0; j < sides; ++j) {
    indices.push_back(topCenter);
    indices.push_back(topCenter + j + 1);
    indices.push_back(topCenter + j + 2);
  }

  // Bottom cap
  int bottomCenter = vertices.size();
  vc.Position = glm::vec3(0, -halfHeight, 0);
  vc.Normal = glm::vec3(0, -1, 0);
  vertices.push_back(vc);

  for (int j = 0; j <= sides; ++j) {
    float angle = j * sectorStep;
    Vertex v;
    v.Position = glm::vec3(bottomRadius * cosf(angle), -halfHeight,
                           bottomRadius * sinf(angle));
    v.Normal = glm::vec3(0, -1, 0);
    v.TexCoords =
        glm::vec2(cosf(angle) * 0.5f + 0.5f, sinf(angle) * 0.5f + 0.5f);
    vertices.push_back(v);
  }

  for (int j = 0; j < sides; ++j) {
    indices.push_back(bottomCenter);
    indices.push_back(bottomCenter + j + 2);
    indices.push_back(bottomCenter + j + 1);
  }

  // Compute tangents
  for (size_t i = 0; i < indices.size(); i += 3) {
    computeTangent(vertices[indices[i]], vertices[indices[i + 1]],
                   vertices[indices[i + 2]]);
  }

  return Mesh(vertices, indices);
}

Mesh createPlane(float width, float depth) {
  float w = width / 2.0f;
  float d = depth / 2.0f;

  std::vector<Vertex> vertices = {
      {{-w, 0, d}, {0, 1, 0}, {0, 0}, {1, 0, 0}, {0, 0, 1}},
      {{w, 0, d}, {0, 1, 0}, {1, 0}, {1, 0, 0}, {0, 0, 1}},
      {{w, 0, -d}, {0, 1, 0}, {1, 1}, {1, 0, 0}, {0, 0, 1}},
      {{-w, 0, -d}, {0, 1, 0}, {0, 1}, {1, 0, 0}, {0, 0, 1}},
  };

  std::vector<unsigned int> indices = {0, 1, 2, 0, 2, 3};

  return Mesh(vertices, indices);
}

} // namespace Primitives
