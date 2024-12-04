#pragma once
#include<gol.h>
#include<SDL2/SDL.h>

typedef struct {
    int running;
    SDL_Window* window;
    SDL_Renderer* renderer;
    Game* gol;
    int updating;
    int cell_x;
    int cell_y;
    int squish_factor;
} GOL_SDL;