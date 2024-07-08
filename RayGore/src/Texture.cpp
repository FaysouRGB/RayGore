#include "Texture.h"

Texture::Texture(std::string filename) {
  data = stbi_load(filename.c_str(), &width, &height, &channels, 0);

  transparent = false;
  has_collisions = filename.find("_") == std::string::npos;
  int total_pixels = width * height;

  for (int i = 0; i < total_pixels; ++i) {
    if (data[i * 4 + 3] != 255) {
      transparent = true;
      break;
    }
  }
}
