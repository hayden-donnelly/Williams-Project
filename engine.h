#pragma once

#include <stdbool.h>

bool game_is_running;

int window_width, window_height;
int render_width, render_height;

bool input_get_key_down(SDL_Scancode scancode);
bool input_get_key(SDL_Scancode scancode);
bool input_get_key_up(SDL_Scancode scancode);

SDL_Texture *load_image_to_texture(char **path);

#pragma endregion