#include<stdio.h>
#include<SDL2/SDL.h>
#include<gol.h>
#include<sdl_game.h>

int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 500;
const int framerate = 30;
const int update_every_tick = 10;
const int original_cell_length = 26;
int cell_length = 26;
int mouse_x;
int mouse_y;
int shift_held = 0;
int shift_factor = 5;
int show_grid = 1;

#define grid_move_speed ((shift_held ? 5 : 1) * (gol_sdl->squish_factor))

const int BACKGROUND_COLOR   = 0x111111;
const int CELL_BORDER_COLOR  = 0x444444;
const int CELL_ALIVE_COLOR   = 0xCCCCCC;
const int CELL_DEAD_COLOR    = 0x000000;
const int CELL_HOVERED_COLOR = 0x555555;

#define RGB_R(val) (((val) & 0xFF0000) >> 16)
#define RGB_G(val) (((val) & 0x00FF00) >> 8)
#define RGB_B(val) (((val) & 0x0000FF) >> 0)

GOL_SDL* gol_sdl;
int tick = 0;
int resized = 0;

void on_exit() {
    SDL_DestroyWindow(gol_sdl->window);
    SDL_Quit();
}

void mark_cell_by_coords(int x, int y) {
    Game* gol = gol_sdl->gol;
    int cx = x / (cell_length + 4) + gol_sdl->cell_x;
    int cy = y / (cell_length + 4) + gol_sdl->cell_y;

    if (cx > gol->width) return;
    if (cy > gol->height) return;

    int index = cy * gol->width + cx;
    gol->cells[index] = !gol->cells[index];
    gol->next_cells[index] = gol->cells[index];
}

void handle_events() {
    SDL_Event e;
    Game* game = gol_sdl->gol;

    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            gol_sdl->running = 0;
            return;
        }
        if (e.type == SDL_WINDOWEVENT) {
            if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
                resized = 1;
            }
        }
        if ((e.type == SDL_MOUSEBUTTONDOWN) || (e.type == SDL_MOUSEMOTION)) {
            SDL_GetMouseState(&mouse_x, &mouse_y);
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                mark_cell_by_coords(mouse_x, mouse_y);
            }
        }
        if (e.type == SDL_KEYUP) {
            switch (e.key.keysym.sym) {
                case SDLK_LSHIFT:
                    shift_held = 0;
                    break;
            }
        }
        if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
                case SDLK_LSHIFT:
                    shift_held = 1;
                    break;
                case SDLK_g:
                    show_grid = !show_grid;
                    break;
                case SDLK_SPACE:
                case SDLK_p:
                    gol_sdl->updating = !gol_sdl->updating;
                    break;
                case SDLK_w:
                    gol_sdl->cell_y -= grid_move_speed;
                    if (gol_sdl->cell_y < 0) {
                        gol_sdl->cell_y = 0;
                    }
                    break;
                case SDLK_s:
                    gol_sdl->cell_y += grid_move_speed;
                    if (gol_sdl->cell_y == game->height) {
                        gol_sdl->cell_y = game->height - 1;
                    }
                    break;
                case SDLK_a:
                    gol_sdl->cell_x -= grid_move_speed;
                    if (gol_sdl->cell_x < 0) {
                        gol_sdl->cell_x = 0;
                    }
                    break;
                case SDLK_d:
                    gol_sdl->cell_x += grid_move_speed;
                    if (gol_sdl->cell_x == game->width) {
                        gol_sdl->cell_x = game->width - 1;
                    }
                    break;
                case SDLK_e:
                case SDLK_PLUS:
                case SDLK_KP_PLUS:
                    gol_sdl->squish_factor += 1;
                    cell_length = original_cell_length / gol_sdl->squish_factor;
                    break;
                case SDLK_q:
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
    SDL_SetRenderDrawColor(
        renderer,
        RGB_R(BACKGROUND_COLOR),
        RGB_G(BACKGROUND_COLOR),
        RGB_B(BACKGROUND_COLOR),
        0xFF
    );
    SDL_RenderClear(renderer);


    for (int gy = 0; gy < SCREEN_HEIGHT; gy += (cell_length + 4)) {
        for (int gx = 0; gx < SCREEN_WIDTH; gx += (cell_length + 4)) {
            int cell_y = gy / (cell_length + 4) + gol_sdl->cell_y;
            int cell_x = gx / (cell_length + 4) + gol_sdl->cell_x;

            if (cell_y > gol->height) continue;
            if (cell_x > gol->width) continue;

            SDL_Rect rect = {
                gx + 2,
                gy + 2,
                cell_length, 
                cell_length
            };
            if (show_grid) {
                SDL_SetRenderDrawColor(
                    renderer,
                    RGB_R(CELL_BORDER_COLOR),
                    RGB_G(CELL_BORDER_COLOR),
                    RGB_B(CELL_BORDER_COLOR),
                    0xFF
                );
                SDL_RenderDrawRect(
                    renderer, 
                    &rect
                );
            }

            rect.x += 1;
            rect.y += 1;
            rect.w -= 2;
            rect.h -= 2;

            int should_render = 0;
            if (gol->cells[cell_y * gol->width + cell_x]) {
                SDL_SetRenderDrawColor(
                    renderer,
                    RGB_R(CELL_ALIVE_COLOR),
                    RGB_G(CELL_ALIVE_COLOR),
                    RGB_B(CELL_ALIVE_COLOR),
                    0xFF
                );
                should_render = 1;
                //SDL_RenderFillRect(renderer, &rect);
            } else {
                if ((gx < mouse_x) && (mouse_x < (gx + cell_length)) && (gy < mouse_y) && (mouse_y < (gy + cell_length))) {
                    SDL_SetRenderDrawColor(
                        renderer,
                        RGB_R(CELL_HOVERED_COLOR),
                        RGB_G(CELL_HOVERED_COLOR),
                        RGB_B(CELL_HOVERED_COLOR),
                        0xFF
                    );
                    should_render = 1;
                }else {
                    if (show_grid) {
                        should_render = 1;
                        SDL_SetRenderDrawColor(
                            renderer,
                            RGB_R(CELL_DEAD_COLOR),
                            RGB_R(CELL_DEAD_COLOR),
                            RGB_R(CELL_DEAD_COLOR),
                            0xFF
                        );
                    } else {
                        should_render = 0;
                    }
                }
            }
            if (should_render) SDL_RenderFillRect(renderer, &rect);
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
        SDL_WINDOW_RESIZABLE
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
    gol_sdl->gol = new_game(1000, 1000);
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
        if (resized) {
            SDL_GetWindowSize(gol_sdl->window, &SCREEN_WIDTH, &SCREEN_HEIGHT);
            SDL_DestroyRenderer(gol_sdl->renderer);
            gol_sdl->renderer = SDL_CreateRenderer(gol_sdl->window, -1, SDL_RENDERER_ACCELERATED);
            resized = 0;
        }
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