// q_learning_with_dataset.c - Q-Learning with PROPER dataset initialization
// This version initializes Q-values per-move using Minimax evaluation
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define BOARD_SIZE 9
#define Q_TABLE_SIZE 20000
#define MAX_EPISODES 100000
#define EMPTY 'b'
#define PLAYER_X 'x'
#define PLAYER_O 'o'

// Q-Learning hyperparameters
#define ALPHA 0.3
#define GAMMA 0.95
#define EPSILON_START 0.9
#define EPSILON_END 0.05
#define EPSILON_DECAY 0.9999

// Rewards
#define REWARD_WIN 1.0
#define REWARD_DRAW 0.5
#define REWARD_LOSE -1.0
#define REWARD_STEP -0.01

// Structure for Q-table entry
typedef struct QEntry {
    char board[BOARD_SIZE];
    int action;
    double q_value;
    int visits;
    struct QEntry *next;
} QEntry;

typedef struct {
    QEntry *table[Q_TABLE_SIZE];
    int total_entries;
} QTable;

typedef struct {
    char board[BOARD_SIZE];
    int game_over;
    char winner;
} GameState;

// Forward declarations
void init_qtable(QTable *qt);
void free_qtable(QTable *qt);
unsigned long hash_board(char board[BOARD_SIZE]);
double get_q_value(QTable *qt, char board[BOARD_SIZE], int action);
void update_q_value(QTable *qt, char board[BOARD_SIZE], int action, double value);
void init_board(GameState *game);
char check_winner(char board[BOARD_SIZE]);
int get_valid_moves(char board[BOARD_SIZE], int moves[BOARD_SIZE]);
int choose_action_epsilon_greedy(QTable *qt, char board[BOARD_SIZE], char player, double epsilon);
int choose_best_action(QTable *qt, char board[BOARD_SIZE], char player);
double get_max_q_value(QTable *qt, char board[BOARD_SIZE], char player);
void train_q_learning(QTable *qt, int episodes);
void save_qtable(const char *filename, QTable *qt);
void test_against_random(QTable *qt, int test_games);
void test_against_minimax(QTable *qt, int test_games);
void print_board(char board[BOARD_SIZE]);
int minimax_move(char board[BOARD_SIZE], char player, int depth_limit);
int minimax_eval(char board[BOARD_SIZE], char player, int depth, int is_maximizing, int depth_limit);
void load_dataset_with_minimax_init(const char *filename, QTable *qt);

// Minimax implementation
int eval_board(char board[BOARD_SIZE], char player) {
    char winner = check_winner(board);
    if (winner == player) return 10;
    if (winner != ' ' && winner != 'd') return -10;
    return 0;
}

int minimax_eval(char board[BOARD_SIZE], char player, int depth, int is_maximizing, int depth_limit) {
    char winner = check_winner(board);
    if (winner != ' ' || depth == depth_limit) {
        return eval_board(board, player);
    }
    
    char current_player = is_maximizing ? player : (player == PLAYER_O ? PLAYER_X : PLAYER_O);
    int valid_moves[BOARD_SIZE];
    int num_moves = get_valid_moves(board, valid_moves);
    
    if (is_maximizing) {
        int max_eval = -1000;
        for (int i = 0; i < num_moves; i++) {
            board[valid_moves[i]] = current_player;
            int eval = minimax_eval(board, player, depth + 1, 0, depth_limit);
            board[valid_moves[i]] = EMPTY;
            if (eval > max_eval) max_eval = eval;
        }
        return max_eval;
    } else {
        int min_eval = 1000;
        for (int i = 0; i < num_moves; i++) {
            board[valid_moves[i]] = current_player;
            int eval = minimax_eval(board, player, depth + 1, 1, depth_limit);
            board[valid_moves[i]] = EMPTY;
            if (eval < min_eval) min_eval = eval;
        }
        return min_eval;
    }
}

int minimax_move(char board[BOARD_SIZE], char player, int depth_limit) {
    int valid_moves[BOARD_SIZE];
    int num_moves = get_valid_moves(board, valid_moves);
    if (num_moves == 0) return -1;
    
    int best_move = valid_moves[0];
    int best_eval = -1000;
    
    for (int i = 0; i < num_moves; i++) {
        board[valid_moves[i]] = player;
        int eval = minimax_eval(board, player, 0, 0, depth_limit);
        board[valid_moves[i]] = EMPTY;
        
        if (eval > best_eval) {
            best_eval = eval;
            best_move = valid_moves[i];
        }
    }
    
    return best_move;
}

void init_qtable(QTable *qt) {
    for (int i = 0; i < Q_TABLE_SIZE; i++) {
        qt->table[i] = NULL;
    }
    qt->total_entries = 0;
}

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

unsigned long hash_board(char board[BOARD_SIZE]) {
    unsigned long hash = 5381;
    for (int i = 0; i < BOARD_SIZE; i++) {
        hash = ((hash << 5) + hash) + board[i];
    }
    return hash % Q_TABLE_SIZE;
}

double get_q_value(QTable *qt, char board[BOARD_SIZE], int action) {
    unsigned long hash = hash_board(board);
    QEntry *entry = qt->table[hash];
    
    while (entry != NULL) {
        if (entry->action == action && memcmp(entry->board, board, BOARD_SIZE) == 0) {
            return entry->q_value;
        }
        entry = entry->next;
    }
    
    return 0.0;
}

void update_q_value(QTable *qt, char board[BOARD_SIZE], int action, double value) {
    unsigned long hash = hash_board(board);
    QEntry *entry = qt->table[hash];
    
    while (entry != NULL) {
        if (entry->action == action && memcmp(entry->board, board, BOARD_SIZE) == 0) {
            entry->q_value = value;
            entry->visits++;
            return;
        }
        entry = entry->next;
    }
    
    QEntry *new_entry = (QEntry *)malloc(sizeof(QEntry));
    memcpy(new_entry->board, board, BOARD_SIZE);
    new_entry->action = action;
    new_entry->q_value = value;
    new_entry->visits = 1;
    new_entry->next = qt->table[hash];
    qt->table[hash] = new_entry;
    qt->total_entries++;
}

void init_board(GameState *game) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        game->board[i] = EMPTY;
    }
    game->game_over = 0;
    game->winner = ' ';
}

char check_winner(char board[BOARD_SIZE]) {
    int wins[8][3] = {
        {0, 1, 2}, {3, 4, 5}, {6, 7, 8},
        {0, 3, 6}, {1, 4, 7}, {2, 5, 8},
        {0, 4, 8}, {2, 4, 6}
    };
    
    for (int i = 0; i < 8; i++) {
        if (board[wins[i][0]] == board[wins[i][1]] && 
            board[wins[i][1]] == board[wins[i][2]] && 
            board[wins[i][0]] != EMPTY) {
            return board[wins[i][0]];
        }
    }
    
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (board[i] == EMPTY) return ' ';
    }
    
    return 'd';
}

int get_valid_moves(char board[BOARD_SIZE], int moves[BOARD_SIZE]) {
    int count = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (board[i] == EMPTY) {
            moves[count++] = i;
        }
    }
    return count;
}

int choose_action_epsilon_greedy(QTable *qt, char board[BOARD_SIZE], char player, double epsilon) {
    int valid_moves[BOARD_SIZE];
    int num_moves = get_valid_moves(board, valid_moves);
    
    if (num_moves == 0) return -1;
    
    if ((double)rand() / RAND_MAX < epsilon) {
        return valid_moves[rand() % num_moves];
    }
    
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

int choose_best_action(QTable *qt, char board[BOARD_SIZE], char player) {
    return choose_action_epsilon_greedy(qt, board, player, 0.0);
}

double get_max_q_value(QTable *qt, char board[BOARD_SIZE], char player) {
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

// PROPER dataset initialization - evaluates each move individually
void load_dataset_with_minimax_init(const char *filename, QTable *qt) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("Warning: Could not load dataset from %s\n", filename);
        printf("Starting with zero-initialized Q-values.\n");
        return;
    }
    
    printf("\n========================================\n");
    printf("DATASET INITIALIZATION (PROPER METHOD)\n");
    printf("========================================\n");
    printf("Loading: %s\n", filename);
    printf("Method: Per-move Minimax evaluation\n");
    printf("Evaluating each possible move...\n\n");
    
    char line[256];
    int boards_processed = 0;
    int moves_initialized = 0;
    
    while (fgets(line, sizeof(line), fp) != NULL) {
        line[strcspn(line, "\n")] = 0;
        if (strlen(line) == 0) continue;
        
        char board[BOARD_SIZE];
        char outcome[10];
        
        // Parse: x,o,b,b,x,o,b,b,x,win
        char *token = strtok(line, ",");
        int i = 0;
        
        while (token != NULL && i < BOARD_SIZE) {
            board[i++] = token[0];
            token = strtok(NULL, ",");
        }
        
        if (token != NULL && i == BOARD_SIZE) {
            strcpy(outcome, token);
            boards_processed++;
            
            // For each empty position, evaluate the move with Minimax
            for (int pos = 0; pos < BOARD_SIZE; pos++) {
                if (board[pos] == EMPTY) {
                    // Try making this move as O
                    board[pos] = PLAYER_O;
                    
                    // Evaluate resulting position with Minimax (depth 4 = balanced quality/speed)
                    int minimax_score = minimax_eval(board, PLAYER_O, 0, 0, 4);
                    
                    // Restore board
                    board[pos] = EMPTY;
                    
                    // Convert minimax score (-10 to +10) to Q-value range (-1.0 to +1.0)
                    // Scale down and add small noise to avoid exact ties
                    double init_q = (minimax_score / 15.0) + ((rand() / (double)RAND_MAX) * 0.05 - 0.025);
                    
                    // Initialize this state-action pair
                    update_q_value(qt, board, pos, init_q);
                    moves_initialized++;
                }
            }
        }
        
        if (boards_processed % 500 == 0 && boards_processed > 0) {
            printf("  Processed %d boards, initialized %d moves...\r", boards_processed, moves_initialized);
            fflush(stdout);
        }
    }
    
    fclose(fp);
    printf("\n\n✓ Dataset initialization complete!\n");
    printf("  Boards processed: %d\n", boards_processed);
    printf("  Moves initialized: %d\n", moves_initialized);
    printf("  Q-table entries: %d\n", qt->total_entries);
    printf("  Average moves per board: %.1f\n\n", (double)moves_initialized / boards_processed);
}

void train_q_learning(QTable *qt, int episodes) {
    printf("========================================\n");
    printf("Q-LEARNING TRAINING (With Dataset Init)\n");
    printf("========================================\n");
    printf("Episodes: %d\n", episodes);
    printf("Alpha: %.3f, Gamma: %.3f\n", ALPHA, GAMMA);
    printf("Epsilon: %.2f → %.2f\n", EPSILON_START, EPSILON_END);
    printf("Training O against Minimax (depth 2)\n");
    printf("========================================\n\n");
    
    int wins = 0, losses = 0, draws = 0;
    double epsilon = EPSILON_START;
    
    for (int episode = 0; episode < episodes; episode++) {
        GameState game;
        init_board(&game);
        
        typedef struct {
            char board[BOARD_SIZE];
            int action;
            double reward;
        } Transition;
        
        Transition transitions[10];
        int num_transitions = 0;
        
        char player = PLAYER_X;
        
        while (!game.game_over) {
            int action;
            
            if (player == PLAYER_O) {
                action = choose_action_epsilon_greedy(qt, game.board, PLAYER_O, epsilon);
                
                if (action != -1) {
                    memcpy(transitions[num_transitions].board, game.board, BOARD_SIZE);
                    transitions[num_transitions].action = action;
                    transitions[num_transitions].reward = REWARD_STEP;
                    num_transitions++;
                }
            } else {
                action = minimax_move(game.board, PLAYER_X, 4);
            }
            
            if (action == -1) break;
            
            game.board[action] = player;
            
            char winner = check_winner(game.board);
            if (winner != ' ') {
                game.game_over = 1;
                game.winner = winner;
                
                if (winner == PLAYER_O) wins++;
                else if (winner == PLAYER_X) losses++;
                else draws++;
            }
            
            player = (player == PLAYER_X) ? PLAYER_O : PLAYER_X;
        }
        
        double final_reward = (game.winner == PLAYER_O) ? REWARD_WIN :
                             (game.winner == PLAYER_X) ? REWARD_LOSE : REWARD_DRAW;
        
        for (int i = num_transitions - 1; i >= 0; i--) {
            char current_board[BOARD_SIZE];
            memcpy(current_board, transitions[i].board, BOARD_SIZE);
            int action = transitions[i].action;
            
            double old_q = get_q_value(qt, current_board, action);
            double new_q;
            
            if (i == num_transitions - 1) {
                new_q = old_q + ALPHA * (final_reward - old_q);
            } else {
                char next_board[BOARD_SIZE];
                memcpy(next_board, transitions[i + 1].board, BOARD_SIZE);
                
                double max_next_q = get_max_q_value(qt, next_board, PLAYER_O);
                double immediate_reward = transitions[i].reward;
                new_q = old_q + ALPHA * (immediate_reward + GAMMA * max_next_q - old_q);
            }
            
            update_q_value(qt, current_board, action, new_q);
        }
        
        epsilon = EPSILON_END + (EPSILON_START - EPSILON_END) * exp(-episode / (episodes / 5.0));
        
        if ((episode + 1) % 5000 == 0) {
            printf("Episode %6d | W: %4d (%.1f%%) D: %4d (%.1f%%) L: %4d (%.1f%%) | ε: %.3f | Q-entries: %d\n",
                   episode + 1, wins, (wins * 100.0) / 5000, draws, (draws * 100.0) / 5000,
                   losses, (losses * 100.0) / 5000, epsilon, qt->total_entries);
            wins = losses = draws = 0;
        }
    }
    
    printf("\n✓ Training complete! Total Q-entries: %d\n", qt->total_entries);
}

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

void test_against_random(QTable *qt, int test_games) {
    printf("\n========================================\n");
    printf("TEST: Q-Learning (O) vs Random (X)\n");
    printf("========================================\n");
    
    int wins = 0, losses = 0, draws = 0;
    
    for (int game_num = 0; game_num < test_games; game_num++) {
        GameState game;
        init_board(&game);
        char player = PLAYER_X;
        
        while (!game.game_over) {
            int action;
            
            if (player == PLAYER_O) {
                action = choose_best_action(qt, game.board, PLAYER_O);
            } else {
                int valid_moves[BOARD_SIZE];
                int num_moves = get_valid_moves(game.board, valid_moves);
                action = valid_moves[rand() % num_moves];
            }
            
            if (action == -1) break;
            game.board[action] = player;
            
            char winner = check_winner(game.board);
            if (winner != ' ') {
                game.game_over = 1;
                if (winner == PLAYER_O) wins++;
                else if (winner == PLAYER_X) losses++;
                else draws++;
            }
            
            player = (player == PLAYER_X) ? PLAYER_O : PLAYER_X;
        }
    }
    
    printf("Results: W: %d (%.1f%%) | D: %d (%.1f%%) | L: %d (%.1f%%)\n",
           wins, (wins * 100.0) / test_games,
           draws, (draws * 100.0) / test_games,
           losses, (losses * 100.0) / test_games);
}

void test_against_minimax(QTable *qt, int test_games) {
    printf("\n========================================\n");
    printf("TEST: Q-Learning (O) vs Minimax Easy (X)\n");
    printf("========================================\n");
    
    int wins = 0, losses = 0, draws = 0;
    
    for (int game_num = 0; game_num < test_games; game_num++) {
        GameState game;
        init_board(&game);
        char player = PLAYER_X;
        
        while (!game.game_over) {
            int action;
            
            if (player == PLAYER_O) {
                action = choose_best_action(qt, game.board, PLAYER_O);
            } else {
                action = minimax_move(game.board, PLAYER_X, 4);
            }
            
            if (action == -1) break;
            game.board[action] = player;
            
            char winner = check_winner(game.board);
            if (winner != ' ') {
                game.game_over = 1;
                if (winner == PLAYER_O) wins++;
                else if (winner == PLAYER_X) losses++;
                else draws++;
            }
            
            player = (player == PLAYER_X) ? PLAYER_O : PLAYER_X;
        }
    }
    
    printf("Results: W: %d (%.1f%%) | D: %d (%.1f%%) | L: %d (%.1f%%)\n",
           wins, (wins * 100.0) / test_games,
           draws, (draws * 100.0) / test_games,
           losses, (losses * 100.0) / test_games);
}

void save_qtable(const char *filename, QTable *qt) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Error: Could not save Q-table to %s\n", filename);
        return;
    }
    
    fprintf(fp, "# Q-Learning Model (Dataset-Initialized)\n");
    fprintf(fp, "# Format: board_state,action,q_value,visits\n");
    fprintf(fp, "# Total entries: %d\n\n", qt->total_entries);
    
    for (int i = 0; i < Q_TABLE_SIZE; i++) {
        QEntry *entry = qt->table[i];
        while (entry != NULL) {
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
    printf("Q-LEARNING WITH DATASET INITIALIZATION\n");
    printf("========================================\n\n");
    
    QTable qtable;
    init_qtable(&qtable);
    
    // Determine dataset file
    char dataset_file[256];
    if (argc > 2) {
        strcpy(dataset_file, argv[2]);
    } else {
        strcpy(dataset_file, "../dataset/tic-tac-toe-minimax-non-terminal.data");
    }
    
    // Load and initialize from dataset
    load_dataset_with_minimax_init(dataset_file, &qtable);
    
    // Determine training episodes
    int episodes = MAX_EPISODES;
    if (argc > 1) {
        episodes = atoi(argv[1]);
    }
    
    // Train
    train_q_learning(&qtable, episodes);
    
    // Test
    test_against_random(&qtable, 1000);
    test_against_minimax(&qtable, 100);
    
    // Save
    printf("\nSaving model...\n");
    save_qtable("q_learning_with_dataset.txt", &qtable);
    
    printf("\n========================================\n");
    printf("✓ TRAINING COMPLETE\n");
    printf("========================================\n");
    printf("Model saved to: q_learning_with_dataset.txt\n");
    printf("Total Q-entries: %d\n", qtable.total_entries);
    printf("\nCompare this with q_learning_fixed.txt to see\n");
    printf("if dataset initialization helps or hurts!\n");
    
    free_qtable(&qtable);
    return 0;
}
