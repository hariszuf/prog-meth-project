// example_frozen_usage.c - Example of how to use frozen models in your game
#include "frozen_q_model.h"
#include <stdio.h>
#include <stdlib.h>

void print_board(const char board[9]) {
    printf("\n");
    for (int i = 0; i < 9; i++) {
        char display = (board[i] == 'b') ? ' ' : board[i];
        printf(" %c ", display);
        if (i % 3 == 2) printf("\n");
        else printf("|");
    }
    printf("\n");
}

int main() {
    // Load the frozen model (read-only)
    printf("Loading frozen Q-Learning model...\n");
    FrozenQModel *model = frozen_q_load("../../models/q learning/q_learning_non_terminal.txt");
    
    if (!model) {
        printf("Failed to load model!\n");
        return 1;
    }
    
    printf("Model loaded successfully!\n");
    printf("Total entries: %d\n", frozen_q_get_entry_count(model));
    
    // Example game state
    char board[9] = {'x', 'b', 'b', 
                     'b', 'o', 'b', 
                     'b', 'b', 'b'};
    
    printf("\nCurrent board:");
    print_board(board);
    
    // Get the best move for O
    int best_move = frozen_q_get_best_action(model, board);
    
    if (best_move != -1) {
        printf("\nBest move for O: Position %d\n", best_move);
        
        // Show Q-values for all valid moves
        printf("\nQ-values for all valid moves:\n");
        for (int i = 0; i < 9; i++) {
            if (board[i] == 'b') {
                double q = frozen_q_get_q_value(model, board, i);
                printf("  Position %d: %.6f %s\n", i, q, (i == best_move) ? "<-- BEST" : "");
            }
        }
    } else {
        printf("\nNo valid moves available!\n");
    }
    
    // Cleanup
    frozen_q_free(model);
    printf("\nModel freed. Exiting.\n");
    
    return 0;
}
