#pragma once
#include<gol.h>
#include<SDL2/SDL.h>

typedef struct {
    int running;
    SDL_Window* window;
    SDL_Renderer* renderer;
    Game* gol;
    int updating;
} GOL_SDL;