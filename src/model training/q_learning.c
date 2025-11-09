#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define BOARD_SIZE 9
#define Q_TABLE_SIZE 20000
#define MAX_EPISODES 50000
#define EMPTY 'b'
#define PLAYER_X 'x'
#define PLAYER_O 'o'

// Q-Learning hyperparameters
#define ALPHA 0.1       // Learning rate
#define GAMMA 0.9       // Discount factor
#define EPSILON 0.1     // Exploration rate

// Rewards
#define REWARD_WIN 1.0
#define REWARD_DRAW 0.0
#define REWARD_LOSE -1.0
#define REWARD_INVALID -2.0

// Structure for Q-table entry
typedef struct QEntry {
    char board[BOARD_SIZE];
    int action;              // 0-8 position
    double q_value;
    int visits;
    struct QEntry *next;     // For hash collision chaining
} QEntry;

// Hash table for Q-values
typedef struct {
    QEntry *table[Q_TABLE_SIZE];
    int total_entries;
} QTable;

// Game state
typedef struct {
    char board[BOARD_SIZE];
    int game_over;
    char winner;
} GameState;

// Function prototypes
void init_qtable(QTable *qt);
void free_qtable(QTable *qt);
unsigned long hash_board(char board[BOARD_SIZE]);
double get_q_value(QTable *qt, char board[BOARD_SIZE], int action);
void update_q_value(QTable *qt, char board[BOARD_SIZE], int action, double value);
void init_board(GameState *game);
char check_winner(char board[BOARD_SIZE]);
int is_valid_move(char board[BOARD_SIZE], int pos);
int get_valid_moves(char board[BOARD_SIZE], int moves[BOARD_SIZE]);
int choose_action(QTable *qt, char board[BOARD_SIZE], double epsilon);
int choose_best_action(QTable *qt, char board[BOARD_SIZE]);
double get_max_q_value(QTable *qt, char board[BOARD_SIZE]);
void train_q_learning(QTable *qt, int episodes);
void save_qtable(const char *filename, QTable *qt);
void load_minimax_dataset(const char *filename, QTable *qt);
void test_q_learning(QTable *qt, int test_games);
void print_board(char board[BOARD_SIZE]);

// Initialize Q-table
void init_qtable(QTable *qt) {
    for (int i = 0; i < Q_TABLE_SIZE; i++) {
        qt->table[i] = NULL;
    }
    qt->total_entries = 0;
}

// Free Q-table memory
void free_qtable(QTable *qt) {
    for (int i = 0; i < Q_TABLE_SIZE; i++) {
        QEntry *entry = qt->table[i];
        while (entry != NULL) {
            QEntry *temp = entry;
            entry = entry->next;
            free(temp);
        }
    }
}

// Hash function for board state
unsigned long hash_board(char board[BOARD_SIZE]) {
    unsigned long hash = 5381;
    for (int i = 0; i < BOARD_SIZE; i++) {
        hash = ((hash << 5) + hash) + board[i];
    }
    return hash % Q_TABLE_SIZE;
}

// Get Q-value for state-action pair
double get_q_value(QTable *qt, char board[BOARD_SIZE], int action) {
    unsigned long hash = hash_board(board);
    QEntry *entry = qt->table[hash];
    
    while (entry != NULL) {
        if (entry->action == action && 
            memcmp(entry->board, board, BOARD_SIZE) == 0) {
            return entry->q_value;
        }
        entry = entry->next;
    }
    
    return 0.0; // Default Q-value for unseen state-action
}

// Update Q-value for state-action pair
void update_q_value(QTable *qt, char board[BOARD_SIZE], int action, double value) {
    unsigned long hash = hash_board(board);
    QEntry *entry = qt->table[hash];
    
    // Search for existing entry
    while (entry != NULL) {
        if (entry->action == action && 
            memcmp(entry->board, board, BOARD_SIZE) == 0) {
            entry->q_value = value;
            entry->visits++;
            return;
        }
        entry = entry->next;
    }
    
    // Create new entry
    QEntry *new_entry = (QEntry *)malloc(sizeof(QEntry));
    memcpy(new_entry->board, board, BOARD_SIZE);
    new_entry->action = action;
    new_entry->q_value = value;
    new_entry->visits = 1;
    new_entry->next = qt->table[hash];
    qt->table[hash] = new_entry;
    qt->total_entries++;
}

// Initialize board
void init_board(GameState *game) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        game->board[i] = EMPTY;
    }
    game->game_over = 0;
    game->winner = ' ';
}

// Check winner
char check_winner(char board[BOARD_SIZE]) {
    int wins[8][3] = {
        {0, 1, 2}, {3, 4, 5}, {6, 7, 8},  // Rows
        {0, 3, 6}, {1, 4, 7}, {2, 5, 8},  // Columns
        {0, 4, 8}, {2, 4, 6}              // Diagonals
    };
    
    for (int i = 0; i < 8; i++) {
        if (board[wins[i][0]] == board[wins[i][1]] && 
            board[wins[i][1]] == board[wins[i][2]] && 
            board[wins[i][0]] != EMPTY) {
            return board[wins[i][0]];
        }
    }
    
    // Check for draw
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (board[i] == EMPTY) {
            return ' '; // Game continues
        }
    }
    
    return 'd'; // Draw
}

// Check if move is valid
int is_valid_move(char board[BOARD_SIZE], int pos) {
    return (pos >= 0 && pos < BOARD_SIZE && board[pos] == EMPTY);
}

// Get all valid moves
int get_valid_moves(char board[BOARD_SIZE], int moves[BOARD_SIZE]) {
    int count = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (board[i] == EMPTY) {
            moves[count++] = i;
        }
    }
    return count;
}

// Choose action using epsilon-greedy policy
int choose_action(QTable *qt, char board[BOARD_SIZE], double epsilon) {
    int valid_moves[BOARD_SIZE];
    int num_moves = get_valid_moves(board, valid_moves);
    
    if (num_moves == 0) return -1;
    
    // Exploration: random move
    if ((double)rand() / RAND_MAX < epsilon) {
        return valid_moves[rand() % num_moves];
    }
    
    // Exploitation: choose best move
    int best_action = valid_moves[0];
    double best_q = get_q_value(qt, board, best_action);
    
    for (int i = 1; i < num_moves; i++) {
        double q = get_q_value(qt, board, valid_moves[i]);
        if (q > best_q) {
            best_q = q;
            best_action = valid_moves[i];
        }
    }
    
    return best_action;
}

// Choose best action (for testing/playing)
int choose_best_action(QTable *qt, char board[BOARD_SIZE]) {
    return choose_action(qt, board, 0.0); // No exploration
}

// Get maximum Q-value for a state
double get_max_q_value(QTable *qt, char board[BOARD_SIZE]) {
    int valid_moves[BOARD_SIZE];
    int num_moves = get_valid_moves(board, valid_moves);
    
    if (num_moves == 0) return 0.0;
    
    double max_q = get_q_value(qt, board, valid_moves[0]);
    for (int i = 1; i < num_moves; i++) {
        double q = get_q_value(qt, board, valid_moves[i]);
        if (q > max_q) max_q = q;
    }
    
    return max_q;
}

// Train Q-learning agent through self-play
void train_q_learning(QTable *qt, int episodes) {
    printf("\n========================================\n");
    printf("TRAINING Q-LEARNING AGENT\n");
    printf("========================================\n");
    printf("Episodes: %d\n", episodes);
    printf("Alpha (learning rate): %.2f\n", ALPHA);
    printf("Gamma (discount): %.2f\n", GAMMA);
    printf("Epsilon (exploration): %.2f\n", EPSILON);
    printf("========================================\n\n");
    
    int wins = 0, losses = 0, draws = 0;
    
    for (int episode = 0; episode < episodes; episode++) {
        GameState game;
        init_board(&game);
        
        char player = PLAYER_X;
        
        // Store state-action pairs for updating
        typedef struct {
            char board[BOARD_SIZE];
            int action;
        } Transition;
        
        Transition transitions[BOARD_SIZE * 2];
        int num_transitions = 0;
        
        // Play one episode
        while (!game.game_over) {
            int action = choose_action(qt, game.board, EPSILON);
            
            if (action == -1) break;
            
            // Store transition for X player only (we're training X)
            if (player == PLAYER_X) {
                memcpy(transitions[num_transitions].board, game.board, BOARD_SIZE);
                transitions[num_transitions].action = action;
                num_transitions++;
            }
            
            // Make move
            game.board[action] = player;
            
            // Check game state
            char winner = check_winner(game.board);
            if (winner != ' ') {
                game.game_over = 1;
                game.winner = winner;
                
                // Update statistics
                if (winner == PLAYER_X) wins++;
                else if (winner == PLAYER_O) losses++;
                else draws++;
            }
            
            // Switch player
            player = (player == PLAYER_X) ? PLAYER_O : PLAYER_X;
        }
        
        // Backward Q-value update (from final state to initial)
        double reward = (game.winner == PLAYER_X) ? REWARD_WIN :
                       (game.winner == PLAYER_O) ? REWARD_LOSE : REWARD_DRAW;
        
        for (int i = num_transitions - 1; i >= 0; i--) {
            double old_q = get_q_value(qt, transitions[i].board, transitions[i].action);
            
            // Q-learning update rule
            double new_q;
            if (i == num_transitions - 1) {
                // Terminal state
                new_q = old_q + ALPHA * (reward - old_q);
            } else {
                // Non-terminal state
                char next_board[BOARD_SIZE];
                memcpy(next_board, transitions[i].board, BOARD_SIZE);
                next_board[transitions[i].action] = PLAYER_X;
                
                double max_next_q = get_max_q_value(qt, next_board);
                new_q = old_q + ALPHA * (0.0 + GAMMA * max_next_q - old_q);
            }
            
            update_q_value(qt, transitions[i].board, transitions[i].action, new_q);
        }
        
        // Progress report
        if ((episode + 1) % 5000 == 0) {
            double win_rate = (wins * 100.0) / 5000;
            printf("Episode %d: Win: %.1f%%, Draw: %.1f%%, Loss: %.1f%% | Q-entries: %d\n",
                   episode + 1, win_rate, (draws * 100.0) / 5000, 
                   (losses * 100.0) / 5000, qt->total_entries);
            wins = losses = draws = 0;
        }
    }
    
    printf("\n✓ Training complete!\n");
    printf("Total Q-table entries: %d\n", qt->total_entries);
}

// Load minimax dataset to bootstrap Q-values
void load_minimax_dataset(const char *filename, QTable *qt) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("Warning: Could not load minimax dataset from %s\n", filename);
        printf("Starting with zero-initialized Q-values.\n");
        return;
    }
    
    printf("Loading minimax dataset for Q-value initialization...\n");
    
    char line[256];
    int count = 0;
    
    while (fgets(line, sizeof(line), fp) != NULL) {
        line[strcspn(line, "\n")] = 0;
        
        char board[BOARD_SIZE];
        char outcome[10];
        
        // Parse: x,o,b,b,x,o,b,b,x,win
        char *token = strtok(line, ",");
        int i = 0;
        
        while (token != NULL && i < BOARD_SIZE) {
            board[i++] = token[0];
            token = strtok(NULL, ",");
        }
        
        if (token != NULL) {
            strcpy(outcome, token);
            
            // Initialize Q-values based on minimax evaluation
            double init_value = 0.0;
            if (strcmp(outcome, "win") == 0) init_value = 0.8;
            else if (strcmp(outcome, "lose") == 0) init_value = -0.8;
            else init_value = 0.0;
            
            // Set Q-value for all possible actions from this state
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (board[j] == EMPTY) {
                    update_q_value(qt, board, j, init_value);
                }
            }
            
            count++;
        }
    }
    
    fclose(fp);
    printf("✓ Initialized Q-values from %d board states\n", count);
    printf("✓ Total Q-entries: %d\n", qt->total_entries);
}

// Print board
void print_board(char board[BOARD_SIZE]) {
    printf("\n");
    for (int i = 0; i < 9; i += 3) {
        for (int j = 0; j < 3; j++) {
            char c = board[i + j];
            printf(" %c ", c == 'b' ? ' ' : c);
            if (j < 2) printf("|");
        }
        printf("\n");
        if (i < 6) printf("---+---+---\n");
    }
    printf("\n");
}

// Test Q-learning agent
void test_q_learning(QTable *qt, int test_games) {
    printf("\n========================================\n");
    printf("TESTING Q-LEARNING AGENT\n");
    printf("========================================\n");
    printf("Playing %d test games...\n\n", test_games);
    
    int wins = 0, losses = 0, draws = 0;
    
    for (int game_num = 0; game_num < test_games; game_num++) {
        GameState game;
        init_board(&game);
        
        char player = PLAYER_X;
        
        while (!game.game_over) {
            int action;
            
            if (player == PLAYER_X) {
                // Q-learning agent (no exploration)
                action = choose_best_action(qt, game.board);
            } else {
                // Random opponent
                int valid_moves[BOARD_SIZE];
                int num_moves = get_valid_moves(game.board, valid_moves);
                action = valid_moves[rand() % num_moves];
            }
            
            if (action == -1) break;
            
            game.board[action] = player;
            
            char winner = check_winner(game.board);
            if (winner != ' ') {
                game.game_over = 1;
                game.winner = winner;
                
                if (winner == PLAYER_X) wins++;
                else if (winner == PLAYER_O) losses++;
                else draws++;
            }
            
            player = (player == PLAYER_X) ? PLAYER_O : PLAYER_X;
        }
    }
    
    printf("Results against random opponent:\n");
    printf("  Wins:   %d (%.1f%%)\n", wins, (wins * 100.0) / test_games);
    printf("  Losses: %d (%.1f%%)\n", losses, (losses * 100.0) / test_games);
    printf("  Draws:  %d (%.1f%%)\n", draws, (draws * 100.0) / test_games);
    printf("========================================\n");
}

// Save Q-table to file
void save_qtable(const char *filename, QTable *qt) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Error: Could not save Q-table to %s\n", filename);
        return;
    }
    
    fprintf(fp, "# Q-Learning Model\n");
    fprintf(fp, "# Format: board_state,action,q_value,visits\n");
    fprintf(fp, "# Total entries: %d\n\n", qt->total_entries);
    
    for (int i = 0; i < Q_TABLE_SIZE; i++) {
        QEntry *entry = qt->table[i];
        while (entry != NULL) {
            // Write board
            for (int j = 0; j < BOARD_SIZE; j++) {
                fprintf(fp, "%c", entry->board[j]);
                if (j < BOARD_SIZE - 1) fprintf(fp, ",");
            }
            fprintf(fp, ",%d,%.6f,%d\n", entry->action, entry->q_value, entry->visits);
            entry = entry->next;
        }
    }
    
    fclose(fp);
    printf("✓ Q-table saved to: %s\n", filename);
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    
    printf("========================================\n");
    printf("Q-LEARNING FOR TIC-TAC-TOE\n");
    printf("========================================\n\n");
    
    QTable qtable;
    init_qtable(&qtable);
    
    // Optional: Load minimax dataset for initialization
    char dataset_file[256] = "";
    int use_dataset = 0;
    
    if (argc > 1) {
        strcpy(dataset_file, argv[1]);
        use_dataset = 1;
    } else {
        printf("Load minimax dataset for Q-value initialization? (y/n): ");
        char choice;
        scanf(" %c", &choice);
        
        if (choice == 'y' || choice == 'Y') {
            printf("Enter dataset filename [tic-tac-toe-minimax-complete.data]: ");
            scanf("%s", dataset_file);
            if (strlen(dataset_file) == 0) {
                strcpy(dataset_file, "tic-tac-toe-minimax-complete.data");
            }
            use_dataset = 1;
        }
    }
    
    if (use_dataset) {
        load_minimax_dataset(dataset_file, &qtable);
        printf("\n");
    }
    
    // Train Q-learning agent
    train_q_learning(&qtable, MAX_EPISODES);
    
    // Test the agent
    test_q_learning(&qtable, 1000);
    
    // Save Q-table
    printf("\nSaving Q-table...\n");
    save_qtable("q_learning_model.txt", &qtable);
    
    printf("\n========================================\n");
    printf("✓ Q-LEARNING TRAINING COMPLETE!\n");
    printf("========================================\n");
    printf("\nModel saved to: q_learning_model.txt\n");
    printf("Total Q-entries learned: %d\n", qtable.total_entries);
    printf("\nYou can use this model in gameplay by loading the Q-table.\n");
    
    // Cleanup
    free_qtable(&qtable);
    
    return 0;
}
