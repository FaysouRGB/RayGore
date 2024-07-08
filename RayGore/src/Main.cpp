#include "Engine.h"

int main(int argc, char* args[]) {
  char engine_path[MAX_PATH];
  strcpy_s(engine_path, args[0]);
  char* last = strrchr(engine_path, '\\');
  *last = '\0';

  Engine engine(800, 600, 4.0f / 3.0f, 0.5f, 60, engine_path);

  Player player(12.0f, 12.0f, 10.0f, 20.0f, 66.8f);
  engine.BindPlayer(player);

  engine.Start();

  return 0;
}