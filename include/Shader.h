#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

/**
 * Shader class - Manages OpenGL shader programs.
 *
 * This class handles loading, compiling, linking, and using GLSL shaders.
 * It also provides utility functions for setting uniform values.
 */
class Shader {
public:
  unsigned int ID; // OpenGL program ID

  Shader() : ID(0) {}

  // Load and compile shaders from file paths
  bool load(const std::string &vertexPath, const std::string &fragmentPath);

  // Activate this shader program
  void use() const;

  // Uniform setters
  void setBool(const std::string &name, bool value) const;
  void setInt(const std::string &name, int value) const;
  void setFloat(const std::string &name, float value) const;
  void setVec2(const std::string &name, const glm::vec2 &value) const;
  void setVec3(const std::string &name, const glm::vec3 &value) const;
  void setVec4(const std::string &name, const glm::vec4 &value) const;
  void setMat3(const std::string &name, const glm::mat3 &value) const;
  void setMat4(const std::string &name, const glm::mat4 &value) const;

private:
  // Cache for uniform locations to avoid repeated lookups
  mutable std::unordered_map<std::string, int> uniformCache;

  int getUniformLocation(const std::string &name) const;
  unsigned int compileShader(const std::string &source, GLenum type);
  bool checkCompileErrors(unsigned int shader, const std::string &type);
};

#endif // SHADER_H
