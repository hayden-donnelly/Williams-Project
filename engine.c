#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_Image.h>
#include <string.h>
#include "engine.h"   
#include "game.h"

Uint8 input_previous_keyboard_state[512];
Uint8 input_current_keyboard_state[512];
Uint8 *input_keyboard_state_pointer;

SDL_Renderer *renderer = NULL;
SDL_Window *window = NULL;

int main(int argc, const char *argv[]) 
{
    window_width = 1920;
    window_height = 1080;
    render_width = 1280;
    render_height = 720;

    // Init SDL
    if(SDL_Init(SDL_INIT_EVERYTHING) == -1)
    {
        printf("Failed to initialize SDL: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    // Create window
    window = SDL_CreateWindow("Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
                                window_width, window_height, SDL_WINDOW_MAXIMIZED);
    if(window == NULL)
    {
        printf("Failed to create window: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    // Create renderer
    renderer = SDL_CreateRenderer(window, -1, 0);
    if(renderer == NULL)
    {
        printf("Failed to create renderer %s\n", SDL_GetError());
    }
    // Set size of renderer to the same as window
    SDL_RenderSetLogicalSize(renderer, render_width, render_height);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    // Init timer
    double timer_now = SDL_GetPerformanceCounter();
    double timer_last = 0;
    double timer_dt = 0;
    double timer_fixed_dt = 1.0/60.0;
    double timer_accumulator = 0;

    game_load();

    // Main loop
    input_keyboard_state_pointer = SDL_GetKeyboardState(NULL);
    game_is_running = true;
    SDL_Event event;
    while (game_is_running) 
    {
        // Update timer
        timer_last = timer_now;
        timer_now = SDL_GetPerformanceCounter();
        timer_dt = ((timer_now - timer_last)/(double)SDL_GetPerformanceFrequency());
        timer_accumulator += timer_dt;

        // Update game
        while (timer_accumulator >= timer_fixed_dt) 
        {
            // Handle all events, making sure previous and current input states are updated properly
            memcpy(input_previous_keyboard_state, input_current_keyboard_state, 512);
            while (SDL_PollEvent(&event)) 
            {
                if(event.type == SDL_QUIT) 
                {
                    game_is_running = false;
                    break;
                }
            }
            memcpy(input_current_keyboard_state, input_keyboard_state_pointer, 512);

            game_update(timer_dt);
            timer_accumulator -= timer_fixed_dt;
        }

        // Draw game
        SDL_RenderClear(renderer);
        game_render(renderer);
        SDL_RenderPresent(renderer);
    }

    game_unload();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}

// Returns true if key was pressed this frame
bool input_get_key_down(SDL_Scancode scancode)
{
    if(input_current_keyboard_state[scancode] && !input_previous_keyboard_state[scancode])
    {
        return true;
    }
    return false;
}

// Returns true of key is pressed
bool input_get_key(SDL_Scancode scancode)
{
    if(input_current_keyboard_state[scancode])
    {
        return true;
    }
    return false;
}

// Returns true if key was released this frame
bool input_get_key_up(SDL_Scancode scancode)
{
    if(!input_current_keyboard_state[scancode] && input_previous_keyboard_state[scancode])
    {
        return true;
    }
    return false;
}

// Loads image to a texture
SDL_Texture *load_image_to_texture(char **path)
{
    SDL_Texture *texture = IMG_LoadTexture(renderer, path);
    return texture;
}