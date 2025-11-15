// game.c — game engine implementation (no printing here)
#include "game.h"            // bring in Game struct and prototypes
#include "minimax.h"         // AI helper functions
#include "naive_bayes_ai.h"  // Naive Bayes AI
#include "linear_regression_ai.h"  // Linear Regression AI
#include "q_learning_ai.h"   // Q-Learning AI
#include "model_config.h"    // AI configuration
#include <stdio.h>

// Global AI models (loaded once at startup)
static NaiveBayesModel nb_model;
static LinearRegressionModel lr_model;
static QLearningModel ql_model;

static int nb_model_loaded = 0;
static int lr_model_loaded = 0;
static int ql_model_loaded = 0;

// Current AI configuration
static AIConfig current_config;

void game_init(Game *g)
{
    for (int i = 0; i < 9; i++)
    {
        g->b[i] = ' ';
    }
    g->turn = 'X';
    g->winner = 0;
}




// reset game
void game_reset(Game *g)
{
    game_init(g);                          
}

// check if there is any empty cell left
int game_is_full(const Game *g)
{
    for (int i = 0; i < 9; i++)                 // scan all cells
    {
        if (g->b[i] != 'X' && g->b[i] != 'O') // if cell is not taken
        {
            return 0;                       // not full 
        }
    }
    return 1;                               // all cells taken
}

// Public: place current player's mark at index if legal
int game_make_move(Game *g, int index)
{
    if (index < 0 || index > 8)   
    {
        return 0; 
    }

    if (g->b[index] == 'X' || g->b[index] == 'O') // full
    {
        return 0;   
    }

    g->b[index] = g->turn; // write current player's sign

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


// ===== AI MODEL MANAGEMENT =====

// Load all AI models with default paths
void game_load_all_models(void) {
    // Initialize configuration with best models preset
    ai_config_preset_best_models(&current_config);
    
    // Load Naive Bayes model (non-terminal version by default)
    if (!nb_model_loaded) {
        if (nb_load_model("../models/naive_bayes_non_terminal/model_non_terminal.txt", &nb_model)) {
            nb_model_loaded = 1;
            printf("Naive Bayes model loaded\n");
        } else {
            printf("Failed to load Naive Bayes model\n");
        }
    }
    
    // Load Linear Regression model (non-terminal version by default)
    if (!lr_model_loaded) {
        if (lr_load_model("../models/linear_regression_non_terminal/model_non_terminal.txt", &lr_model)) {
            lr_model_loaded = 1;
            printf("Linear Regression model loaded\n");
        } else {
            printf("Failed to load Linear Regression model\n");
        }
    }
    
    // Load Q-Learning model (non-terminal version by default)
    if (!ql_model_loaded) {
        if (ql_load_model("../models/q learning/q_learning_non_terminal.txt", &ql_model)) {
            ql_model_loaded = 1;
            printf("✓ Q-Learning model loaded\n");
        } else {
            printf("✗ Failed to load Q-Learning model\n");
        }
    }
    
    printf("\nCurrent AI Configuration:\n");
    printf("  Easy (Level 1):   %s\n", ai_config_get_model_name(current_config.easy_model));
    printf("  Medium (Level 2): %s\n", ai_config_get_model_name(current_config.medium_model));
    printf("  Hard (Level 3):   %s\n", ai_config_get_model_name(current_config.hard_model));
    printf("\n");
}

// Set AI configuration
void game_set_ai_config(const AIConfig *config) {
    current_config = *config;
    printf("AI Configuration updated:\n");
    printf("  Easy (Level 1):   %s\n", ai_config_get_model_name(current_config.easy_model));
    printf("  Medium (Level 2): %s\n", ai_config_get_model_name(current_config.medium_model));
    printf("  Hard (Level 3):   %s\n", ai_config_get_model_name(current_config.hard_model));
}

// Get current AI configuration
void game_get_ai_config(AIConfig *config) {
    *config = current_config;
}

// Load specific model file (for swapping between dataset variants)
void game_load_model_file(AIModelType model_type, const char *model_path) {
    switch (model_type) {
        case AI_MODEL_NAIVE_BAYES:
            if (nb_load_model(model_path, &nb_model)) {
                nb_model_loaded = 1;
                printf("Reloaded Naive Bayes model from: %s\n", model_path);
            } else {
                printf("Failed to load Naive Bayes model from: %s\n", model_path);
            }
            break;
            
        case AI_MODEL_LINEAR_REGRESSION:
            if (lr_load_model(model_path, &lr_model)) {
                lr_model_loaded = 1;
                printf("Reloaded Linear Regression model from: %s\n", model_path);
            } else {
                printf("Failed to load Linear Regression model from: %s\n", model_path);
            }
            break;
            
        case AI_MODEL_Q_LEARNING:
            if (ql_model_loaded) {
                ql_free_model(&ql_model);
            }
            if (ql_load_model(model_path, &ql_model)) {
                ql_model_loaded = 1;
                printf("Reloaded Q-Learning model from: %s\n", model_path);
            } else {
                printf("Failed to load Q-Learning model from: %s\n", model_path);
            }
            break;
            
        default:
            printf("Warning: Cannot reload Minimax models (they are algorithmic)\n");
            break;
    }
}

// Get the name of the current AI for a given level
const char* game_get_ai_name(int level) {
    AIModelType model = ai_config_get_level(&current_config, level);
    return ai_config_get_model_name(model);
}


// Public: if it's O's turn, ask AI to play based on level (1..3)
void game_ai_move(Game *g, int level)
{

    if (g->turn != 'O')                     // not O's turn?
    {
        return;                             // do nothing
    }

    int mv = -1;
    AIModelType model = ai_config_get_level(&current_config, level);
    
    // Route to the appropriate AI model
    switch (model) {
        case AI_MODEL_NAIVE_BAYES:
            if (nb_model_loaded) {
                mv = nb_find_best_move(&nb_model, g->b);
            }
            break;
            
        case AI_MODEL_LINEAR_REGRESSION:
            if (lr_model_loaded) {
                mv = lr_find_best_move(&lr_model, g->b);
            }
            break;
            
        case AI_MODEL_Q_LEARNING:
            if (ql_model_loaded) {
                mv = ql_find_best_move(&ql_model, g->b);
            }
            break;
            
        case AI_MODEL_MINIMAX_EASY:
            mv = findBestMoveLvl(g->b, 2);  // Depth-limited minimax
            break;
            
        case AI_MODEL_MINIMAX_HARD:
            mv = findBestMoveLvl(g->b, 3);  // Full minimax
            break;
            
        default:
            // Fallback to hard minimax
            mv = findBestMoveLvl(g->b, 3);
            break;
    }

    // If AI returned a valid empty cell, use it
    if (mv >= 0 && mv < 9 && g->b[mv] != 'X' && g->b[mv] != 'O')
    {
        g->b[mv] = 'O';             // place O
        g->turn = 'X';              // switch turn to X
        return;                    
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