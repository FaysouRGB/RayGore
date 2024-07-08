#include "Engine.h"

Engine::Engine(int width, int height, float aspect_ratio, float resolution, int fps, std::string folder)
    : player(0.0f, 0.0f, 0.0f, 0.0f, 0.0f) {
  this->width = width;
  this->height = height;
  this->render_width = static_cast<int>(width * resolution);
  this->render_height = static_cast<int>(height * resolution);
  this->resolution = resolution;
  this->aspect_ratio = aspect_ratio;
  engine_folder = folder;
  target_fps = fps;
  selected_level = 0;

  this->player = player;

  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
  TTF_Init();
  Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

  char font[MAX_PATH];
  snprintf(font, MAX_PATH, "%s\\fonts\\alagard.ttf", engine_folder.c_str());

  this->font = TTF_OpenFont(font, 24);

  window = SDL_CreateWindow("RayGore Engine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height,
                            SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_FULLSCREEN_DESKTOP);
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  render_texture =
      SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, render_width, render_height);
  ui_texture =
      SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, render_width, render_height);
  LoadTextures();
  LoadMaps();
  LoadAudio();
  UpdateRenderDimensions();
}

void Engine::LoadAudio() {
  char files[MAX_PATH];
  snprintf(files, MAX_PATH, "%s\\audio\\ambiant.mp3", engine_folder.c_str());
  music = Mix_LoadMUS(files);
  Mix_PlayMusic(music, -1);
}

void Engine::DrawUI() {
  SDL_Color color = {255, 255, 255};
  SDL_Surface* surface = TTF_RenderText_Blended(font, "You feel like something is watching you...", color);
  if (ui_texture) SDL_DestroyTexture(ui_texture);
  ui_texture = SDL_CreateTextureFromSurface(renderer, surface);

  SDL_Rect ui_rect;
  ui_rect.w = surface->w;
  ui_rect.h = surface->h;
  ui_rect.x = (width - ui_rect.w) / 2;
  ui_rect.y = (height - ui_rect.h) - height / 10;

  SDL_FreeSurface(surface);

  // Use ui_rect when rendering the UI
  SDL_RenderCopy(renderer, ui_texture, NULL, &ui_rect);
}

void Engine::UpdateRenderDimensions() {
  float window_aspect_ratio = static_cast<float>(width) / static_cast<float>(height);

  if (window_aspect_ratio > aspect_ratio) {
    render_width = static_cast<int>(height * resolution * aspect_ratio);
    render_height = height * resolution;
  } else {
    render_width = width * resolution;
    render_height = static_cast<int>(width * resolution / aspect_ratio);
  }

  render_rect.x = (width - height * aspect_ratio) / 2;
  render_rect.y = (height - width / aspect_ratio) / 2;
  render_rect.w = height * aspect_ratio;
  render_rect.h = width / aspect_ratio;

  if (render_texture) SDL_DestroyTexture(render_texture);

  render_texture =
      SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, render_width, render_height);
}

void Engine::LoadMaps() {
  char files[MAX_PATH];
  snprintf(files, MAX_PATH, "%s\\levels\\*", engine_folder.c_str());

  WIN32_FIND_DATAA find_data;
  HANDLE h_find;

  std::string full_path = std::string(files);
  std::vector<std::string> dir_list;

  h_find = FindFirstFileA(full_path.c_str(), &find_data);
  dir_list.push_back(std::string(find_data.cFileName));

  full_path.pop_back();
  while (FindNextFileA(h_find, &find_data)) {
    dir_list.push_back(full_path + std::string(find_data.cFileName));
  }

  FindClose(h_find);

  dir_list.erase(std::remove_if(dir_list.begin(), dir_list.end(),
                                [](const std::string& name) {
                                  return name == ".." || name == "." || strstr(name.c_str(), ".txt") == nullptr;
                                }),
                 dir_list.end());

  std::sort(dir_list.begin(), dir_list.end());

  for (auto& file : dir_list) {
    Level level(file);
    levels.push_back(level);
  }
}

void Engine::Start() {
  bool shouldQuit = false;

  Uint32 frame_time = 0;
  Uint32 frame_start = 0;
  float delta_time = 0.0f;
  Uint32 target_frame_time = 1000 / target_fps;

  SDL_SetRelativeMouseMode(SDL_TRUE);

  while (!shouldQuit) {
    frame_start = SDL_GetTicks();

    HandleEvents(&shouldQuit, delta_time);

    SDL_RenderClear(renderer);

    RenderWorld();

    DrawUI();

    SDL_RenderPresent(renderer);

    frame_time = SDL_GetTicks() - frame_start;

    if (target_fps != -1 && frame_time < target_frame_time) {
      SDL_Delay(target_frame_time - frame_time);
    }

    delta_time = frame_time / 1000.0f;
  }

  Mix_FreeMusic(music);
  Mix_Quit();
  TTF_Quit();
  SDL_DestroyTexture(render_texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  ReleaseTextures();
}

void Engine::HandleEvents(bool* shouldQuit, float delta_time) {
  SDL_Event event;
  const Uint8* keystate = SDL_GetKeyboardState(NULL);

  if (keystate[SDL_SCANCODE_ESCAPE]) *shouldQuit = true;

  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
      *shouldQuit = true;
    } else if (event.window.type == SDL_WINDOWEVENT_RESIZED || event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
      width = event.window.data1;
      height = event.window.data2;
      UpdateRenderDimensions();
    } else if (event.type == SDL_MOUSEMOTION) {
      player.Rotate(-event.motion.xrel, delta_time);
      SDL_WarpMouseInWindow(window, width / 2, height / 2);
      SDL_PollEvent(nullptr);
    }
  }

  player.Move(keystate, levels[selected_level], textures, delta_time);
  if (keystate[SDL_SCANCODE_R]) Reload();
}

void Engine::LoadTextures() {
  char files[MAX_PATH];
  snprintf(files, MAX_PATH, "%s\\textures\\*", engine_folder.c_str());

  WIN32_FIND_DATAA find_data;
  HANDLE h_find;

  std::string full_path = std::string(files);
  std::vector<std::string> dir_list;

  h_find = FindFirstFileA(full_path.c_str(), &find_data);
  dir_list.push_back(std::string(find_data.cFileName));

  full_path.pop_back();
  while (FindNextFileA(h_find, &find_data)) {
    dir_list.push_back(full_path + std::string(find_data.cFileName));
  }

  FindClose(h_find);

  dir_list.erase(std::remove_if(dir_list.begin(), dir_list.end(),
                                [](const std::string& name) {
                                  return name == ".." || name == "." || strstr(name.c_str(), ".png") == nullptr;
                                }),
                 dir_list.end());

  std::sort(dir_list.begin(), dir_list.end());

  for (auto& file : dir_list) {
    Texture texture(file);
    textures.push_back(texture);
  }
}

void Engine::ReleaseTextures() {
  for (auto& texture : textures) {
    stbi_image_free(texture.data);
  }

  textures.clear();
}

void Engine::RenderWorld() {
  Uint32* pixels;
  int pitch, render_texture_width, render_texture_height;

  SDL_QueryTexture(render_texture, NULL, NULL, &render_texture_width, &render_texture_height);
  SDL_LockTexture(render_texture, NULL, (void**)&pixels, &pitch);
  SDL_memset(pixels, 0, static_cast<size_t>(pitch) * render_texture_height);

  const float inv_width = 1.0f / render_width;
  const int half_height = render_height / 2;

  const float dir_x = player.dir.x, dir_y = player.dir.y;
  const float pos_x = player.pos.x, pos_y = player.pos.y;
  const float plane_x = player.plane.x, plane_y = player.plane.y;

  // Calculate headbob offset
  float headbob_offset = player.headbob_amplitude * sin(player.headbob_timer);

#pragma omp parallel for
  for (int x = 0; x < render_texture_width; x++) {
    float camera_x = 2 * x * inv_width - 1;
    float ray_dir_x = dir_x + plane_x * camera_x;
    float ray_dir_y = dir_y + plane_y * camera_x;
    float ray_dir_z = headbob_offset;
    int map_x = static_cast<int>(pos_x);
    int map_y = static_cast<int>(pos_y);

    float side_dist_x, side_dist_y;

    float delta_dist_x = std::abs(1 / ray_dir_x);
    float delta_dist_y = std::abs(1 / ray_dir_y);

    int step_x, step_y;

    if (ray_dir_x < 0) {
      step_x = -1;
      side_dist_x = (pos_x - map_x) * delta_dist_x;
    } else {
      step_x = 1;
      side_dist_x = (map_x + 1.0 - pos_x) * delta_dist_x;
    }
    if (ray_dir_y < 0) {
      step_y = -1;
      side_dist_y = (player.pos.y - map_y) * delta_dist_y;
    } else {
      step_y = 1;
      side_dist_y = (map_y + 1.0 - pos_y) * delta_dist_y;
    }

    int side;
    std::vector<Hit> hits;
    bool hit_solid = false;
    while (!hit_solid) {
      if (side_dist_x < side_dist_y) {
        side_dist_x += delta_dist_x;
        map_x += step_x;
        side = 0;
      } else {
        side_dist_y += delta_dist_y;
        map_y += step_y;
        side = 1;
      }

      int wall_type = levels[selected_level].data[map_x][map_y];
      if (wall_type > 0) {
        float perp_wall_dist = (side == 0) ? (map_x - pos_x + (1 - step_x) / 2.0f) / ray_dir_x
                                           : (map_y - pos_y + (1 - step_y) / 2.0f) / ray_dir_y;
        Hit hit = {wall_type - 1, perp_wall_dist, side};
        hits.push_back(hit);

        if (!textures[wall_type - 1].transparent) {
          hit_solid = true;
        }
      }
    }

    std::reverse(hits.begin(), hits.end());

    float camera_height = render_height / 2.0f + headbob_offset * render_height;

    for (const auto& hit : hits) {
      float perp_wall_dist = hit.dst;
      int tex_index = hit.wall_type;
      int side = hit.side;

      int line_height = static_cast<int>(render_texture_height / perp_wall_dist) * 1.5f;
      int draw_start = -line_height / 2 + half_height;
      int draw_end = line_height / 2 + half_height;

      draw_start += static_cast<int>(ray_dir_z * render_height);
      draw_end += static_cast<int>(ray_dir_z * render_height);

      if (draw_start < 0) draw_start = 0;
      if (draw_end >= render_texture_height) draw_end = render_texture_height - 1;

      float wall_x;
      if (side == 0) {
        wall_x = player.pos.y + perp_wall_dist * ray_dir_y;
      } else {
        wall_x = player.pos.x + perp_wall_dist * ray_dir_x;
      }
      wall_x -= floor(wall_x);

      int tex_x = int(wall_x * textures[tex_index].width);
      if ((side == 0 && ray_dir_x > 0) || (side == 1 && ray_dir_y < 0)) {
        tex_x = textures[tex_index].width - tex_x - 1;
      }

      float step = 1.0f * textures[tex_index].height / line_height;
      float tex_pos = (draw_start - half_height - headbob_offset * render_height + line_height / 2.0f) * step;

      float shade = 1.0f / perp_wall_dist;
      if (shade > 1.0f) shade = 1.0f;

      for (int y = draw_start; y < draw_end; y++) {
        int tex_y = static_cast<int>(tex_pos) % textures[tex_index].height;
        tex_pos += step;

        unsigned char* pixel_offset = textures[tex_index].data + (tex_y * textures[tex_index].width + tex_x) * 4;
        unsigned char r = pixel_offset[0];
        unsigned char g = pixel_offset[1];
        unsigned char b = pixel_offset[2];
        unsigned char a = pixel_offset[3];

        if (a == 0) continue;

        r = static_cast<unsigned char>(r * shade);
        g = static_cast<unsigned char>(g * shade);
        b = static_cast<unsigned char>(b * shade);

        if (textures[tex_index].transparent) {
          Uint32 existing_color = pixels[y * render_texture_width + x];
          float alpha = a / 255.0f;

          unsigned char existing_r = (existing_color >> 24) & 0xFF;
          unsigned char existing_g = (existing_color >> 16) & 0xFF;
          unsigned char existing_b = (existing_color >> 8) & 0xFF;

          r = static_cast<unsigned char>(r * alpha + existing_r * (1 - alpha));
          g = static_cast<unsigned char>(g * alpha + existing_g * (1 - alpha));
          b = static_cast<unsigned char>(b * alpha + existing_b * (1 - alpha));
        }

        pixels[y * render_texture_width + x] = (r << 24) | (g << 16) | (b << 8) | 0xFF;
      }
    }
  }

  SDL_UnlockTexture(render_texture);
  SDL_RenderCopy(renderer, render_texture, NULL, &render_rect);
}

void Engine::BindPlayer(Player player) { this->player = player; }

void Engine::Reload() {
  ReleaseTextures();
  levels.clear();
  LoadMaps();
  LoadTextures();
}