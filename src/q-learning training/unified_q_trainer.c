// unified_q_trainer.c - Unified Q-Learning training system
// Supports: from-scratch, dataset-init, and resume from checkpoint
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

// Training modes
typedef enum {
    MODE_FROM_SCRATCH,      // Pure RL from zero
    MODE_DATASET_INIT,      // Initialize from dataset
    MODE_RESUME             // Resume from checkpoint
} TrainingMode;

// Q-Learning hyperparameters
#define ALPHA 0.3
#define GAMMA 0.95
#define EPSILON_MIN 0.05
#define EPSILON_DECAY 0.99995

// Checkpointing
#define CHECKPOINT_INTERVAL 1000
#define STATS_INTERVAL 100

// Rewards
#define REWARD_WIN 1.0
#define REWARD_DRAW 0.5
#define REWARD_LOSE -1.0

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
    int total_episodes;
    int x_wins;
    int o_wins;
    int draws;
    double current_epsilon;
    time_t start_time;
    TrainingMode mode;
} TrainingStats;

volatile sig_atomic_t keep_running = 1;
QTable global_qtable_o;
TrainingStats global_stats;

void signal_handler(int signum) {
    printf("\n\n[SIGNAL] Received interrupt. Saving model and shutting down...\n");
    keep_running = 0;
}

unsigned long hash_board(char board[BOARD_SIZE]) {
    unsigned long hash = 5381;
    for (int i = 0; i < BOARD_SIZE; i++) {
        hash = ((hash << 5) + hash) + board[i];
    }
    return hash % Q_TABLE_SIZE;
}

void init_qtable(QTable *qt) {
    for (int i = 0; i < Q_TABLE_SIZE; i++) {
        qt->table[i] = NULL;
    }
    qt->total_entries = 0;
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

int choose_action(QTable *qt, char board[BOARD_SIZE], double epsilon) {
    int valid_moves[BOARD_SIZE];
    int num_moves = get_valid_moves(board, valid_moves);
    
    if (num_moves == 0) return -1;
    
    if ((double)rand() / RAND_MAX < epsilon) {
        return valid_moves[rand() % num_moves];
    }
    
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

// Minimax implementation
int eval_board(char board[BOARD_SIZE]) {
    char winner = check_winner(board);
    if (winner == PLAYER_O) return 10;
    if (winner == PLAYER_X) return -10;
    return 0;
}

int minimax(char board[BOARD_SIZE], int depth, int is_max, int depth_limit) {
    int score = eval_board(board);
    if (score == 10) return score - depth;
    if (score == -10) return score + depth;
    char winner = check_winner(board);
    if (winner != ' ') return 0;
    if (depth_limit > 0 && depth >= depth_limit) return 0;
    
    if (is_max) {
        int best = -1000;
        for (int i = 0; i < BOARD_SIZE; i++) {
            if (board[i] == EMPTY) {
                board[i] = PLAYER_O;
                int val = minimax(board, depth + 1, 0, depth_limit);
                board[i] = EMPTY;
                if (val > best) best = val;
            }
        }
        return best;
    } else {
        int best = 1000;
        for (int i = 0; i < BOARD_SIZE; i++) {
            if (board[i] == EMPTY) {
                board[i] = PLAYER_X;
                int val = minimax(board, depth + 1, 1, depth_limit);
                board[i] = EMPTY;
                if (val < best) best = val;
            }
        }
        return best;
    }
}

int minimax_move(char board[BOARD_SIZE], int depth_limit) {
    int best_move = -1, best_val = -1000;
    
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (board[i] == EMPTY) {
            board[i] = PLAYER_O;
            int val = minimax(board, 0, 0, depth_limit);
            board[i] = EMPTY;
            
            if (val > best_val) {
                best_val = val;
                best_move = i;
            }
        }
    }
    
    return best_move;
}

void load_dataset_with_minimax_init(const char *filename, QTable *qt) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("Warning: Could not load dataset from %s\n", filename);
        return;
    }
    
    printf("Loading dataset: %s\n", filename);
    printf("Initializing Q-values with Minimax evaluation...\n");
    
    char line[256];
    int boards_processed = 0, moves_initialized = 0;
    
    while (fgets(line, sizeof(line), fp) != NULL) {
        line[strcspn(line, "\n")] = 0;
        if (strlen(line) == 0) continue;
        
        char board[BOARD_SIZE];
        char *token = strtok(line, ",");
        int i = 0;
        
        while (token != NULL && i < BOARD_SIZE) {
            board[i++] = token[0];
            token = strtok(NULL, ",");
        }
        
        if (i == BOARD_SIZE) {
            boards_processed++;
            
            for (int pos = 0; pos < BOARD_SIZE; pos++) {
                if (board[pos] == EMPTY) {
                    board[pos] = PLAYER_O;
                    int minimax_score = minimax(board, 0, 0, 4);
                    board[pos] = EMPTY;
                    
                    double init_q = (minimax_score / 15.0) + ((rand() / (double)RAND_MAX) * 0.05 - 0.025);
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
    printf("\n✓ Dataset initialization complete!\n");
    printf("  Boards: %d | Moves: %d | Q-entries: %d\n\n", 
           boards_processed, moves_initialized, qt->total_entries);
}

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

char play_episode(QTable *qt_o, double epsilon_o, 
                  char history[][BOARD_SIZE], int actions[], int *history_len) {
    char board[BOARD_SIZE];
    for (int i = 0; i < BOARD_SIZE; i++) board[i] = EMPTY;
    
    *history_len = 0;
    char current_player = PLAYER_X;
    
    while (1) {
        char winner = check_winner(board);
        if (winner != ' ') return winner;
        
        int action;
        if (current_player == PLAYER_X) {
            action = minimax_move(board, 4);
            if (action == -1) break;
        } else {
            action = choose_action(qt_o, board, epsilon_o);
            if (action == -1) break;
            
            memcpy(history[*history_len], board, BOARD_SIZE);
            actions[*history_len] = action;
            (*history_len)++;
        }
        
        board[action] = current_player;
        current_player = (current_player == PLAYER_X) ? PLAYER_O : PLAYER_X;
    }
    
    return check_winner(board);
}

void update_episode(QTable *qt, char history[][BOARD_SIZE], int actions[], int history_len, double final_reward) {
    for (int i = history_len - 1; i >= 0; i--) {
        double current_q = get_q_value(qt, history[i], actions[i]);
        double next_max_q = 0.0;
        
        if (i < history_len - 1) {
            next_max_q = get_max_q_value(qt, history[i + 1]);
        }
        
        double td_target = final_reward + GAMMA * next_max_q;
        double new_q = current_q + ALPHA * (td_target - current_q);
        
        update_q_value(qt, history[i], actions[i], new_q);
    }
}

void save_qtable(const char *filename, QTable *qt, TrainingMode mode) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        printf("[ERROR] Could not save model to %s\n", filename);
        return;
    }
    
    const char *mode_str = (mode == MODE_FROM_SCRATCH) ? "From-Scratch" : 
                          (mode == MODE_DATASET_INIT) ? "Dataset-Init" : "Resumed";
    
    fprintf(fp, "# Q-Learning Model - %s Training\n", mode_str);
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

void print_stats(TrainingStats *stats) {
    time_t current_time = time(NULL);
    double elapsed = difftime(current_time, stats->start_time);
    
    printf("\n=== Episode %d ===\n", stats->total_episodes);
    printf("Last %d games: O:%d (%.1f%%) X:%d (%.1f%%) Draw:%d (%.1f%%)\n",
           STATS_INTERVAL,
           stats->o_wins, stats->o_wins * 100.0 / STATS_INTERVAL,
           stats->x_wins, stats->x_wins * 100.0 / STATS_INTERVAL,
           stats->draws, stats->draws * 100.0 / STATS_INTERVAL);
    printf("Epsilon: %.4f | Q-entries: %d | Time: %.0fs\n",
           stats->current_epsilon, global_qtable_o.total_entries, elapsed);
}

void train_continuous(TrainingMode mode, const char *dataset_file, const char *checkpoint_file, 
                     const char *output_file, int max_episodes) {
    printf("\n========================================\n");
    printf("UNIFIED Q-LEARNING TRAINING SYSTEM\n");
    printf("========================================\n\n");
    
    init_qtable(&global_qtable_o);
    double epsilon_start;
    
    // Determine initialization strategy
    if (mode == MODE_RESUME) {
        printf("Mode: RESUME from checkpoint\n");
        int loaded = load_qtable(checkpoint_file, &global_qtable_o);
        if (loaded > 0) {
            printf("✓ Loaded checkpoint: %d entries\n\n", loaded);
            epsilon_start = 0.1; // Lower epsilon for refinement
            global_stats.mode = MODE_RESUME;
        } else {
            printf("✗ Checkpoint not found. Switching to dataset-init mode.\n\n");
            mode = MODE_DATASET_INIT;
        }
    }
    
    if (mode == MODE_DATASET_INIT) {
        printf("Mode: DATASET-INITIALIZED\n");
        load_dataset_with_minimax_init(dataset_file, &global_qtable_o);
        epsilon_start = 0.2; // Moderate epsilon
        global_stats.mode = MODE_DATASET_INIT;
    } else if (mode == MODE_FROM_SCRATCH) {
        printf("Mode: FROM-SCRATCH (Pure RL)\n");
        printf("Starting with empty Q-table\n\n");
        epsilon_start = 0.3; // Higher epsilon for exploration
        global_stats.mode = MODE_FROM_SCRATCH;
    }
    
    global_stats.total_episodes = 0;
    global_stats.x_wins = 0;
    global_stats.o_wins = 0;
    global_stats.draws = 0;
    global_stats.current_epsilon = epsilon_start;
    global_stats.start_time = time(NULL);
    
    printf("Training parameters:\n");
    printf("  Initial entries: %d\n", global_qtable_o.total_entries);
    printf("  Learning rate (α): %.2f\n", ALPHA);
    printf("  Discount (γ): %.2f\n", GAMMA);
    printf("  Epsilon: %.2f → %.2f\n", epsilon_start, EPSILON_MIN);
    printf("  Checkpoint: every %d episodes\n", CHECKPOINT_INTERVAL);
    printf("  Output: %s\n\n", output_file);
    
    printf("Press Ctrl+C to stop and save.\nTraining...\n");
    
    while (keep_running && (max_episodes <= 0 || global_stats.total_episodes < max_episodes)) {
        char history[50][BOARD_SIZE];
        int actions[50];
        int history_len;
        
        char winner = play_episode(&global_qtable_o, global_stats.current_epsilon,
                                   history, actions, &history_len);
        
        double reward;
        if (winner == PLAYER_O) {
            reward = REWARD_WIN;
            global_stats.o_wins++;
        } else if (winner == PLAYER_X) {
            reward = REWARD_LOSE;
            global_stats.x_wins++;
        } else {
            reward = REWARD_DRAW;
            global_stats.draws++;
        }
        
        update_episode(&global_qtable_o, history, actions, history_len, reward);
        
        global_stats.total_episodes++;
        
        if (global_stats.current_epsilon > EPSILON_MIN) {
            global_stats.current_epsilon *= EPSILON_DECAY;
        }
        
        if (global_stats.total_episodes % STATS_INTERVAL == 0) {
            print_stats(&global_stats);
            global_stats.x_wins = 0;
            global_stats.o_wins = 0;
            global_stats.draws = 0;
        }
        
        if (global_stats.total_episodes % CHECKPOINT_INTERVAL == 0) {
            printf("\n[CHECKPOINT] Saving...\n");
            save_qtable(output_file, &global_qtable_o, global_stats.mode);
            printf("[CHECKPOINT] Saved.\n\n");
        }
    }
    
    printf("\n\n========================================\n");
    printf("TRAINING COMPLETE\n");
    printf("========================================\n");
    printf("Episodes: %d | Entries: %d\n", global_stats.total_episodes, global_qtable_o.total_entries);
    printf("Saving final model...\n");
    save_qtable(output_file, &global_qtable_o, global_stats.mode);
    printf("✓ Saved to: %s\n", output_file);
    
    free_qtable(&global_qtable_o);
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    signal(SIGINT, signal_handler);
    
    if (argc < 2) {
        printf("Usage: %s <mode> [options]\n", argv[0]);
        printf("Modes:\n");
        printf("  scratch <output_file> [episodes]     - Train from zero\n");
        printf("  dataset <dataset> <output> [episodes] - Init from dataset\n");
        printf("  resume <checkpoint> <output> [episodes] - Resume training\n");
        return 1;
    }
    
    TrainingMode mode;
    const char *dataset_file = NULL;
    const char *checkpoint_file = NULL;
    const char *output_file = NULL;
    int max_episodes = 0;
    
    if (strcmp(argv[1], "scratch") == 0) {
        mode = MODE_FROM_SCRATCH;
        output_file = (argc > 2) ? argv[2] : "../../models/q learning/q_learning_scratch.txt";
        if (argc > 3) max_episodes = atoi(argv[3]);
    } else if (strcmp(argv[1], "dataset") == 0) {
        mode = MODE_DATASET_INIT;
        dataset_file = (argc > 2) ? argv[2] : "../../dataset/tic-tac-toe-minimax-non-terminal.data";
        output_file = (argc > 3) ? argv[3] : "../../models/q learning/q_learning_dataset.txt";
        if (argc > 4) max_episodes = atoi(argv[4]);
    } else if (strcmp(argv[1], "resume") == 0) {
        mode = MODE_RESUME;
        checkpoint_file = (argc > 2) ? argv[2] : "../../models/q learning/q_learning_o_dataset_continuous.txt";
        output_file = checkpoint_file;
        if (argc > 3) max_episodes = atoi(argv[3]);
    } else {
        printf("Unknown mode: %s\n", argv[1]);
        return 1;
    }
    
    train_continuous(mode, dataset_file, checkpoint_file, output_file, max_episodes);
    
    return 0;
}
