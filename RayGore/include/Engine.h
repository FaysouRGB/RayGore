#pragma once

#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <stb_image.h>
#include <Windows.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "Player.h"
#include "Texture.h"

class Engine {
 private:
  std::string engine_folder;
  Player player;
  TTF_Font* font;
  SDL_Window* window;
  SDL_Renderer* renderer;
  SDL_Texture* render_texture;
  SDL_Texture* ui_texture;

  SDL_Rect render_rect;

  Mix_Music* music;

  int width;
  int height;

  int render_width;
  int render_height;

  std::vector<Texture> textures;
  std::vector<Level> levels;
  int selected_level;

  float aspect_ratio = 4.0f / 3.0f;
  float resolution;
  int target_fps;

 public:
  Engine(int width, int height, float aspect_ratio, float resolution, int fps, std::string folderd);

  void LoadAudio();

  void DrawUI();

  void UpdateRenderDimensions();

  void LoadMaps();

  void Start();

  void HandleEvents(bool* shouldQuit, float delta_time);

  void LoadTextures();

  void ReleaseTextures();

  void RenderWorld();

  void BindPlayer(Player player);

  void Reload();
};
