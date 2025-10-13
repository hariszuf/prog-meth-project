// game.h — pure game engine API (no I/O)
#ifndef GAME_H
#define GAME_H

typedef struct {
    char b[9];      // board: 'X','O', or '1'..'9' for empty
    char turn;      // 'X' or 'O'
    int  winner;    // 0=none, 1=X, 2=O, 3=draw
} Game;

void game_init(Game *g);              // start new game (X to move)
void game_reset(Game *g);             // reset board/winner/turn
int  game_is_full(const Game *g);     // 1 if no empty cells
int  game_make_move(Game *g, int idx);// idx 0..8; returns 1 if placed
void game_check_end(Game *g);         // sets g->winner if win/draw
void game_ai_move(Game *g, int level);// AI plays for 'O' with difficulty

#endif
