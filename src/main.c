#include<stdio.h>
#include<SDL2/SDL.h>
#include<gol.h>
#include<sdl_game.h>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 500;
const int framerate = 30;
const int update_every_tick = 10;
const int original_cell_length = 26;
int cell_length = 26;

GOL_SDL* gol_sdl;
int tick = 0;

void on_exit() {
    SDL_DestroyWindow(gol_sdl->window);
    SDL_Quit();
}

void handle_events() {
    SDL_Event e;
    Game* game = gol_sdl->gol;

    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            gol_sdl->running = 0;
            return;
        }
        if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
                case SDLK_p:
                    gol_sdl->updating = !gol_sdl->updating;
                    break;
                case SDLK_w:
                    gol_sdl->cell_y -= 1;
                    if (gol_sdl->cell_y < 0) {
                        gol_sdl->cell_y = 0;
                    }
                    break;
                case SDLK_s:
                    gol_sdl->cell_y += 1;
                    if (gol_sdl->cell_y == game->height) {
                        gol_sdl->cell_y = game->height - 1;
                    }
                    break;
                case SDLK_a:
                    gol_sdl->cell_x -= 1;
                    if (gol_sdl->cell_x < 0) {
                        gol_sdl->cell_x = 0;
                    }
                    break;
                case SDLK_d:
                    gol_sdl->cell_x += 1;
                    if (gol_sdl->cell_x == game->width) {
                        gol_sdl->cell_x = game->width - 1;
                    }
                    break;
                case SDLK_PLUS:
                case SDLK_KP_PLUS:
                    gol_sdl->squish_factor += 1;
                    cell_length = original_cell_length / gol_sdl->squish_factor;
                    break;
                case SDLK_MINUS:
                case SDLK_KP_MINUS:
                    gol_sdl->squish_factor -= 1;
                    if (gol_sdl->squish_factor == 0) {
                        gol_sdl->squish_factor = 1;
                    }
                    cell_length = original_cell_length / gol_sdl->squish_factor;
                    break;
            }
        }
    }
}

void render() {
    SDL_Renderer* renderer = gol_sdl->renderer;
    Game* gol = gol_sdl->gol;
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

    for (int gy = 0; gy < SCREEN_HEIGHT; gy += (cell_length + 4)) {
        for (int gx = 0; gx < SCREEN_WIDTH; gx += (cell_length + 4)) {
            SDL_Rect rect = {
                gx+2, 
                gy+2, 
                cell_length, 
                cell_length
            };
            SDL_RenderDrawRect(
                renderer, 
                &rect
            );
            int cell_y = gy / (cell_length + 4) + gol_sdl->cell_y;
            int cell_x = gx / (cell_length + 4) + gol_sdl->cell_x;

            if (gol->cells[cell_y * gol->width + cell_x]) {
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }

    SDL_RenderPresent(renderer);
}

int main(int argc, char* args[]) {
    printf("[DEBUG]: Starting program.\n");

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("[SDL Error]: %s\n", SDL_GetError());
        return 1;
    }
    printf("[DEBUG]: Initialized SDL.\n");

    SDL_Window* window = SDL_CreateWindow(
        "Game of Life",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        0
    );

    if (window == NULL) {
        printf("[DEBUG]: Failed to create window.\n");
        printf("[SDL Error]: %s\n", SDL_GetError());
        return 1;
    }

    gol_sdl = (GOL_SDL*) malloc(sizeof(GOL_SDL));
    gol_sdl->running = 1;
    gol_sdl->window = window;
    gol_sdl->renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    gol_sdl->gol = new_game(60, 60);
    gol_sdl->updating = 1;
    gol_sdl->cell_x = 0;
    gol_sdl->cell_y = 0;
    gol_sdl->squish_factor = 1;

    set_cell(gol_sdl->gol, 5, 0, 1);
    set_cell(gol_sdl->gol, 5, 1, 1);
    set_cell(gol_sdl->gol, 5, 2, 1);
    set_cell(gol_sdl->gol, 4, 2, 1);
    set_cell(gol_sdl->gol, 3, 1, 1);

    set_next(gol_sdl->gol, 5, 0, 1);
    set_next(gol_sdl->gol, 5, 1, 1);
    set_next(gol_sdl->gol, 5, 2, 1);
    set_next(gol_sdl->gol, 4, 2, 1);
    set_next(gol_sdl->gol, 3, 1, 1);


    atexit(on_exit);

    while (gol_sdl->running) {
        handle_events();
        if (tick == update_every_tick) {
            if (gol_sdl->updating) game_tick(gol_sdl->gol);
            tick = 0;
        }
        tick++;
        render();
        SDL_Delay(1000 / framerate);
    }

    return 0;
}