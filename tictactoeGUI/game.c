// game.c — engine impl
#include "game.h"

// AI (from minimax.c)
int findBestMoveLvl(char b[9], int level);
int winBy(char b[9], char p);

static void board_init(char b[9]) {
    for (int i = 0; i < 9; i++) {
        b[i] = '1' + i;
    }
}

void game_init(Game *g) {
    board_init(g->b);
    g->turn = 'X';
    g->winner = 0;
}

void game_reset(Game *g) {
    game_init(g);
}

int game_is_full(const Game *g) {
    for (int i = 0; i < 9; i++) {
        if (g->b[i] != 'X' && g->b[i] != 'O') {
            return 0;
        } else {
            continue;
        }
    }
    return 1;
}

int game_make_move(Game *g, int idx) {
    if (idx < 0 || idx > 8) {
        return 0;
    } else {
        if (g->b[idx] == 'X' || g->b[idx] == 'O') {
            return 0;
        } else {
            g->b[idx] = g->turn;
            if (g->turn == 'X') {
                g->turn = 'O';
            } else {
                g->turn = 'X';
            }
            return 1;
        }
    }
}

void game_check_end(Game *g) {
    if (winBy(g->b, 'X')) {
        g->winner = 1;
        return;
    } else {
        if (winBy(g->b, 'O')) {
            g->winner = 2;
            return;
        } else {
            if (game_is_full(g)) {
                g->winner = 3;
                return;
            } else {
                g->winner = 0;
            }
        }
    }
}

void game_ai_move(Game *g, int level) {
    if (g->turn != 'O') {
        return;
    } else {
        int mv = findBestMoveLvl(g->b, level); // expect 0..8

        if (mv >= 0 && mv < 9 && g->b[mv] != 'X' && g->b[mv] != 'O') {
            g->b[mv] = 'O';
            g->turn = 'X';
        } else {
            // fallback: first free
            for (int i = 0; i < 9; i++) {
                if (g->b[i] != 'X' && g->b[i] != 'O') {
                    g->b[i] = 'O';
                    g->turn = 'X';
                    break;
                } else {
                    continue;
                }
            }
        }
    }
}
