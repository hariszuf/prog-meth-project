// game.c — game engine implementation (no printing here)
#include "game.h"            // bring in Game struct and prototypes
#include "minimax.h"        // AI helper functions
#include "naive_bayes_ai.h" // Naive Bayes AI
// #include "linear_regression_ai.h" // Linear Regression AI (available for future use)

// Global AI models (loaded once at startup)
static NaiveBayesModel nb_model;
static int nb_model_loaded = 0;
// Linear Regression model available but not loaded
// static LinearRegressionModel lr_model;
// static int lr_model_loaded = 0;

void game_init(Game *g)
{
    for (int i = 0; i < 9; i++)
    {
        g->b[i] = ' ';
    }
    g->turn = 'X';
    g->winner = 0;
}




// Public: reset game (same behavior as init for simplicity)
void game_reset(Game *g)
{
    game_init(g);                           // reuse initialization
}

// Public: check if there is any empty cell left
int game_is_full(const Game *g)
{
    for (int i = 0; i < 9; i++)                 // scan all cells
    {
        if (g->b[i] != 'X' && g->b[i] != 'O') // if cell is not taken
        {
            return 0;                       // not full yet
        }
    }
    return 1;                               // all cells are taken
}

// Public: place current player's mark at index (0..8) if legal
int game_make_move(Game *g, int index)
{
    if (index < 0 || index > 8)                 // index out of range?
    {
        return 0;                           // reject
    }

    if (g->b[index] == 'X' || g->b[index] == 'O') // already occupied?
    {
        return 0;                           // reject
    }

    g->b[index] = g->turn;                    // write current player's mark

    // switch turn to the other player (expanded if/else version)
    if (g->turn == 'X')
    {
        g->turn = 'O';                      // after X, now O moves
    }
    else 
    {
        g->turn = 'X';                      // after O, now X moves
    }

    return 1;                               // move was successful
}

// Public: set winner state based on current board
void game_check_end(Game *g) {
    if (winBy(g->b, 'X')) {
        g->winner = 1;
    } 
    else if (winBy(g->b, 'O')) {
        g->winner = 2;
    } 
    else if (game_is_full(g)) {
        g->winner = 3;
    } 
    else {
        g->winner = 0;
    }
}


// Load the Naive Bayes model (call this once at startup)
void game_load_nb_model(const char *model_path)
{
    if (!nb_model_loaded) {
        if (nb_load_model(model_path, &nb_model)) {
            nb_model_loaded = 1;
        }
    }
}

// Load the Linear Regression model (call this once at startup)
// Commented out - available for future use
/*
void game_load_lr_model(const char *model_path)
{
    if (!lr_model_loaded) {
        if (lr_load_model(model_path, &lr_model)) {
            lr_model_loaded = 1;
        }
    }
}
*/

// Public: if it's O's turn, ask AI to play based on level (1..3)
void game_ai_move(Game *g, int level)
{

    if (g->turn != 'O')                     // not O's turn?
    {
        return;                             // do nothing
    }

    int mv = -1;

    // Level 1 = Easy = Naive Bayes AI
    if (level == 1 && nb_model_loaded)
    {
        mv = nb_find_best_move(&nb_model, g->b);
    }
    // Level 2 = Medium = Minimax with depth limit (original level 2)
    else if (level == 2)
    {
        mv = findBestMoveLvl(g->b, 2);
    }
    // Level 3 = Hard = Full Minimax (original level 3)
    else if (level == 3)
    {
        mv = findBestMoveLvl(g->b, 3);
    }
    else
    {
        // Fallback to original minimax if Naive Bayes not loaded
        mv = findBestMoveLvl(g->b, level);
    }

    // If AI returned a valid empty cell, use it
    if (mv >= 0 && mv < 9 && g->b[mv] != 'X' && g->b[mv] != 'O')
    {
        g->b[mv] = 'O';             // place O
        g->turn = 'X';              // switch turn to X
        return;                     // done
    }

    // Fallback: choose the first free cell if AI failed
    for (int i = 0; i < 9; i++)
    {
        if (g->b[i] != 'X' && g->b[i] != 'O') // found a free spot
        {
            g->b[i] = 'O';                   // place O
            g->turn = 'X';                   // switch to X
            break;                           // stop searching
        }
    }
}