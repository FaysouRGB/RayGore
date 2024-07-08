#pragma once

#include <string>

#include "stb_image.h"

class Texture {
 public:
  int width;
  int height;
  int channels;
  unsigned char* data;
  bool transparent;
  bool has_collisions;
  Texture(std::string filename);
};