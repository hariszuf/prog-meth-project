// game.h - core game logic and AI control
#ifndef GAME_H
#define GAME_H

#include "model_config.h"

// Game state for 3x3 Tic-Tac-Toe
typedef struct {
    char b[9];     // board cells
    char turn;     // 'X' or 'O'
    int  winner;   // 0 none, 1 X, 2 O, 3 draw
} Game;

// Initialise new game
void game_init(Game *g);

// Reset game (same as init)
void game_reset(Game *g);

// Check if board is full
int game_is_full(const Game *g);

// Place move if legal; return 1 if OK
int game_make_move(Game *g, int index);

// Update winner field
void game_check_end(Game *g);

// Load all AI models
void game_load_all_models(void);

// Set AI config for difficulty levels
void game_set_ai_config(const AIConfig *config);

// Get AI config
void game_get_ai_config(AIConfig *config);

// Reload specific AI model
void game_load_model_file(AIModelType model_type, const char *path);

// AI plays move for O
void game_ai_move(Game *g, int level);

// Get AI name for level
const char* game_get_ai_name(int level);

#endif
