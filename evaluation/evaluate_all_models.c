// evaluate_all_models.c - Comprehensive evaluation of all AI models
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BOARD_SIZE 9
#define Q_TABLE_SIZE 20000

// Model types
typedef enum {
    MODEL_Q_LEARNING_SCRATCH,
    MODEL_Q_LEARNING_DATASET,
    MODEL_MINIMAX_EASY,
    MODEL_MINIMAX_HARD
} ModelType;

// Q-Learning structures
typedef struct QEntry {
    char board[9];
    int action;
    double q_value;
    int visits;
    struct QEntry *next;
} QEntry;

typedef struct {
    QEntry *table[Q_TABLE_SIZE];
    int total_entries;
} QLearningModel;

// Game functions
char check_winner(const char board[9]) {
    int wins[8][3] = {
        {0, 1, 2}, {3, 4, 5}, {6, 7, 8},
        {0, 3, 6}, {1, 4, 7}, {2, 5, 8},
        {0, 4, 8}, {2, 4, 6}
    };
    
    for (int i = 0; i < 8; i++) {
        if (board[wins[i][0]] == board[wins[i][1]] && 
            board[wins[i][1]] == board[wins[i][2]] && 
            board[wins[i][0]] != 'b') {
            return board[wins[i][0]];
        }
    }
    
    for (int i = 0; i < 9; i++) {
        if (board[i] == 'b') return ' ';
    }
    return 'd';
}

int get_valid_moves(const char board[9], int moves[9]) {
    int count = 0;
    for (int i = 0; i < 9; i++) {
        if (board[i] == 'b') moves[count++] = i;
    }
    return count;
}

// Q-Learning functions
unsigned long hash_board(const char board[9]) {
    unsigned long hash = 5381;
    for (int i = 0; i < 9; i++) {
        hash = ((hash << 5) + hash) + board[i];
    }
    return hash % Q_TABLE_SIZE;
}

double get_q_value(const QLearningModel *model, const char board[9], int action) {
    unsigned long hash = hash_board(board);
    QEntry *entry = model->table[hash];
    
    while (entry != NULL) {
        if (entry->action == action && memcmp(entry->board, board, 9) == 0) {
            return entry->q_value;
        }
        entry = entry->next;
    }
    return 0.0;
}

void add_q_entry(QLearningModel *model, char board[9], int action, double q_value, int visits) {
    unsigned long hash = hash_board(board);
    QEntry *new_entry = (QEntry *)malloc(sizeof(QEntry));
    memcpy(new_entry->board, board, 9);
    new_entry->action = action;
    new_entry->q_value = q_value;
    new_entry->visits = visits;
    new_entry->next = model->table[hash];
    model->table[hash] = new_entry;
    model->total_entries++;
}

int ql_load_model(const char *filename, QLearningModel *model) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return 0;
    
    for (int i = 0; i < Q_TABLE_SIZE; i++) model->table[i] = NULL;
    model->total_entries = 0;
    
    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == '#' || line[0] == '\n') continue;
        
        char board[9];
        int action, visits = 1;
        double q_value;
        
        char *token = strtok(line, ",");
        for (int i = 0; i < 9 && token != NULL; i++) {
            board[i] = token[0];
            token = strtok(NULL, ",");
        }
        
        if (token != NULL) {
            action = atoi(token);
            token = strtok(NULL, ",");
            if (token != NULL) {
                q_value = atof(token);
                token = strtok(NULL, ",\n");
                if (token != NULL) visits = atoi(token);
                add_q_entry(model, board, action, q_value, visits);
            }
        }
    }
    
    fclose(fp);
    return model->total_entries;
}

int ql_choose_move(const QLearningModel *model, const char board[9]) {
    int valid_moves[9], num = 0;
    for (int i = 0; i < 9; i++) {
        if (board[i] == 'b') valid_moves[num++] = i;
    }
    if (num == 0) return -1;
    
    int best_move = valid_moves[0];
    double best_q = get_q_value(model, board, best_move);
    
    for (int i = 1; i < num; i++) {
        double q = get_q_value(model, board, valid_moves[i]);
        if (q > best_q) {
            best_q = q;
            best_move = valid_moves[i];
        }
    }
    
    return best_move;
}

// Minimax implementation
int eval_board(const char board[9]) {
    char winner = check_winner(board);
    if (winner == 'o') return 10;
    if (winner == 'x') return -10;
    return 0;
}

int minimax(char board[9], int depth, int is_max, int max_depth) {
    int score = eval_board(board);
    if (score == 10) return score - depth;
    if (score == -10) return score + depth;
    char winner = check_winner(board);
    if (winner != ' ') return 0;
    if (max_depth > 0 && depth >= max_depth) return 0;
    
    if (is_max) {
        int best = -1000;
        for (int i = 0; i < 9; i++) {
            if (board[i] == 'b') {
                char save = board[i];
                board[i] = 'o';
                int val = minimax(board, depth + 1, 0, max_depth);
                board[i] = save;
                if (val > best) best = val;
            }
        }
        return best;
    } else {
        int best = 1000;
        for (int i = 0; i < 9; i++) {
            if (board[i] == 'b') {
                char save = board[i];
                board[i] = 'x';
                int val = minimax(board, depth + 1, 1, max_depth);
                board[i] = save;
                if (val < best) best = val;
            }
        }
        return best;
    }
}

int minimax_move(char board[9], int depth_limit) {
    int best_move = -1, best_val = -1000;
    
    for (int i = 0; i < 9; i++) {
        if (board[i] == 'b') {
            char save = board[i];
            board[i] = 'o';
            int val = minimax(board, 0, 0, depth_limit);
            board[i] = save;
            
            if (val > best_val) {
                best_val = val;
                best_move = i;
            }
        }
    }
    
    return best_move;
}

// Get move from any model
int get_model_move(void *model, ModelType type, char board[9]) {
    switch (type) {
        case MODEL_Q_LEARNING_SCRATCH:
        case MODEL_Q_LEARNING_DATASET:
            return ql_choose_move((QLearningModel*)model, board);
        case MODEL_MINIMAX_EASY:
            return minimax_move(board, 4);  // Depth 4
        case MODEL_MINIMAX_HARD:
            return minimax_move(board, 0);  // Full depth
        default:
            return -1;
    }
}

// Play a game
int play_game(void *o_model, ModelType o_type, void *x_model, ModelType x_type, int verbose) {
    char board[9];
    for (int i = 0; i < 9; i++) board[i] = 'b';
    
    char player = 'x';
    
    while (1) {
        int move = -1;
        
        if (player == 'x') {
            move = get_model_move(x_model, x_type, board);
        } else {
            move = get_model_move(o_model, o_type, board);
        }
        
        if (move == -1 || board[move] != 'b') {
            if (verbose) printf("Invalid move by %c!\n", player);
            return player == 'x' ? 1 : -1;
        }
        
        board[move] = player;
        
        char winner = check_winner(board);
        if (winner == 'x') return -1;
        if (winner == 'o') return 1;
        if (winner == 'd') return 0;
        
        player = (player == 'x') ? 'o' : 'x';
    }
}

// Evaluate model performance
void evaluate_model(const char *name, void *model, ModelType type, int num_games) {
    printf("\n========================================\n");
    printf("EVALUATING: %s\n", name);
    printf("========================================\n");
    
    // vs Random
    int rand_wins = 0, rand_draws = 0, rand_losses = 0;
    for (int i = 0; i < num_games; i++) {
        int result = play_game(model, type, NULL, MODEL_MINIMAX_EASY, 0);
        // Simulate random by using weak minimax
        char board[9];
        for (int j = 0; j < 9; j++) board[j] = 'b';
        
        char player = 'x';
        while (1) {
            int move;
            if (player == 'x') {
                // Random-like move
                int valid[9], num = 0;
                for (int k = 0; k < 9; k++) if (board[k] == 'b') valid[num++] = k;
                if (num == 0) break;
                move = valid[rand() % num];
            } else {
                move = get_model_move(model, type, board);
            }
            
            if (move == -1 || board[move] != 'b') break;
            board[move] = player;
            
            char winner = check_winner(board);
            if (winner == 'x') { rand_losses++; break; }
            if (winner == 'o') { rand_wins++; break; }
            if (winner == 'd') { rand_draws++; break; }
            
            player = (player == 'x') ? 'o' : 'x';
        }
    }
    
    // vs Minimax Easy
    int easy_wins = 0, easy_draws = 0, easy_losses = 0;
    for (int i = 0; i < num_games; i++) {
        int result = play_game(model, type, NULL, MODEL_MINIMAX_EASY, 0);
        if (result == 1) easy_wins++;
        else if (result == -1) easy_losses++;
        else easy_draws++;
    }
    
    // vs Perfect Minimax
    int hard_wins = 0, hard_draws = 0, hard_losses = 0;
    for (int i = 0; i < num_games; i++) {
        int result = play_game(model, type, NULL, MODEL_MINIMAX_HARD, 0);
        if (result == 1) hard_wins++;
        else if (result == -1) hard_losses++;
        else hard_draws++;
    }
    
    printf("\nPerformance Summary:\n");
    printf("  vs Random:        W:%3d (%.1f%%) D:%3d (%.1f%%) L:%3d (%.1f%%)\n",
           rand_wins, rand_wins*100.0/num_games, rand_draws, rand_draws*100.0/num_games,
           rand_losses, rand_losses*100.0/num_games);
    printf("  vs Minimax Easy:  W:%3d (%.1f%%) D:%3d (%.1f%%) L:%3d (%.1f%%)\n",
           easy_wins, easy_wins*100.0/num_games, easy_draws, easy_draws*100.0/num_games,
           easy_losses, easy_losses*100.0/num_games);
    printf("  vs Minimax Hard:  W:%3d (%.1f%%) D:%3d (%.1f%%) L:%3d (%.1f%%)\n",
           hard_wins, hard_wins*100.0/num_games, hard_draws, hard_draws*100.0/num_games,
           hard_losses, hard_losses*100.0/num_games);
    
    // Calculate score
    int total_score = rand_wins * 3 + rand_draws * 1 + 
                      easy_wins * 5 + easy_draws * 3 +
                      hard_wins * 10 + hard_draws * 5;
    
    printf("\n  Quality Score: %d (higher is better)\n", total_score);
}

int main() {
    srand(time(NULL));
    
    printf("========================================\n");
    printf("COMPREHENSIVE MODEL EVALUATION\n");
    printf("========================================\n");
    printf("Testing all available models...\n");
    
    QLearningModel ql_scratch, ql_dataset;
    
    // Load models
    printf("\nLoading Q-Learning (From-Scratch)...\n");
    int scratch_entries = ql_load_model("../models/q learning/q_learning_from_scratch.txt", &ql_scratch);
    printf("  Loaded %d entries (4KB file)\n", scratch_entries);
    
    printf("\nLoading Q-Learning (Dataset-Init)...\n");
    int dataset_entries = ql_load_model("../models/q learning/q_learning_non_terminal.txt", &ql_dataset);
    printf("  Loaded %d entries (527KB file)\n", dataset_entries);
    
    int num_games = 100;
    
    // Evaluate each model
    if (scratch_entries > 0) {
        evaluate_model("Q-Learning (From-Scratch, 10K episodes)", &ql_scratch, MODEL_Q_LEARNING_SCRATCH, num_games);
    }
    
    if (dataset_entries > 0) {
        evaluate_model("Q-Learning (Dataset-Init, 50K episodes)", &ql_dataset, MODEL_Q_LEARNING_DATASET, num_games);
    }
    
    evaluate_model("Minimax Easy (Depth 4)", NULL, MODEL_MINIMAX_EASY, num_games);
    evaluate_model("Minimax Perfect (Full Depth)", NULL, MODEL_MINIMAX_HARD, num_games);
    
    // Final comparison
    printf("\n========================================\n");
    printf("RECOMMENDATION\n");
    printf("========================================\n");
    printf("\nFor best move prediction:\n");
    printf("  1. Minimax Perfect - Always optimal, but predictable\n");
    printf("  2. Check which Q-Learning has highest quality score\n");
    printf("  3. Minimax Easy - Good balance of challenge\n");
    printf("\nFor Medium difficulty, use the Q-Learning model with:\n");
    printf("  - Highest draw rate vs Minimax Easy\n");
    printf("  - Good win rate vs Random\n");
    printf("  - Best overall quality score\n");
    
    return 0;
}
