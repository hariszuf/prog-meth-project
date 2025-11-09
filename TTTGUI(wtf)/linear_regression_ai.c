// linear_regression_ai.c - Linear Regression AI implementation for Tic-Tac-Toe
#include "linear_regression_ai.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Load model from text file
int lr_load_model(const char *filename, LinearRegressionModel *model) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open Linear Regression model file %s\n", filename);
        return 0;
    }
    
    char line[256];
    int weight_idx = 0;
    
    // Read weights from file (format: "Weight[0] = value")
    while (fgets(line, sizeof(line), file) && weight_idx < NUM_FEATURES) {
        if (strstr(line, "Weight") != NULL) {
            double weight;
            if (sscanf(line, "Weight[%*d] = %lf", &weight) == 1) {
                model->weights[weight_idx++] = weight;
            }
        }
    }
    
    fclose(file);
    
    if (weight_idx != NUM_FEATURES) {
        printf("Warning: Expected %d weights, got %d\n", NUM_FEATURES, weight_idx);
        return 0;
    }
    
    printf("Linear Regression model loaded successfully: %d weights\n", weight_idx);
    return 1;
}

// Encode board features for prediction
static void encode_features(const char *board, double *features) {
    features[0] = 1.0;  // Bias term
    
    for (int i = 0; i < 9; i++) {
        if (board[i] == 'X') {
            features[i + 1] = 1.0;
        } else if (board[i] == 'O') {
            features[i + 1] = -1.0;
        } else {  // Empty cell
            features[i + 1] = 0.0;
        }
    }
}

// Compute prediction (dot product of weights and features)
static double lr_predict(const LinearRegressionModel *model, const double *features) {
    double result = 0.0;
    for (int i = 0; i < NUM_FEATURES; i++) {
        result += model->weights[i] * features[i];
    }
    return result;
}

// Find best move for 'O' using Linear Regression
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
    double best_score = -1000.0;
    
    for (int i = 0; i < empty_count; i++) {
        int move = empty_cells[i];
        
        // Create a temporary board with this move
        char temp_board[9];
        memcpy(temp_board, board, 9);
        temp_board[move] = 'O';
        
        // Encode features and predict
        double features[NUM_FEATURES];
        encode_features(temp_board, features);
        double score = lr_predict(model, features);
        
        // Higher score = better for O
        if (score > best_score) {
            best_score = score;
            best_move = move;
        }
    }
    
    return best_move;
}
