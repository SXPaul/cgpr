#ifndef BOARD_GENERATOR_H
#define BOARD_GENERATOR_H

#include "Level.h"
#include "Mesh.h"

namespace BoardGenerator {

struct BoardMeshes {
  Mesh floor;
  Mesh walls;
  Mesh frame;
  Mesh holeMarker; // Single cylinder, rendered per hole position
  Mesh startMarker;
  Mesh goalMarker;
};

BoardMeshes generateBoard(const Level &level);
Mesh createHoleMesh(float radius, float depth, int segments = 24);
Mesh createFrameMesh(float width, float depth, float height, float thickness);

} // namespace BoardGenerator

#endif // BOARD_GENERATOR_H
