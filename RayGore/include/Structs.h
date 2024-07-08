#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

struct Vector2 {
  float x = 0.0f;
  float y = 0.0f;

  Vector2& operator+=(const Vector2& other) {
    x += other.x;
    y += other.y;
    return *this;
  }

  Vector2& operator-=(const Vector2& other) {
    x -= other.x;
    y -= other.y;
    return *this;
  }

  Vector2 operator*(float scalar) const { return Vector2{x * scalar, y * scalar}; }

  Vector2 operator+(const Vector2& other) const { return Vector2{x + other.x, y + other.y}; }
  Vector2 operator-(const Vector2& other) const { return Vector2{x - other.x, y - other.y}; }

  void normalize() {
    float magnitude = sqrt(x * x + y * y);
    x /= magnitude;
    y /= magnitude;
  }

  void rotate(float angle_degrees) {
    float radians = angle_degrees * (M_PI / 180.0f);

    float cos_theta = std::cos(radians);
    float sin_theta = std::sin(radians);

    float temp_x = x;

    x = cos_theta * x - sin_theta * y;
    y = sin_theta * temp_x + cos_theta * y;
  }
};

struct Hit {
  int wall_type;
  float dst;
  int side;
};