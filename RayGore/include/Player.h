#pragma once
#include <SDL.h>

#include "Level.h"
#include "Structs.h"
#include "Texture.h"
#define _USE_MATH_DEFINES
#include <cmath>

class Player {
 private:
  float speed;
  float sensitivity;

 public:
  Vector2 pos;
  Vector2 dir;
  Vector2 plane;
  float headbob_timer = 0.0f;
  float headbob_amplitude = 0.0125f;
  float headbob_frequency = 20.0f;

  Player(float x, float y, float speed, float sensitivity, float FOV_degrees);

  void Move(const Uint8* keystate, Level& level, std::vector<Texture>& textures, float delta_time);

  void Rotate(float input, float delta_time);
};