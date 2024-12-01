#include<stdio.h>
#include<stdint.h>
#include<malloc.h>
#include<synchapi.h> // Windows header file that has "Sleep"

#define get_cell(game, x, y) (game->cells[(y) * game->width + (x)])
#define set_cell(game, x, y, val) (game->cells[(y) * game->width + (x)] = (val))
#define set_next(game, x, y, val) (game->next_cells[(y) * game->width + (x)] = (val))

typedef struct Game {
    int width;
    int height;
    uint8_t* cells;
    uint8_t* next_cells;
} Game;

Game* new_game(int width, int height) {
    Game* game = (Game*) malloc(sizeof(Game));
    game->width = width;
    game->height = height;
    game->cells = (uint8_t*)malloc(sizeof(uint8_t) * width * height);
    game->next_cells = (uint8_t*)malloc(sizeof(uint8_t) * width * height);
    for(int i = 0; i < (height * width); i++) {
        game->cells[i] = 0;
        game->next_cells[i] = 0;
    }
    return game;
}

void game_print(Game* game, int should_clear) {
    if(should_clear) {
        printf("\e[%iA", game->height); // Go up height times.
        printf("\e[%iD", game->width); // Go left width times.
    }
    for (int y = 0; y < game->height; y++) {
        for (int x = 0; x < game->width; x++) {
            if (get_cell(game, x, y) == 1) {
                printf("X");
            } else {
                printf("_");
            }
        }
        printf("\n");
    }
}

int game_count_alive_neighbours(Game* game, int x, int y, int x2, int y2) {
    int count = 0;

    if (y > 0) {
        if (x > 0) count += get_cell(game, x - 1, y - 1);
        count += get_cell(game, x - 0, y - 1);
        if (x < x2) count += get_cell(game, x + 1, y - 1);
    }
    if (x > 0) count += get_cell(game, x - 1, y - 0);
    if (x < x2) count += get_cell(game, x + 1, y - 0);

    if (y < y2) {
        if (x > 0) count += get_cell(game, x - 1, y + 1);
        count += get_cell(game, x - 0, y + 1);
        if (x < x2) count += get_cell(game, x + 1, y + 1);
    }

    return count;
}

void game_tick(Game* game) {
    int x2 = game->width - 1;
    int y2 = game->height - 1;

    for (int y = 0; y < game->height; y++) {
        for (int x = 0; x < game->width; x++) {
            int current_cell = get_cell(game, x, y);
            int alive_neighbours = game_count_alive_neighbours(game, x, y, x2, y2);

            if (current_cell && (alive_neighbours < 2)) {
                set_next(game, x, y, 0);
                continue;
            }
            if (current_cell && (alive_neighbours > 3)) {
                set_next(game, x, y, 0);
                continue;
            }
            if (!current_cell && (alive_neighbours == 3)) {
                set_next(game, x, y, 1);
                continue;
            }
        }
    }
    for (int i = 0; i < (game->width * game->height); i++) {
        game->cells[i] = game->next_cells[i];
    }
}

void main() {
    Game* game = new_game(50, 25);
    printf("Created game.\n");

    #define alive(x, y)\
        set_cell(game, x, y, 1);\
        set_next(game, x, y, 1);\

    alive(5, 0);
    alive(5, 1);
    alive(5, 2);
    alive(4, 2);
    alive(3, 1);

    alive(10, 6);
    alive(10, 7);
    alive(10, 8);

    printf("Set initial seed.\n");

    printf("\e[?25l"); // Disable cursor.
    game_print(game, 0);
    for (int i = 0; i < 100; i++) {
        Sleep(200);
        game_tick(game);
        game_print(game, 1);
    }
}