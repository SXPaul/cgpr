#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <string>

/**
 * Texture class - Handles 2D texture loading and binding.
 *
 * For PBR, we typically need multiple texture types:
 *   - Albedo (base color)
 *   - Normal map
 *   - Metallic
 *   - Roughness
 *   - AO (Ambient Occlusion)
 *
 * This class also supports HDR textures for IBL (Image Based Lighting).
 */
class Texture {
public:
  unsigned int ID;
  int width, height;

  Texture() : ID(0), width(0), height(0) {}

  // Load a texture from file (supports JPG, PNG, HDR, etc.)
  bool loadFromFile(const std::string &path, bool sRGB = true);

  // Load an HDR texture (for environment maps)
  bool loadHDR(const std::string &path);

  // Create a solid color texture (useful for defaults)
  void createSolidColor(float r, float g, float b, float a = 1.0f);

  // Bind this texture to a texture unit
  void bind(unsigned int unit = 0) const;

  void cleanup();
};

/**
 * Cubemap class - For environment mapping / IBL.
 */
class Cubemap {
public:
  unsigned int ID;

  Cubemap() : ID(0) {}

  // Create an empty cubemap for rendering into
  void create(int size, bool hdr = true);

  void bind(unsigned int unit = 0) const;
  void cleanup();
};

#endif // TEXTURE_H
