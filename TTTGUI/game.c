#include "game.h"
#include "minimax.h"
#include "naive_bayes_ai.h"
#include "linear_regression_ai.h"
#include "q_learning_ai.h"
#include "model_config.h"
#include <stdio.h>

// AI model structures
static NaiveBayesModel nb_model;
static LinearRegressionModel lr_model;
static QLearningModel ql_model;

// Flags for loaded status
static int nb_model_loaded = 0;
static int lr_model_loaded = 0;
static int ql_model_loaded = 0;

// Current AI settings
static AIConfig current_config;

void game_init(Game *g)
{
    // Clear board
    for (int i = 0; i < 9; i++)
        g->b[i] = ' ';

    // X starts
    g->turn = 'X';

    // No winner
    g->winner = 0;
}

void game_reset(Game *g)
{
    // Reset game
    game_init(g);
}

int game_is_full(const Game *g)
{
    // Check for empty cells
    for (int i = 0; i < 9; i++)
        if (g->b[i] != 'X' && g->b[i] != 'O')
            return 0;

    // Board full
    return 1;
}

int game_make_move(Game *g, int index)
{
    // Check bounds
    if (index < 0 || index > 8)
        return 0;

    // Check if occupied
    if (g->b[index] == 'X' || g->b[index] == 'O')
        return 0;

    // Place symbol
    g->b[index] = g->turn;

    // Switch turn
    if (g->turn == 'X')
        g->turn = 'O';
    else
        g->turn = 'X';

    return 1; 
}

void game_check_end(Game *g)
{
    // Check X win
    if (winBy(g->b, 'X'))
        g->winner = 1;

    // Check O win
    else if (winBy(g->b, 'O'))
        g->winner = 2;

    // Check draw
    else if (game_is_full(g))
        g->winner = 3;

    // Game continues
    else
        g->winner = 0;
}

void game_load_all_models(void)
{
    // Load best-config preset
    ai_config_preset_best_models(&current_config);

    // Load Naive Bayes
    if (!nb_model_loaded)
    {
        if (nb_load_model("../models/naive_bayes_non_terminal/model_non_terminal.txt", &nb_model))
        {
            nb_model_loaded = 1;
            printf("Naive Bayes model loaded\n");
        }
        else
            printf("Failed to load Naive Bayes model\n");
    }

    // Load Linear Regression
    if (!lr_model_loaded)
    {
        if (lr_load_model("../models/linear_regression_non_terminal/model_non_terminal.txt", &lr_model))
        {
            lr_model_loaded = 1;
            printf("Linear Regression model loaded\n");
        }
        else
            printf("Failed to load Linear Regression model\n");
    }

    // Load Q-Learning
    if (!ql_model_loaded)
    {
        if (ql_load_model("../models/q learning/q_learning_dataset.txt", &ql_model))
        {
            ql_model_loaded = 1;
            printf("Q-Learning model loaded\n");
        }
        else
            printf("Failed to load Q-Learning model\n");
    }

    // Print config
    printf("\nCurrent AI Configuration:\n");
    printf("  Easy:   %s\n", ai_config_get_model_name(current_config.easy_model));
    printf("  Medium: %s\n", ai_config_get_model_name(current_config.medium_model));
    printf("  Hard:   %s\n", ai_config_get_model_name(current_config.hard_model));
    printf("\n");
}

void game_set_ai_config(const AIConfig *config)
{
    // Update AI settings
    current_config = *config;

    // Print updated config
    printf("AI Configuration updated:\n");
    printf("  Easy:   %s\n", ai_config_get_model_name(current_config.easy_model));
    printf("  Medium: %s\n", ai_config_get_model_name(current_config.medium_model));
    printf("  Hard:   %s\n", ai_config_get_model_name(current_config.hard_model));
}

void game_get_ai_config(AIConfig *config)
{
    // Copy config out
    *config = current_config;
}

void game_load_model_file(AIModelType model_type, const char *path)
{
    // Reload specific model
    switch (model_type)
    {
        case AI_MODEL_NAIVE_BAYES:
            if (nb_load_model(path, &nb_model))
            {
                nb_model_loaded = 1;
                printf("Reloaded Naive Bayes: %s\n", path);
            }
            else
                printf("Failed: %s\n", path);
            break;

        case AI_MODEL_LINEAR_REGRESSION:
            if (lr_load_model(path, &lr_model))
            {
                lr_model_loaded = 1;
                printf("Reloaded Linear Regression: %s\n", path);
            }
            else
                printf("Failed: %s\n", path);
            break;

        case AI_MODEL_Q_LEARNING:
            if (ql_model_loaded)
                ql_free_model(&ql_model);

            if (ql_load_model(path, &ql_model))
            {
                ql_model_loaded = 1;
                printf("Reloaded Q-Learning: %s\n", path);
            }
            else
                printf("Failed: %s\n", path);
            break;

        default:
            printf("Cannot reload Minimax (algorithmic)\n");
            break;
    }
}

const char* game_get_ai_name(int level)
{
    // Get model name
    AIModelType model = ai_config_get_level(&current_config, level);
    return ai_config_get_model_name(model);
}

void game_ai_move(Game *g, int level)
{
    // AI moves only when O's turn
    if (g->turn != 'O')
        return;

    int mv = -1;

    // Select model for difficulty
    AIModelType model = ai_config_get_level(&current_config, level);

    // Run appropriate AI
    switch (model)
    {
        case AI_MODEL_NAIVE_BAYES:
            if (nb_model_loaded)
                mv = nb_find_best_move(&nb_model, g->b);
            break;

        case AI_MODEL_LINEAR_REGRESSION:
            if (lr_model_loaded)
                mv = lr_find_best_move(&lr_model, g->b);
            break;

        case AI_MODEL_Q_LEARNING:
            if (ql_model_loaded)
                mv = ql_find_best_move(&ql_model, g->b);
            break;

        case AI_MODEL_MINIMAX_EASY:
            mv = findBestMoveLvl(g->b, 2);
            break;

        case AI_MODEL_MINIMAX_HARD:
            mv = findBestMoveLvl(g->b, 3);
            break;

        default:
            mv = findBestMoveLvl(g->b, 3);
            break;
    }

    // If AI move valid
    if (mv >= 0 && mv < 9 && g->b[mv] != 'X' && g->b[mv] != 'O')
    {
        g->b[mv] = 'O';
        g->turn = 'X';
        return;
    }

    // Fallback: first empty cell
    for (int i = 0; i < 9; i++)
    {
        if (g->b[i] != 'X' && g->b[i] != 'O')
        {
            g->b[i] = 'O';
            g->turn = 'X';
            break;
        }
    }
}
