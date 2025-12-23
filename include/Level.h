#ifndef LEVEL_H
#define LEVEL_H

#include <glm/glm.hpp>
#include <string>
#include <vector>

/**
 * Level class - Grid-based level definition for the marble maze.
 */
class Level {
public:
  std::vector<std::string> grid;
  int width = 0;
  int height = 0;
  float cellSize = 1.0f;

  glm::ivec2 startPos = {0, 0};
  glm::ivec2 goalPos = {0, 0};
  std::vector<glm::ivec2> holePoss; // All hole positions

  Level() = default;
  Level(const std::vector<std::string> &gridData, float cellSize = 1.0f);

  char getCell(int x, int y) const;
  glm::vec3 gridToWorld(int x, int y) const;
  glm::vec3 gridToWorld(glm::ivec2 pos) const;
  glm::ivec2 worldToGrid(glm::vec3 worldPos) const;
  bool isOverHole(glm::vec3 worldPos, float radius) const;
  bool isAtGoal(glm::vec3 worldPos, float radius) const;
  glm::vec3 resolveWallCollision(glm::vec3 pos, float radius) const;

  float getBoardWidth() const { return width * cellSize; }
  float getBoardDepth() const { return height * cellSize; }
};

class LevelManager {
public:
  std::vector<Level> levels;
  int currentLevelIndex = 0;

  void loadBuiltInLevels();
  Level &getCurrentLevel();
  bool nextLevel();
  void restartLevel();
  bool hasNextLevel() const;
};

#endif // LEVEL_H
