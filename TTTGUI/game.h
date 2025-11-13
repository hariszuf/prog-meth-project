// game.h — game engine (no user I/O)
#ifndef GAME_H
#define GAME_H

#include "model_config.h"  // AI model configuration

// Simple struct to hold game state
typedef struct {
    char b[9];               // board cells: 'X', 'O', or digits '' when empty
    char turn;               // whose turn: 'X' or 'O'
    int  winner;             // 0 none, 1 X, 2 O, 3 draw
} Game;

// Start a brand new game state
void game_init(Game *g);               // initialize board, set X to start
// Reset is same as init (kept for readability in caller)
void game_reset(Game *g);              // reset board and winner
// Check if there is no more empty cell
int  game_is_full(const Game *g);      // returns 1 if full, else 0
// Place current player's mark at index 0..8 if legal
int  game_make_move(Game *g, int index); // returns 1 if move made, else 0
// Update g->winner based on current board
void game_check_end(Game *g);          // sets winner field

// ===== AI MODEL MANAGEMENT =====
// Load all AI models (call once at startup)
void game_load_all_models(void);

// Set AI configuration (which models to use for each difficulty)
void game_set_ai_config(const AIConfig *config);

// Get current AI configuration
void game_get_ai_config(AIConfig *config);

// Load specific model files (for swapping between dataset variants)
void game_load_model_file(AIModelType model_type, const char *model_path);

// Ask AI (playing 'O') to move according to level 1..3
void game_ai_move(Game *g, int level); // does nothing if not O's turn

// Get the name of the current AI for a given level
const char* game_get_ai_name(int level);

#endif