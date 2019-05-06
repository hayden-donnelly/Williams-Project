#pragma once

#include <SDL2/SDL.h>

void game_load();
void game_update(double dt);
void game_render(SDL_Renderer *renderer);

#pragma endregion