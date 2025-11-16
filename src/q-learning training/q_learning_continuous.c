// q_learning_continuous.c - Continuous self-play training with checkpointing
// Trains Q-Learning agent through self-play, saves progress periodically
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <signal.h>

#define BOARD_SIZE 9
#define Q_TABLE_SIZE 20000
#define EMPTY 'b'
#define PLAYER_X 'x'
#define PLAYER_O 'o'

// Q-Learning hyperparameters
#define ALPHA 0.3           // Learning rate
#define GAMMA 0.95          // Discount factor
#define EPSILON_START 0.3   // Start with more exploitation (already know basics)
#define EPSILON_MIN 0.05    // Minimum exploration
#define EPSILON_DECAY 0.99995

// Checkpointing
#define CHECKPOINT_INTERVAL 1000  // Save every N episodes
#define STATS_INTERVAL 100        // Print stats every N episodes

// Rewards
#define REWARD_WIN 1.0
#define REWARD_DRAW 0.5
#define REWARD_LOSE -1.0

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

// Training statistics
typedef struct {
    int total_episodes;
    int x_wins;
    int o_wins;
    int draws;
    double current_epsilon;
    time_t start_time;
} TrainingStats;

// Global variables for signal handling
volatile sig_atomic_t keep_running = 1;
QTable global_qtable_x;
QTable global_qtable_o;
TrainingStats global_stats;

// Signal handler for graceful shutdown (Ctrl+C)
void signal_handler(int signum) {
    printf("\n\n[SIGNAL] Received interrupt. Saving models and shutting down...\n");
    keep_running = 0;
}

// Hash function for board state
unsigned long hash_board(char board[BOARD_SIZE]) {
    unsigned long hash = 5381;
    for (int i = 0; i < BOARD_SIZE; i++) {
        hash = ((hash << 5) + hash) + board[i];
    }
    return hash % Q_TABLE_SIZE;
}

// Initialize Q-table
void init_qtable(QTable *qt) {
    for (int i = 0; i < Q_TABLE_SIZE; i++) {
        qt->table[i] = NULL;
    }
    qt->total_entries = 0;
}

// Get Q-value for state-action pair
double get_q_value(QTable *qt, char board[BOARD_SIZE], int action) {
    unsigned long hash = hash_board(board);
    QEntry *entry = qt->table[hash];
    
    while (entry != NULL) {
        if (entry->action == action && memcmp(entry->board, board, BOARD_SIZE) == 0) {
            return entry->q_value;
        }
        entry = entry->next;
    }
    return 0.0; // Default Q-value for unseen states
}

// Update Q-value for state-action pair
void update_q_value(QTable *qt, char board[BOARD_SIZE], int action, double value) {
    unsigned long hash = hash_board(board);
    QEntry *entry = qt->table[hash];
    
    // Search for existing entry
    while (entry != NULL) {
        if (entry->action == action && memcmp(entry->board, board, BOARD_SIZE) == 0) {
            entry->q_value = value;
            entry->visits++;
            return;
        }
        entry = entry->next;
    }
    
    // Create new entry if not found
    QEntry *new_entry = (QEntry *)malloc(sizeof(QEntry));
    memcpy(new_entry->board, board, BOARD_SIZE);
    new_entry->action = action;
    new_entry->q_value = value;
    new_entry->visits = 1;
    new_entry->next = qt->table[hash];
    qt->table[hash] = new_entry;
    qt->total_entries++;
}

// Check for winner
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
        if (board[i] == EMPTY) return ' '; // Game continues
    }
    return 'd'; // Draw
}

// Get valid moves
int get_valid_moves(char board[BOARD_SIZE], int moves[BOARD_SIZE]) {
    int count = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (board[i] == EMPTY) {
            moves[count++] = i;
        }
    }
    return count;
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

// Choose action using epsilon-greedy policy
int choose_action(QTable *qt, char board[BOARD_SIZE], double epsilon) {
    int valid_moves[BOARD_SIZE];
    int num_moves = get_valid_moves(board, valid_moves);
    
    if (num_moves == 0) return -1;
    
    // Exploration: random move
    if ((double)rand() / RAND_MAX < epsilon) {
        return valid_moves[rand() % num_moves];
    }
    
    // Exploitation: best known move
    int best_move = valid_moves[0];
    double best_q = get_q_value(qt, board, best_move);
    
    for (int i = 1; i < num_moves; i++) {
        double q = get_q_value(qt, board, valid_moves[i]);
        if (q > best_q) {
            best_q = q;
            best_move = valid_moves[i];
        }
    }
    
    return best_move;
}

// Play one self-play episode
char play_episode(QTable *qt_x, QTable *qt_o, double epsilon_x, double epsilon_o, 
                  char history_x[][BOARD_SIZE], int actions_x[], int *history_len_x,
                  char history_o[][BOARD_SIZE], int actions_o[], int *history_len_o) {
    char board[BOARD_SIZE];
    for (int i = 0; i < BOARD_SIZE; i++) board[i] = EMPTY;
    
    *history_len_x = 0;
    *history_len_o = 0;
    char current_player = PLAYER_X;
    
    while (1) {
        char winner = check_winner(board);
        if (winner != ' ') return winner;
        
        // Choose and execute action
        int action;
        if (current_player == PLAYER_X) {
            action = choose_action(qt_x, board, epsilon_x);
            if (action == -1) break;
            
            // Record state-action for X
            memcpy(history_x[*history_len_x], board, BOARD_SIZE);
            actions_x[*history_len_x] = action;
            (*history_len_x)++;
        } else {
            action = choose_action(qt_o, board, epsilon_o);
            if (action == -1) break;
            
            // Record state-action for O
            memcpy(history_o[*history_len_o], board, BOARD_SIZE);
            actions_o[*history_len_o] = action;
            (*history_len_o)++;
        }
        
        board[action] = current_player;
        current_player = (current_player == PLAYER_X) ? PLAYER_O : PLAYER_X;
    }
    
    return check_winner(board);
}

// Update Q-values based on episode outcome
void update_episode(QTable *qt, char history[][BOARD_SIZE], int actions[], int history_len, 
                    double final_reward) {
    for (int i = history_len - 1; i >= 0; i--) {
        double current_q = get_q_value(qt, history[i], actions[i]);
        double next_max_q = 0.0;
        
        // If not terminal state, get max Q from next state
        if (i < history_len - 1) {
            next_max_q = get_max_q_value(qt, history[i + 1]);
        }
        
        // Q-learning update: Q(s,a) += α[r + γ·max Q(s',a') - Q(s,a)]
        double td_target = final_reward + GAMMA * next_max_q;
        double new_q = current_q + ALPHA * (td_target - current_q);
        
        update_q_value(qt, history[i], actions[i], new_q);
    }
}

// Save Q-table to file
void save_qtable(const char *filename, QTable *qt) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        printf("[ERROR] Could not save model to %s\n", filename);
        return;
    }
    
    fprintf(fp, "# Q-Learning Model - Continuous Training\n");
    fprintf(fp, "# Format: board_state,action,q_value,visits\n");
    fprintf(fp, "# Total entries: %d\n\n", qt->total_entries);
    
    for (int i = 0; i < Q_TABLE_SIZE; i++) {
        QEntry *entry = qt->table[i];
        while (entry != NULL) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                fprintf(fp, "%c,", entry->board[j]);
            }
            fprintf(fp, "%d,%.6f,%d\n", entry->action, entry->q_value, entry->visits);
            entry = entry->next;
        }
    }
    
    fclose(fp);
}

// Load Q-table from file
int load_qtable(const char *filename, QTable *qt) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return 0;
    
    char line[256];
    int loaded = 0;
    
    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == '#' || line[0] == '\n') continue;
        
        char board[BOARD_SIZE];
        int action, visits;
        double q_value;
        
        // Parse line
        char *token = strtok(line, ",");
        for (int i = 0; i < BOARD_SIZE && token != NULL; i++) {
            board[i] = token[0];
            token = strtok(NULL, ",");
        }
        
        if (token != NULL) {
            action = atoi(token);
            token = strtok(NULL, ",");
            if (token != NULL) {
                q_value = atof(token);
                token = strtok(NULL, ",\n");
                visits = (token != NULL) ? atoi(token) : 1;
                
                // Add entry to Q-table
                unsigned long hash = hash_board(board);
                QEntry *new_entry = (QEntry *)malloc(sizeof(QEntry));
                memcpy(new_entry->board, board, BOARD_SIZE);
                new_entry->action = action;
                new_entry->q_value = q_value;
                new_entry->visits = visits;
                new_entry->next = qt->table[hash];
                qt->table[hash] = new_entry;
                qt->total_entries++;
                loaded++;
            }
        }
    }
    
    fclose(fp);
    return loaded;
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
        qt->table[i] = NULL;
    }
    qt->total_entries = 0;
}

// Print training statistics
void print_stats(TrainingStats *stats) {
    time_t current_time = time(NULL);
    double elapsed = difftime(current_time, stats->start_time);
    int total_games = stats->x_wins + stats->o_wins + stats->draws;
    
    printf("\n=== Episode %d ===\n", stats->total_episodes);
    printf("Last %d games: X:%d (%.1f%%) O:%d (%.1f%%) Draw:%d (%.1f%%)\n",
           STATS_INTERVAL,
           stats->x_wins, stats->x_wins * 100.0 / STATS_INTERVAL,
           stats->o_wins, stats->o_wins * 100.0 / STATS_INTERVAL,
           stats->draws, stats->draws * 100.0 / STATS_INTERVAL);
    printf("Epsilon: %.4f | Q-table size: X=%d, O=%d | Time: %.0fs\n",
           stats->current_epsilon, global_qtable_x.total_entries, 
           global_qtable_o.total_entries, elapsed);
}

// Main training loop
void train_continuous(const char *model_x_path, const char *model_o_path, int max_episodes) {
    printf("\n========================================\n");
    printf("CONTINUOUS Q-LEARNING TRAINING (SELF-PLAY)\n");
    printf("========================================\n\n");
    
    // Initialize or load models
    init_qtable(&global_qtable_x);
    init_qtable(&global_qtable_o);
    
    printf("Loading existing models (if available)...\n");
    int loaded_x = load_qtable(model_x_path, &global_qtable_x);
    int loaded_o = load_qtable(model_o_path, &global_qtable_o);
    
    if (loaded_x > 0) printf("  Loaded X model: %d entries\n", loaded_x);
    else printf("  Starting X model from scratch\n");
    
    if (loaded_o > 0) printf("  Loaded O model: %d entries\n", loaded_o);
    else printf("  Starting O model from scratch\n");
    
    // Initialize statistics
    global_stats.total_episodes = 0;
    global_stats.x_wins = 0;
    global_stats.o_wins = 0;
    global_stats.draws = 0;
    global_stats.current_epsilon = EPSILON_START;
    global_stats.start_time = time(NULL);
    
    printf("\nTraining parameters:\n");
    printf("  Learning rate (α): %.2f\n", ALPHA);
    printf("  Discount factor (γ): %.2f\n", GAMMA);
    printf("  Epsilon: %.2f → %.2f (decay: %.5f)\n", 
           EPSILON_START, EPSILON_MIN, EPSILON_DECAY);
    printf("  Checkpoint interval: %d episodes\n", CHECKPOINT_INTERVAL);
    printf("  Max episodes: %s\n\n", max_episodes > 0 ? "Limited" : "Unlimited");
    
    printf("Press Ctrl+C to stop training and save models.\n");
    printf("Training started...\n\n");
    
    // Training loop
    while (keep_running && (max_episodes <= 0 || global_stats.total_episodes < max_episodes)) {
        // Play one episode
        char history_x[50][BOARD_SIZE], history_o[50][BOARD_SIZE];
        int actions_x[50], actions_o[50];
        int history_len_x, history_len_o;
        
        char winner = play_episode(&global_qtable_x, &global_qtable_o,
                                   global_stats.current_epsilon, global_stats.current_epsilon,
                                   history_x, actions_x, &history_len_x,
                                   history_o, actions_o, &history_len_o);
        
        // Update Q-values based on outcome
        double reward_x, reward_o;
        if (winner == PLAYER_X) {
            reward_x = REWARD_WIN;
            reward_o = REWARD_LOSE;
            global_stats.x_wins++;
        } else if (winner == PLAYER_O) {
            reward_x = REWARD_LOSE;
            reward_o = REWARD_WIN;
            global_stats.o_wins++;
        } else {
            reward_x = REWARD_DRAW;
            reward_o = REWARD_DRAW;
            global_stats.draws++;
        }
        
        update_episode(&global_qtable_x, history_x, actions_x, history_len_x, reward_x);
        update_episode(&global_qtable_o, history_o, actions_o, history_len_o, reward_o);
        
        global_stats.total_episodes++;
        
        // Decay epsilon
        if (global_stats.current_epsilon > EPSILON_MIN) {
            global_stats.current_epsilon *= EPSILON_DECAY;
        }
        
        // Print statistics
        if (global_stats.total_episodes % STATS_INTERVAL == 0) {
            print_stats(&global_stats);
            // Reset interval stats
            global_stats.x_wins = 0;
            global_stats.o_wins = 0;
            global_stats.draws = 0;
        }
        
        // Save checkpoint
        if (global_stats.total_episodes % CHECKPOINT_INTERVAL == 0) {
            printf("\n[CHECKPOINT] Saving models...\n");
            save_qtable(model_x_path, &global_qtable_x);
            save_qtable(model_o_path, &global_qtable_o);
            printf("[CHECKPOINT] Models saved successfully.\n\n");
        }
    }
    
    // Final save
    printf("\n\n========================================\n");
    printf("TRAINING COMPLETE\n");
    printf("========================================\n");
    printf("Total episodes: %d\n", global_stats.total_episodes);
    printf("Saving final models...\n");
    save_qtable(model_x_path, &global_qtable_x);
    save_qtable(model_o_path, &global_qtable_o);
    printf("Models saved to:\n  %s\n  %s\n", model_x_path, model_o_path);
    
    // Cleanup
    free_qtable(&global_qtable_x);
    free_qtable(&global_qtable_o);
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    
    // Register signal handler for Ctrl+C
    signal(SIGINT, signal_handler);
    
    // Default paths
    const char *model_x_path = "../../models/q learning/q_learning_x_continuous.txt";
    const char *model_o_path = "../../models/q learning/q_learning_o_continuous.txt";
    int max_episodes = 0; // 0 = unlimited
    
    // Parse command line arguments
    if (argc > 1) max_episodes = atoi(argv[1]);
    if (argc > 2) model_x_path = argv[2];
    if (argc > 3) model_o_path = argv[3];
    
    train_continuous(model_x_path, model_o_path, max_episodes);
    
    return 0;
}
