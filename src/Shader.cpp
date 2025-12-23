#include "Shader.h"
#include <fstream>
#include <iostream>
#include <sstream>

bool Shader::load(const std::string &vertexPath,
                  const std::string &fragmentPath) {
  // 1. Read shader source code from files
  std::string vertexCode, fragmentCode;
  std::ifstream vShaderFile, fShaderFile;

  vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

  try {
    vShaderFile.open(vertexPath);
    fShaderFile.open(fragmentPath);

    std::stringstream vShaderStream, fShaderStream;
    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();

    vShaderFile.close();
    fShaderFile.close();

    vertexCode = vShaderStream.str();
    fragmentCode = fShaderStream.str();
  } catch (std::ifstream::failure &e) {
    std::cerr << "ERROR::SHADER::FILE_NOT_READ: " << e.what() << std::endl;
    return false;
  }

  // 2. Compile shaders
  unsigned int vertex = compileShader(vertexCode, GL_VERTEX_SHADER);
  if (!checkCompileErrors(vertex, "VERTEX"))
    return false;

  unsigned int fragment = compileShader(fragmentCode, GL_FRAGMENT_SHADER);
  if (!checkCompileErrors(fragment, "FRAGMENT"))
    return false;

  // 3. Link program
  ID = glCreateProgram();
  glAttachShader(ID, vertex);
  glAttachShader(ID, fragment);
  glLinkProgram(ID);

  if (!checkCompileErrors(ID, "PROGRAM"))
    return false;

  // 4. Clean up individual shaders (they're now linked into the program)
  glDeleteShader(vertex);
  glDeleteShader(fragment);

  return true;
}

unsigned int Shader::compileShader(const std::string &source, GLenum type) {
  unsigned int shader = glCreateShader(type);
  const char *src = source.c_str();
  glShaderSource(shader, 1, &src, nullptr);
  glCompileShader(shader);
  return shader;
}

bool Shader::checkCompileErrors(unsigned int shader, const std::string &type) {
  int success;
  char infoLog[1024];

  if (type != "PROGRAM") {
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
      std::cerr << "ERROR::SHADER_COMPILATION::" << type << "\n"
                << infoLog << std::endl;
      return false;
    }
  } else {
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
      std::cerr << "ERROR::PROGRAM_LINKING\n" << infoLog << std::endl;
      return false;
    }
  }
  return true;
}

void Shader::use() const { glUseProgram(ID); }

int Shader::getUniformLocation(const std::string &name) const {
  auto it = uniformCache.find(name);
  if (it != uniformCache.end()) {
    return it->second;
  }
  int location = glGetUniformLocation(ID, name.c_str());
  uniformCache[name] = location;
  return location;
}

void Shader::setBool(const std::string &name, bool value) const {
  glUniform1i(getUniformLocation(name), (int)value);
}

void Shader::setInt(const std::string &name, int value) const {
  glUniform1i(getUniformLocation(name), value);
}

void Shader::setFloat(const std::string &name, float value) const {
  glUniform1f(getUniformLocation(name), value);
}

void Shader::setVec2(const std::string &name, const glm::vec2 &value) const {
  glUniform2fv(getUniformLocation(name), 1, &value[0]);
}

void Shader::setVec3(const std::string &name, const glm::vec3 &value) const {
  glUniform3fv(getUniformLocation(name), 1, &value[0]);
}

void Shader::setVec4(const std::string &name, const glm::vec4 &value) const {
  glUniform4fv(getUniformLocation(name), 1, &value[0]);
}

void Shader::setMat3(const std::string &name, const glm::mat3 &value) const {
  glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, &value[0][0]);
}

void Shader::setMat4(const std::string &name, const glm::mat4 &value) const {
  glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &value[0][0]);
}
