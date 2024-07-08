#include "Level.h"

Level::Level(std::string& filename) {
  // Create stream
  std::ifstream file(filename);

  // Read lines
  std::string line;
  while (std::getline(file, line)) {
    std::vector<int> row;
    for (char& c : line) {
      row.push_back(c - '0');
    }

    data.push_back(row);
  }

  // Set dimensions
  width = data[0].size();
  height = data.size();
}