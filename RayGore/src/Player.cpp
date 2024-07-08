#include "Player.h"

Player::Player(float x, float y, float speed, float sensitivity, float FOV_degrees) {
  // Speeds
  this->speed = speed;
  this->sensitivity = sensitivity;

  // Vectors
  pos = {x, y};
  dir = {-1.0f, 0.0f};
  plane = {0.0f, std::tanf(FOV_degrees / 2.0f * M_PI / 180.0f)};
}

void Player::Move(const Uint8* keystate, Level& level, std::vector<Texture>& textures, float delta_time) {
  // Create the move vector
  Vector2 move = {0.0f, 0.0f};

  // Check each keys
  if (keystate[SDL_SCANCODE_W]) move += dir;
  if (keystate[SDL_SCANCODE_S]) move -= dir;
  if (keystate[SDL_SCANCODE_A]) move += {-dir.y, dir.x};
  if (keystate[SDL_SCANCODE_D]) move += {dir.y, -dir.x};

  // If no inputs, exit
  if (!move.x && !move.y) {
    headbob_timer = 0.0f;
    return;
  }

  // Normalize the vector
  move.normalize();

  // Compute new position
  Vector2 new_pos;
  if (keystate[SDL_SCANCODE_LSHIFT]) {
    new_pos = pos + move * 2 * speed * delta_time;
    headbob_timer += 2 * delta_time * headbob_frequency;
  } else {
    new_pos = pos + move * speed * delta_time;
    headbob_timer += delta_time * headbob_frequency;
  }

  // Apply if valid per axis
  int new_map_x = level.data[static_cast<int>(new_pos.x)][static_cast<int>(pos.y)];
  int new_map_y = level.data[static_cast<int>(pos.x)][static_cast<int>(new_pos.y)];

  if (!new_map_x || !textures[new_map_x - 1].has_collisions) pos.x = new_pos.x;
  if (!new_map_y || !textures[new_map_y - 1].has_collisions) pos.y = new_pos.y;
}

void Player::Rotate(float input, float delta_time) {
  dir.rotate(input * sensitivity * delta_time);
  plane.rotate(input * sensitivity * delta_time);
}