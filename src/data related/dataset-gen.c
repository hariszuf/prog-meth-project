#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BOARD_SIZE 9
#define MAX_STATES 20000

// Outcome types
typedef enum {
    WIN = 1,
    DRAW = 0,
    LOSE = -1,
    CONTINUE = -99
} Outcome;

// Structure for a board state
typedef struct {
    char board[BOARD_SIZE];  // 'x', 'o', or 'b'
    char outcome[5];         // "win", "lose", "draw"
} BoardState;

// Structure for dataset
typedef struct {
    BoardState *states;
    int count;
    int capacity;
    int wins;
    int losses;
    int draws;
    int terminal;
    int non_terminal;
} Dataset;

// Hash table for memoization (simple implementation)
#define MEMO_SIZE 100003
typedef struct MemoNode {
    char board[BOARD_SIZE];
    char is_maximizing;
    int score;
    struct MemoNode *next;
} MemoNode;

typedef struct {
    MemoNode *table[MEMO_SIZE];
    int hits;
    int misses;
} MemoTable;

// Function prototypes
void init_dataset(Dataset *dataset);
void free_dataset(Dataset *dataset);
void add_to_dataset(Dataset *dataset, char board[BOARD_SIZE], const char *outcome);
int check_winner(char board[BOARD_SIZE]);
int is_valid_state(char board[BOARD_SIZE]);
int minimax(char board[BOARD_SIZE], int is_maximizing, int alpha, int beta, MemoTable *memo);
void generate_all_states(Dataset *dataset, int include_terminal, int include_non_terminal, MemoTable *memo);
void save_dataset(const char *filename, Dataset *dataset);
void print_statistics(Dataset *dataset);
void display_board(char board[BOARD_SIZE]);
unsigned long hash_board(char board[BOARD_SIZE], int is_maximizing);
int memo_lookup(MemoTable *memo, char board[BOARD_SIZE], int is_maximizing, int *score);
void memo_insert(MemoTable *memo, char board[BOARD_SIZE], int is_maximizing, int score);
void init_memo_table(MemoTable *memo);
void free_memo_table(MemoTable *memo);

// Initialize dataset
void init_dataset(Dataset *dataset) {
    dataset->capacity = MAX_STATES;
    dataset->states = (BoardState *)malloc(dataset->capacity * sizeof(BoardState));
    dataset->count = 0;
    dataset->wins = 0;
    dataset->losses = 0;
    dataset->draws = 0;
    dataset->terminal = 0;
    dataset->non_terminal = 0;
}

// Free dataset memory
void free_dataset(Dataset *dataset) {
    if (dataset->states != NULL) {
        free(dataset->states);
        dataset->states = NULL;
    }
}

// Add board state to dataset
void add_to_dataset(Dataset *dataset, char board[BOARD_SIZE], const char *outcome) {
    if (dataset->count >= dataset->capacity) {
        dataset->capacity *= 2;
        dataset->states = (BoardState *)realloc(dataset->states, 
                                                 dataset->capacity * sizeof(BoardState));
    }
    
    memcpy(dataset->states[dataset->count].board, board, BOARD_SIZE);
    strcpy(dataset->states[dataset->count].outcome, outcome);
    dataset->count++;
    
    // Update statistics
    if (strcmp(outcome, "win") == 0) {
        dataset->wins++;
    } else if (strcmp(outcome, "lose") == 0) {
        dataset->losses++;
    } else if (strcmp(outcome, "draw") == 0) {
        dataset->draws++;
    }
}

// Check winner: returns WIN(1), LOSE(-1), DRAW(0), or CONTINUE(-99)
int check_winner(char board[BOARD_SIZE]) {
    // Win patterns: rows, columns, diagonals
    int patterns[8][3] = {
        {0, 1, 2}, {3, 4, 5}, {6, 7, 8},  // Rows
        {0, 3, 6}, {1, 4, 7}, {2, 5, 8},  // Columns
        {0, 4, 8}, {2, 4, 6}              // Diagonals
    };
    
    // Check for winner
    for (int i = 0; i < 8; i++) {
        if (board[patterns[i][0]] == board[patterns[i][1]] && 
            board[patterns[i][1]] == board[patterns[i][2]] && 
            board[patterns[i][0]] != 'b') {
            
            if (board[patterns[i][0]] == 'x') {
                return WIN;
            } else {
                return LOSE;
            }
        }
    }
    
    // Check for draw (board full)
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (board[i] == 'b') {
            return CONTINUE;
        }
    }
    
    return DRAW;
}

// Check if board state is valid (reachable through legal play)
int is_valid_state(char board[BOARD_SIZE]) {
    int x_count = 0, o_count = 0;
    
    // Count X's and O's
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (board[i] == 'x') x_count++;
        else if (board[i] == 'o') o_count++;
    }
    
    // X goes first: x_count == o_count or x_count == o_count + 1
    if (!(x_count == o_count || x_count == o_count + 1)) {
        return 0;
    }
    
    // Check if both players won (impossible)
    int x_wins = 0, o_wins = 0;
    int patterns[8][3] = {
        {0, 1, 2}, {3, 4, 5}, {6, 7, 8},
        {0, 3, 6}, {1, 4, 7}, {2, 5, 8},
        {0, 4, 8}, {2, 4, 6}
    };
    
    for (int i = 0; i < 8; i++) {
        if (board[patterns[i][0]] == board[patterns[i][1]] && 
            board[patterns[i][1]] == board[patterns[i][2]]) {
            
            if (board[patterns[i][0]] == 'x') x_wins = 1;
            else if (board[patterns[i][0]] == 'o') o_wins = 1;
        }
    }
    
    // Both can't win
    if (x_wins && o_wins) {
        return 0;
    }
    
    // If X wins, X made last move (x_count == o_count + 1)
    if (x_wins && x_count != o_count + 1) {
        return 0;
    }
    
    // If O wins, O made last move (x_count == o_count)
    if (o_wins && x_count != o_count) {
        return 0;
    }
    
    return 1;
}

// Initialize memoization table
void init_memo_table(MemoTable *memo) {
    for (int i = 0; i < MEMO_SIZE; i++) {
        memo->table[i] = NULL;
    }
    memo->hits = 0;
    memo->misses = 0;
}

// Free memoization table
void free_memo_table(MemoTable *memo) {
    for (int i = 0; i < MEMO_SIZE; i++) {
        MemoNode *node = memo->table[i];
        while (node != NULL) {
            MemoNode *temp = node;
            node = node->next;
            free(temp);
        }
    }
}

// Hash function for board
unsigned long hash_board(char board[BOARD_SIZE], int is_maximizing) {
    unsigned long hash = is_maximizing ? 5381 : 7919;
    
    for (int i = 0; i < BOARD_SIZE; i++) {
        hash = ((hash << 5) + hash) + board[i];
    }
    
    return hash % MEMO_SIZE;
}

// Lookup in memoization table
int memo_lookup(MemoTable *memo, char board[BOARD_SIZE], int is_maximizing, int *score) {
    unsigned long hash = hash_board(board, is_maximizing);
    MemoNode *node = memo->table[hash];
    
    while (node != NULL) {
        if (node->is_maximizing == is_maximizing && 
            memcmp(node->board, board, BOARD_SIZE) == 0) {
            *score = node->score;
            memo->hits++;
            return 1;
        }
        node = node->next;
    }
    
    memo->misses++;
    return 0;
}

// Insert into memoization table
void memo_insert(MemoTable *memo, char board[BOARD_SIZE], int is_maximizing, int score) {
    unsigned long hash = hash_board(board, is_maximizing);
    
    MemoNode *new_node = (MemoNode *)malloc(sizeof(MemoNode));
    memcpy(new_node->board, board, BOARD_SIZE);
    new_node->is_maximizing = is_maximizing;
    new_node->score = score;
    new_node->next = memo->table[hash];
    memo->table[hash] = new_node;
}

// Minimax algorithm with alpha-beta pruning and memoization
int minimax(char board[BOARD_SIZE], int is_maximizing, int alpha, int beta, MemoTable *memo) {
    // Check memoization
    int cached_score;
    if (memo_lookup(memo, board, is_maximizing, &cached_score)) {
        return cached_score;
    }
    
    // Check terminal state
    int winner = check_winner(board);
    if (winner != CONTINUE) {
        return winner;
    }
    
    int best_score;
    
    if (is_maximizing) {
        best_score = -1000;
        
        for (int i = 0; i < BOARD_SIZE; i++) {
            if (board[i] == 'b') {
                board[i] = 'x';
                int score = minimax(board, 0, alpha, beta, memo);
                board[i] = 'b';
                
                if (score > best_score) {
                    best_score = score;
                }
                if (score > alpha) {
                    alpha = score;
                }
                if (beta <= alpha) {
                    break;  // Beta cutoff
                }
            }
        }
    } else {
        best_score = 1000;
        
        for (int i = 0; i < BOARD_SIZE; i++) {
            if (board[i] == 'b') {
                board[i] = 'o';
                int score = minimax(board, 1, alpha, beta, memo);
                board[i] = 'b';
                
                if (score < best_score) {
                    best_score = score;
                }
                if (score < beta) {
                    beta = score;
                }
                if (beta <= alpha) {
                    break;  // Alpha cutoff
                }
            }
        }
    }
    
    // Store in memoization table
    memo_insert(memo, board, is_maximizing, best_score);
    
    return best_score;
}

// Generate all possible board states recursively
void generate_state_recursive(char board[BOARD_SIZE], int pos, Dataset *dataset, 
                              int include_terminal, int include_non_terminal, 
                              MemoTable *memo) {
    if (pos == BOARD_SIZE) {
        // Check if valid state
        if (!is_valid_state(board)) {
            return;
        }
        
        // Check if terminal or non-terminal
        int winner = check_winner(board);
        int is_terminal = (winner != CONTINUE);
        
        // Filter based on preferences
        if (is_terminal && !include_terminal) {
            return;
        }
        if (!is_terminal && !include_non_terminal) {
            return;
        }
        
        // Update statistics
        if (is_terminal) {
            dataset->terminal++;
        } else {
            dataset->non_terminal++;
        }
        
        // Determine outcome
        const char *outcome;
        if (is_terminal) {
            // Use actual outcome for terminal states
            if (winner == WIN) {
                outcome = "win";
            } else if (winner == LOSE) {
                outcome = "lose";
            } else {
                outcome = "draw";
            }
        } else {
            // Use minimax evaluation for non-terminal states
            int x_count = 0, o_count = 0;
            for (int i = 0; i < BOARD_SIZE; i++) {
                if (board[i] == 'x') x_count++;
                else if (board[i] == 'o') o_count++;
            }
            
            int is_x_turn = (x_count == o_count);
            int minimax_score = minimax(board, is_x_turn, -1000, 1000, memo);
            
            if (minimax_score > 0) {
                outcome = "win";
            } else if (minimax_score == 0) {
                outcome = "draw";
            } else {
                outcome = "lose";
            }
        }
        
        // Add to dataset
        add_to_dataset(dataset, board, outcome);
        return;
    }
    
    // Try all possible values for current position
    char values[] = {'x', 'o', 'b'};
    for (int i = 0; i < 3; i++) {
        board[pos] = values[i];
        generate_state_recursive(board, pos + 1, dataset, include_terminal, 
                                include_non_terminal, memo);
    }
}

// Generate all states
void generate_all_states(Dataset *dataset, int include_terminal, 
                        int include_non_terminal, MemoTable *memo) {
    char board[BOARD_SIZE];
    printf("Generating all valid board states...\n");
    printf("This may take 30-60 seconds...\n\n");
    
    generate_state_recursive(board, 0, dataset, include_terminal, 
                            include_non_terminal, memo);
    
    printf("\n✓ Generation complete!\n");
    printf("✓ Generated %d valid game states\n", dataset->count);
}

// Display board
void display_board(char board[BOARD_SIZE]) {
    printf("\n");
    for (int i = 0; i < BOARD_SIZE; i++) {
        printf(" %c ", board[i]);
        if ((i + 1) % 3 == 0) {
            printf("\n");
            if (i < 8) {
                printf("-----------\n");
            }
        } else {
            printf("|");
        }
    }
    printf("\n");
}

// Print dataset statistics
void print_statistics(Dataset *dataset) {
    printf("\n========================================\n");
    printf("DATASET STATISTICS\n");
    printf("========================================\n");
    printf("\nTotal states: %d\n", dataset->count);
    
    printf("\nState types:\n");
    printf("  Terminal states:     %5d (%5.2f%%)\n", 
           dataset->terminal, 
           (dataset->terminal * 100.0) / dataset->count);
    printf("  Non-terminal states: %5d (%5.2f%%)\n", 
           dataset->non_terminal, 
           (dataset->non_terminal * 100.0) / dataset->count);
    
    printf("\nOptimal outcomes (with perfect play):\n");
    printf("  X wins (win):   %5d (%5.2f%%)\n", 
           dataset->wins, 
           (dataset->wins * 100.0) / dataset->count);
    printf("  O wins (lose):  %5d (%5.2f%%)\n", 
           dataset->losses, 
           (dataset->losses * 100.0) / dataset->count);
    printf("  Draws (draw):   %5d (%5.2f%%)\n", 
           dataset->draws, 
           (dataset->draws * 100.0) / dataset->count);
    printf("========================================\n");
    
    printf("\n💡 KEY INSIGHT:\n");
    printf("   These labels represent outcomes with OPTIMAL play\n");
    printf("   Not actual game results, but what SHOULD happen\n");
    printf("========================================\n");
}

// Save dataset to file
void save_dataset(const char *filename, Dataset *dataset) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("Error: Could not create file %s\n", filename);
        return;
    }
    
    for (int i = 0; i < dataset->count; i++) {
        // Write board
        for (int j = 0; j < BOARD_SIZE; j++) {
            fprintf(fp, "%c", dataset->states[i].board[j]);
            if (j < BOARD_SIZE - 1) {
                fprintf(fp, ",");
            }
        }
        // Write outcome
        fprintf(fp, ",%s\n", dataset->states[i].outcome);
    }
    
    fclose(fp);
    printf("\n✓ Dataset saved to: %s\n", filename);
}

// Show example states
void show_examples(Dataset *dataset) {
    printf("\n========================================\n");
    printf("SAMPLE BOARD STATES WITH MINIMAX EVALUATION\n");
    printf("========================================\n");
    
    // Find one example of each type (non-terminal)
    int shown_win = 0, shown_draw = 0, shown_lose = 0;
    
    for (int i = 0; i < dataset->count && !(shown_win && shown_draw && shown_lose); i++) {
        BoardState *state = &dataset->states[i];
        int winner = check_winner(state->board);
        
        // Only show non-terminal examples
        if (winner != CONTINUE) {
            continue;
        }
        
        if (!shown_win && strcmp(state->outcome, "win") == 0) {
            printf("\nX CAN FORCE WIN:\n");
            printf("----------------------------------------\n");
            display_board(state->board);
            printf("Minimax evaluation: %s\n", state->outcome);
            shown_win = 1;
        } else if (!shown_draw && strcmp(state->outcome, "draw") == 0) {
            printf("\nPERFECT PLAY → DRAW:\n");
            printf("----------------------------------------\n");
            display_board(state->board);
            printf("Minimax evaluation: %s\n", state->outcome);
            shown_draw = 1;
        } else if (!shown_lose && strcmp(state->outcome, "lose") == 0) {
            printf("\nX WILL LOSE:\n");
            printf("----------------------------------------\n");
            display_board(state->board);
            printf("Minimax evaluation: %s\n", state->outcome);
            shown_lose = 1;
        }
    }
    
    printf("========================================\n");
}

int main() {
    printf("========================================\n");
    printf("OPTIMAL TIC-TAC-TOE DATASET GENERATOR\n");
    printf("Using Minimax Algorithm (C Implementation)\n");
    printf("========================================\n");
    printf("\nThis generates a dataset where each board state is labeled\n");
    printf("with its OPTIMAL outcome assuming both players play perfectly.\n");
    printf("\nThis is superior to random gameplay datasets because:\n");
    printf("  ✓ ML models learn optimal strategy\n");
    printf("  ✓ No noise from suboptimal play\n");
    printf("  ✓ Includes non-terminal strategic positions\n");
    printf("========================================\n");
    
    // Choose dataset type
    printf("\n🎮 Choose dataset type:\n");
    printf("  1. Terminal states only (games already finished)\n");
    printf("  2. Non-terminal states only (games in progress)\n");
    printf("  3. Both terminal and non-terminal (RECOMMENDED)\n");
    printf("\nEnter choice (1/2/3) [default: 3]: ");
    
    char choice[10];
    if (fgets(choice, sizeof(choice), stdin) == NULL || choice[0] == '\n') {
        choice[0] = '3';
    }
    
    int include_terminal, include_non_terminal;
    char filename[100];
    
    if (choice[0] == '1') {
        include_terminal = 1;
        include_non_terminal = 0;
        strcpy(filename, "tic-tac-toe-minimax-terminal.data");
    } else if (choice[0] == '2') {
        include_terminal = 0;
        include_non_terminal = 1;
        strcpy(filename, "tic-tac-toe-minimax-non-terminal.data");
    } else {
        include_terminal = 1;
        include_non_terminal = 1;
        strcpy(filename, "tic-tac-toe-minimax-complete.data");
    }
    
    // Initialize
    Dataset dataset;
    MemoTable memo;
    init_dataset(&dataset);
    init_memo_table(&memo);
    
    // Generate dataset
    clock_t start = clock();
    generate_all_states(&dataset, include_terminal, include_non_terminal, &memo);
    clock_t end = clock();
    
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    printf("\n⏱️  Generation time: %.2f seconds\n", time_taken);
    printf("📊 Memoization hits: %d, misses: %d (hit rate: %.2f%%)\n",
           memo.hits, memo.misses, 
           (memo.hits * 100.0) / (memo.hits + memo.misses));
    
    // Print statistics
    print_statistics(&dataset);
    
    // Show examples
    show_examples(&dataset);
    
    // Save dataset
    printf("\n💾 Saving dataset...\n");
    save_dataset(filename, &dataset);
    
    printf("\n========================================\n");
    printf("✓ GENERATION COMPLETE!\n");
    printf("========================================\n");
    printf("\nGenerated file: %s\n", filename);
    printf("Total samples: %d\n", dataset.count);
    
    printf("\n🎯 USAGE RECOMMENDATIONS:\n");
    printf("\nFor Q-Learning:\n");
    printf("  - Use this dataset for reward shaping\n");
    printf("  - Rewards: win=+1, draw=0, lose=-1\n");
    printf("  - Train against minimax-labeled optimal outcomes\n");
    
    printf("\nFor Naive Bayes:\n");
    printf("  - Train on optimal outcomes, not random play\n");
    printf("  - Model learns 'what should happen'\n");
    printf("  - Better probability estimates\n");
    
    printf("\n========================================\n");
    
    // Cleanup
    free_dataset(&dataset);
    free_memo_table(&memo);
    
    return 0;
}