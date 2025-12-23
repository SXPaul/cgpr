#define STB_IMAGE_IMPLEMENTATION
#include "Texture.h"
#include "stb_image.h"
#include <iostream>

bool Texture::loadFromFile(const std::string &path, bool sRGB) {
  stbi_set_flip_vertically_on_load(true);

  int nrChannels;
  unsigned char *data =
      stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

  if (!data) {
    std::cerr << "Failed to load texture: " << path << std::endl;
    return false;
  }

  // Determine format based on number of channels
  GLenum internalFormat, dataFormat;
  if (nrChannels == 1) {
    internalFormat = GL_RED;
    dataFormat = GL_RED;
  } else if (nrChannels == 3) {
    internalFormat = sRGB ? GL_SRGB : GL_RGB;
    dataFormat = GL_RGB;
  } else if (nrChannels == 4) {
    internalFormat = sRGB ? GL_SRGB_ALPHA : GL_RGBA;
    dataFormat = GL_RGBA;
  } else {
    std::cerr << "Unsupported channel count: " << nrChannels << std::endl;
    stbi_image_free(data);
    return false;
  }

  glGenTextures(1, &ID);
  glBindTexture(GL_TEXTURE_2D, ID);

  glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat,
               GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  // Good default filtering for most cases
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  stbi_image_free(data);
  return true;
}

bool Texture::loadHDR(const std::string &path) {
  stbi_set_flip_vertically_on_load(true);

  int nrChannels;
  float *data = stbi_loadf(path.c_str(), &width, &height, &nrChannels, 0);

  if (!data) {
    std::cerr << "Failed to load HDR texture: " << path << std::endl;
    return false;
  }

  glGenTextures(1, &ID);
  glBindTexture(GL_TEXTURE_2D, ID);

  // HDR textures use floating point format
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT,
               data);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  stbi_image_free(data);
  return true;
}

void Texture::createSolidColor(float r, float g, float b, float a) {
  unsigned char pixel[4] = {
      static_cast<unsigned char>(r * 255), static_cast<unsigned char>(g * 255),
      static_cast<unsigned char>(b * 255), static_cast<unsigned char>(a * 255)};

  glGenTextures(1, &ID);
  glBindTexture(GL_TEXTURE_2D, ID);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE,
               pixel);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  width = height = 1;
}

void Texture::bind(unsigned int unit) const {
  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::cleanup() {
  if (ID) {
    glDeleteTextures(1, &ID);
    ID = 0;
  }
}

// --- Cubemap ---

void Cubemap::create(int size, bool hdr) {
  glGenTextures(1, &ID);
  glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

  for (unsigned int i = 0; i < 6; ++i) {
    if (hdr) {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, size, size,
                   0, GL_RGB, GL_FLOAT, nullptr);
    } else {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, size, size, 0,
                   GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    }
  }

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Cubemap::bind(unsigned int unit) const {
  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
}

void Cubemap::cleanup() {
  if (ID) {
    glDeleteTextures(1, &ID);
    ID = 0;
  }
}
