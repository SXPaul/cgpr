#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices)
    : vertices(std::move(vertices)), indices(std::move(indices)), VAO(0),
      VBO(0), EBO(0) {
  setupMesh();
}

void Mesh::setupMesh() {
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  // Upload vertex data
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
               vertices.data(), GL_STATIC_DRAW);

  // Upload index data
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               indices.data(), GL_STATIC_DRAW);

  // Vertex attribute layout:
  // layout(location = 0) = Position
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, Position));
  glEnableVertexAttribArray(0);

  // layout(location = 1) = Normal
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, Normal));
  glEnableVertexAttribArray(1);

  // layout(location = 2) = TexCoords
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, TexCoords));
  glEnableVertexAttribArray(2);

  // layout(location = 3) = Tangent
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, Tangent));
  glEnableVertexAttribArray(3);

  // layout(location = 4) = Bitangent
  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, Bitangent));
  glEnableVertexAttribArray(4);

  glBindVertexArray(0);
}

void Mesh::draw() const {
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()),
                 GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

void Mesh::cleanup() {
  if (VAO)
    glDeleteVertexArrays(1, &VAO);
  if (VBO)
    glDeleteBuffers(1, &VBO);
  if (EBO)
    glDeleteBuffers(1, &EBO);
  VAO = VBO = EBO = 0;
}
