#pragma once

#include <fstream>
#include <string>
#include <vector>

class Level {
 public:
  std::vector<std::vector<int>> data;
  int width;
  int height;

  Level(std::string& filename);
};