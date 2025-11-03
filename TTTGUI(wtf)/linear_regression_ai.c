// linear_regression_ai.c - Linear Regression AI implementation for Tic-Tac-Toe
#include "linear_regression_ai.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Load model from text file
int lr_load_model(const char *filename, LinearRegressionModel *model) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        // Model file not found - silently fail (will use fallback AI)
        return 0;
    }
    
    char line[256];
    int weight_index = 0;
    
    // Skip header lines until we find weights
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, "Weight[") != NULL) {
            // Parse weight line: "Weight[0] (bias): 0.123456"
            double weight_value;
            if (sscanf(line, "Weight[%*d] %*[^:]: %lf", &weight_value) == 1) {
                if (weight_index < LR_NUM_FEATURES) {
                    model->weights[weight_index++] = weight_value;
                }
            }
        }
    }
    
    fclose(fp);
    
    // Verify we loaded all weights
    if (weight_index != LR_NUM_FEATURES) {
        return 0;
    }
    
    return 1;
}

// Convert board state to features for prediction
static void board_to_features(const char board[9], double features[LR_NUM_FEATURES]) {
    features[0] = 1.0;  // Bias term
    
    for (int i = 0; i < 9; i++) {
        if (board[i] == 'X') {
            features[i + 1] = 1.0;
        } else if (board[i] == 'O') {
            features[i + 1] = -1.0;
        } else {
            features[i + 1] = 0.0;  // Empty cell
        }
    }
}

// Predict outcome for a board state
// Returns: >0.5 = win likely, <-0.5 = lose likely, else = draw/uncertain
double lr_predict(const LinearRegressionModel *model, const char board[9]) {
    double features[LR_NUM_FEATURES];
    board_to_features(board, features);
    
    double result = 0.0;
    for (int i = 0; i < LR_NUM_FEATURES; i++) {
        result += model->weights[i] * features[i];
    }
    
    return result;
}

// Find the best move for 'O' using Linear Regression
int lr_find_best_move(const LinearRegressionModel *model, char board[9]) {
    int empty_cells[9];
    int empty_count = 0;
    
    // Find all empty cells
    for (int i = 0; i < 9; i++) {
        if (board[i] != 'X' && board[i] != 'O') {
            empty_cells[empty_count++] = i;
        }
    }
    
    if (empty_count == 0) {
        return -1; // No valid moves
    }
    
    // Try each empty cell and evaluate the resulting board state
    int best_move = empty_cells[0];
    double best_score = -999999.0;
    
    for (int i = 0; i < empty_count; i++) {
        int move = empty_cells[i];
        
        // Create a temporary board with this move
        char temp_board[9];
        memcpy(temp_board, board, 9);
        temp_board[move] = 'O';
        
        // Predict outcome (from X's perspective where positive = X wins)
        // Since model predicts X's outcome and we're playing as O:
        // Lower (more negative) prediction = better for O
        double prediction = lr_predict(model, temp_board);
        
        // We want to MINIMIZE the prediction (since negative = O wins)
        // So we negate it to maximize
        double score = -prediction;
        
        if (score > best_score) {
            best_score = score;
            best_move = move;
        }
    }
    
    // Add some randomness (10% chance to pick a random move)
    // This makes the AI less predictable
    if ((rand() % 100) < 10 && empty_count > 1) {
        return empty_cells[rand() % empty_count];
    }
    
    return best_move;
}
