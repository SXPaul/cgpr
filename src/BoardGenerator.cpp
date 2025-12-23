#include "BoardGenerator.h"
#include "Primitives.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace BoardGenerator {

BoardMeshes generateBoard(const Level &level) {
  BoardMeshes result;

  float cellSize = level.cellSize;
  float wallHeight = cellSize * 0.6f;
  float floorThickness = cellSize * 0.15f;

  std::vector<Vertex> floorVerts, wallVerts;
  std::vector<unsigned int> floorInds, wallInds;

  Mesh floorTile = Primitives::createCube(cellSize);
  Mesh wallBlock = Primitives::createCube(cellSize);

  for (int y = 0; y < level.height; ++y) {
    for (int x = 0; x < level.width; ++x) {
      char cell = level.getCell(x, y);
      glm::vec3 worldPos = level.gridToWorld(x, y);

      if (cell == '#') {
        glm::vec3 wallPos = worldPos;
        wallPos.y = wallHeight / 2.0f;
        for (const auto &v : wallBlock.vertices) {
          Vertex newV = v;
          newV.Position.x = v.Position.x + wallPos.x;
          newV.Position.y = v.Position.y * (wallHeight / cellSize) + wallPos.y;
          newV.Position.z = v.Position.z + wallPos.z;
          wallVerts.push_back(newV);
        }
        unsigned int baseIdx = static_cast<unsigned int>(wallVerts.size()) -
                               wallBlock.vertices.size();
        for (unsigned int idx : wallBlock.indices)
          wallInds.push_back(baseIdx + idx);
      } else {
        // Floor tile for walkable cells (including holes, start, goal)
        glm::vec3 floorPos = worldPos;
        floorPos.y = -floorThickness / 2.0f;
        for (const auto &v : floorTile.vertices) {
          Vertex newV = v;
          newV.Position.x = v.Position.x + floorPos.x;
          newV.Position.y =
              v.Position.y * (floorThickness / cellSize) + floorPos.y;
          newV.Position.z = v.Position.z + floorPos.z;
          floorVerts.push_back(newV);
        }
        unsigned int baseIdx = static_cast<unsigned int>(floorVerts.size()) -
                               floorTile.vertices.size();
        for (unsigned int idx : floorTile.indices)
          floorInds.push_back(baseIdx + idx);
      }
    }
  }

  if (!floorVerts.empty())
    result.floor = Mesh(floorVerts, floorInds);
  if (!wallVerts.empty())
    result.walls = Mesh(wallVerts, wallInds);

  result.frame = createFrameMesh(level.getBoardWidth(), level.getBoardDepth(),
                                 wallHeight * 1.2f, cellSize * 0.3f);

  // All markers use the same cylinder shape
  result.holeMarker = Primitives::createCylinder(cellSize * 0.3f, 0.02f, 16);
  result.startMarker = Primitives::createCylinder(cellSize * 0.35f, 0.02f, 16);
  result.goalMarker = Primitives::createCylinder(cellSize * 0.35f, 0.02f, 16);

  return result;
}

Mesh createHoleMesh(float radius, float depth, int segments) {
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  float step = 2.0f * M_PI / segments;

  for (int i = 0; i <= segments; ++i) {
    float angle = i * step;
    float x = radius * std::cos(angle);
    float z = radius * std::sin(angle);

    Vertex vTop;
    vTop.Position = glm::vec3(x, 0.01f, z);
    vTop.Normal = glm::vec3(0, 1, 0);
    vTop.TexCoords = glm::vec2(float(i) / segments, 0);
    vertices.push_back(vTop);

    Vertex vBot;
    vBot.Position = glm::vec3(x, -depth, z);
    vBot.Normal = glm::normalize(glm::vec3(x, 0, z));
    vBot.TexCoords = glm::vec2(float(i) / segments, 1);
    vertices.push_back(vBot);
  }

  for (int i = 0; i < segments; ++i) {
    int t0 = i * 2, t1 = i * 2 + 1, t2 = (i + 1) * 2, t3 = (i + 1) * 2 + 1;
    indices.push_back(t0);
    indices.push_back(t1);
    indices.push_back(t2);
    indices.push_back(t2);
    indices.push_back(t1);
    indices.push_back(t3);
  }

  return Mesh(vertices, indices);
}

Mesh createFrameMesh(float width, float depth, float height, float thickness) {
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  Mesh wallTemplate = Primitives::createCube(1.0f);

  float halfW = width / 2.0f, halfD = depth / 2.0f, halfH = height / 2.0f;

  struct Wall {
    glm::vec3 pos, scale;
  };
  std::vector<Wall> walls = {
      {{0, halfH, -halfD - thickness / 2},
       {width + thickness * 2, height, thickness}},
      {{0, halfH, halfD + thickness / 2},
       {width + thickness * 2, height, thickness}},
      {{-halfW - thickness / 2, halfH, 0}, {thickness, height, depth}},
      {{halfW + thickness / 2, halfH, 0}, {thickness, height, depth}},
  };

  for (const auto &wall : walls) {
    unsigned int baseIdx = static_cast<unsigned int>(vertices.size());
    for (const auto &v : wallTemplate.vertices) {
      Vertex newV = v;
      newV.Position.x = v.Position.x * wall.scale.x + wall.pos.x;
      newV.Position.y = v.Position.y * wall.scale.y + wall.pos.y;
      newV.Position.z = v.Position.z * wall.scale.z + wall.pos.z;
      vertices.push_back(newV);
    }
    for (unsigned int idx : wallTemplate.indices)
      indices.push_back(baseIdx + idx);
  }

  return Mesh(vertices, indices);
}

} // namespace BoardGenerator
