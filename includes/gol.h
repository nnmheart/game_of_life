#pragma once
#include<stdint.h>

#define get_cell(game, x, y) (game->cells[(y) * game->width + (x)])
#define set_cell(game, x, y, val) (game->cells[(y) * game->width + (x)] = (val))
#define set_next(game, x, y, val) (game->next_cells[(y) * game->width + (x)] = (val))

typedef struct Game {
    int width;
    int height;
    uint8_t* cells;
    uint8_t* next_cells;
} Game;

Game* new_game(int width, int height);
void game_tick(Game* game);