#include "Level.h"
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>

Level::Level(const std::vector<std::string> &gridData, float cellSize)
    : grid(gridData), cellSize(cellSize) {
  height = static_cast<int>(grid.size());
  width = 0;

  for (int y = 0; y < height; ++y) {
    width = std::max(width, static_cast<int>(grid[y].size()));
    for (int x = 0; x < static_cast<int>(grid[y].size()); ++x) {
      if (grid[y][x] == 'S')
        startPos = {x, y};
      else if (grid[y][x] == 'G')
        goalPos = {x, y};
      else if (grid[y][x] == 'O')
        holePoss.push_back({x, y});
    }
  }
}

char Level::getCell(int x, int y) const {
  if (y < 0 || y >= height)
    return '#';
  if (x < 0 || x >= static_cast<int>(grid[y].size()))
    return '#';
  return grid[y][x];
}

glm::vec3 Level::gridToWorld(int x, int y) const {
  float worldX = (x + 0.5f) * cellSize - getBoardWidth() / 2.0f;
  float worldZ = (y + 0.5f) * cellSize - getBoardDepth() / 2.0f;
  return glm::vec3(worldX, 0.0f, worldZ);
}

glm::vec3 Level::gridToWorld(glm::ivec2 pos) const {
  return gridToWorld(pos.x, pos.y);
}

glm::ivec2 Level::worldToGrid(glm::vec3 worldPos) const {
  float localX = worldPos.x + getBoardWidth() / 2.0f;
  float localZ = worldPos.z + getBoardDepth() / 2.0f;
  return {static_cast<int>(localX / cellSize),
          static_cast<int>(localZ / cellSize)};
}

bool Level::isOverHole(glm::vec3 worldPos, float radius) const {
  glm::ivec2 cell = worldToGrid(worldPos);
  if (getCell(cell.x, cell.y) == 'O') {
    glm::vec3 holeCenter = gridToWorld(cell);
    float dist = glm::length(
        glm::vec2(worldPos.x - holeCenter.x, worldPos.z - holeCenter.z));
    return dist < cellSize * 0.3f;
  }
  return false;
}

bool Level::isAtGoal(glm::vec3 worldPos, float radius) const {
  glm::ivec2 cell = worldToGrid(worldPos);
  if (getCell(cell.x, cell.y) == 'G') {
    glm::vec3 goalCenter = gridToWorld(cell);
    float dist = glm::length(
        glm::vec2(worldPos.x - goalCenter.x, worldPos.z - goalCenter.z));
    return dist < cellSize * 0.4f;
  }
  return false;
}

glm::vec3 Level::resolveWallCollision(glm::vec3 pos, float radius) const {
  glm::vec3 result = pos;
  glm::ivec2 cell = worldToGrid(pos);

  for (int dy = -1; dy <= 1; ++dy) {
    for (int dx = -1; dx <= 1; ++dx) {
      int nx = cell.x + dx;
      int ny = cell.y + dy;
      if (getCell(nx, ny) == '#') {
        glm::vec3 wallCenter = gridToWorld(nx, ny);
        float halfSize = cellSize / 2.0f;
        float closestX = std::clamp(result.x, wallCenter.x - halfSize,
                                    wallCenter.x + halfSize);
        float closestZ = std::clamp(result.z, wallCenter.z - halfSize,
                                    wallCenter.z + halfSize);
        float distX = result.x - closestX;
        float distZ = result.z - closestZ;
        float dist = std::sqrt(distX * distX + distZ * distZ);
        if (dist < radius && dist > 0.0001f) {
          float overlap = radius - dist;
          result.x += (distX / dist) * overlap;
          result.z += (distZ / dist) * overlap;
        }
      }
    }
  }
  return result;
}

// Load a single level from a text file
static bool loadLevelFromFile(const std::string &filepath, Level &outLevel) {
  std::ifstream file(filepath);
  if (!file.is_open()) {
    std::cerr << "Failed to open level file: " << filepath << std::endl;
    return false;
  }

  std::vector<std::string> gridData;
  std::string line;
  while (std::getline(file, line)) {
    // Skip empty lines and trim whitespace
    if (!line.empty() && line[0] != '\n' && line[0] != '\r') {
      // Remove trailing \r if present (Windows line endings)
      if (!line.empty() && line.back() == '\r') {
        line.pop_back();
      }
      if (!line.empty()) {
        gridData.push_back(line);
      }
    }
  }

  if (gridData.empty()) {
    std::cerr << "Level file is empty: " << filepath << std::endl;
    return false;
  }

  outLevel = Level(gridData, 1.0f);
  return true;
}

void LevelManager::loadBuiltInLevels() {
  levels.clear();

  const std::string levelsDir = "assets/levels";

  // Collect all .txt files in the levels directory
  std::vector<std::string> levelFiles;
  try {
    for (const auto &entry : std::filesystem::directory_iterator(levelsDir)) {
      if (entry.is_regular_file() && entry.path().extension() == ".txt") {
        levelFiles.push_back(entry.path().string());
      }
    }
  } catch (const std::filesystem::filesystem_error &e) {
    std::cerr << "Failed to read levels directory: " << e.what() << std::endl;
    return;
  }

  // Sort files alphabetically (level1.txt, level2.txt, etc.)
  std::sort(levelFiles.begin(), levelFiles.end());

  // Load each level file
  for (const auto &filepath : levelFiles) {
    Level level;
    if (loadLevelFromFile(filepath, level)) {
      levels.push_back(std::move(level));
      std::cout << "Loaded level: " << filepath << std::endl;
    }
  }

  if (levels.empty()) {
    std::cerr << "Warning: No levels loaded!" << std::endl;
  }

  currentLevelIndex = 0;
}

Level &LevelManager::getCurrentLevel() { return levels[currentLevelIndex]; }
bool LevelManager::nextLevel() {
  if (hasNextLevel()) {
    currentLevelIndex++;
    return true;
  }
  return false;
}
void LevelManager::restartLevel() {}
bool LevelManager::hasNextLevel() const {
  return currentLevelIndex + 1 < static_cast<int>(levels.size());
}
