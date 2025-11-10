// evaluate_against_minimax.c
// Tests all trained models against perfect Minimax play
// Generates a comprehensive comparison report

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_GAMES 100
#define NUM_FEATURES 10
#define MAX_FEATURE_LENGTH 32
#define MAX_FEATURES_NB 9
#define MAX_STATES 10
#define MAX_LABELS 10

// =====================================================
// NAIVE BAYES STRUCTURES
// =====================================================
typedef struct {
    char state[MAX_FEATURE_LENGTH];
    char label[MAX_FEATURE_LENGTH];
    double probability;
} FeatureProbability;

typedef struct {
    char label[MAX_FEATURE_LENGTH];
    double probability;
} LabelProbability;

typedef struct {
    FeatureProbability feature_probs[MAX_FEATURES_NB][MAX_STATES * MAX_LABELS];
    int feature_count[MAX_FEATURES_NB];
    LabelProbability label_probs[MAX_LABELS];
    int label_count;
} NaiveBayesModel;

// =====================================================
// LINEAR REGRESSION STRUCTURES
// =====================================================
typedef struct {
    double weights[NUM_FEATURES];
} LinearModel;

// =====================================================
// Q-LEARNING STRUCTURES
// =====================================================
#define Q_TABLE_SIZE 20000

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
} QTable;

// =====================================================
// GAME STATE TRACKING
// =====================================================
typedef struct {
    int wins;
    int losses;
    int draws;
    int total_games;
    double total_time_ms;
    int illegal_moves;
} ModelStats;

// =====================================================
// MINIMAX IMPLEMENTATION
// =====================================================
int winBy(char b[9], char p) {
    if (b[0] == p && b[1] == p && b[2] == p) return 1;
    if (b[3] == p && b[4] == p && b[5] == p) return 1;
    if (b[6] == p && b[7] == p && b[8] == p) return 1;
    if (b[0] == p && b[3] == p && b[6] == p) return 1;
    if (b[1] == p && b[4] == p && b[7] == p) return 1;
    if (b[2] == p && b[5] == p && b[8] == p) return 1;
    if (b[0] == p && b[4] == p && b[8] == p) return 1;
    if (b[2] == p && b[4] == p && b[6] == p) return 1;
    return 0;
}

static int has_space(const char b[9]) {
    for (int i = 0; i < 9; i++) {
        if (b[i] != 'X' && b[i] != 'O') return 1;
    }
    return 0;
}

static int eval(const char b[9]) {
    if (winBy((char*)b, 'O')) return 10;
    if (winBy((char*)b, 'X')) return -10;
    return 0;
}

static int minimax(char b[9], int isMax, int depth) {
    int score = eval(b);
    
    if (score == 10) return score - depth;
    if (score == -10) return score + depth;
    if (!has_space(b)) return 0;
    
    if (isMax) {
        int best = -1000;
        for (int i = 0; i < 9; i++) {
            if (b[i] != 'X' && b[i] != 'O') {
                char save = b[i];
                b[i] = 'O';
                int value = minimax(b, 0, depth + 1);
                b[i] = save;
                if (value > best) best = value;
            }
        }
        return best;
    } else {
        int best = 1000;
        for (int i = 0; i < 9; i++) {
            if (b[i] != 'X' && b[i] != 'O') {
                char save = b[i];
                b[i] = 'X';
                int value = minimax(b, 1, depth + 1);
                b[i] = save;
                if (value < best) best = value;
            }
        }
        return best;
    }
}

int findBestMove(char b[9], char player) {
    int best = (player == 'O') ? -1000 : 1000;
    int move = -1;
    
    for (int i = 0; i < 9; i++) {
        if (b[i] != 'X' && b[i] != 'O') {
            char save = b[i];
            b[i] = player;
            int value = minimax(b, (player == 'X') ? 0 : 1, 0);
            b[i] = save;
            
            if (player == 'O') {
                if (value > best) {
                    best = value;
                    move = i;
                }
            } else {
                if (value < best) {
                    best = value;
                    move = i;
                }
            }
        }
    }
    return move;
}

// =====================================================
// NAIVE BAYES LOADING & PREDICTION
// =====================================================
int nb_load_model(const char *filename, NaiveBayesModel *model) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return 0;
    
    char line[256];
    model->label_count = 0;
    for (int i = 0; i < MAX_FEATURES_NB; i++) {
        model->feature_count[i] = 0;
    }
    
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, "Total labels:") != NULL) {
            sscanf(line, "Total labels: %d", &model->label_count);
            fgets(line, sizeof(line), fp);
            break;
        }
    }
    
    for (int i = 0; i < model->label_count; i++) {
        if (fgets(line, sizeof(line), fp)) {
            sscanf(line, "Label: %s P(Label) = %lf", 
                   model->label_probs[i].label, 
                   &model->label_probs[i].probability);
        }
    }
    
    int current_feature = -1;
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, "Feature") != NULL && strstr(line, ":") != NULL) {
            int count;
            sscanf(line, "Feature %d: (%d unique", &current_feature, &count);
            model->feature_count[current_feature] = 0;
        } else if (strstr(line, "State=") != NULL && current_feature >= 0) {
            int idx = model->feature_count[current_feature];
            sscanf(line, "  State=%s | Label=%s | P(State|Label) = %lf",
                   model->feature_probs[current_feature][idx].state,
                   model->feature_probs[current_feature][idx].label,
                   &model->feature_probs[current_feature][idx].probability);
            model->feature_count[current_feature]++;
        }
    }
    
    fclose(fp);
    return 1;
}

void nb_predict(const NaiveBayesModel *model, char features[MAX_FEATURES_NB][MAX_FEATURE_LENGTH], 
                char *best_label, double *best_prob) {
    double max_prob = -1.0;
    strcpy(best_label, "draw");
    
    for (int i = 0; i < model->label_count; i++) {
        double prob = model->label_probs[i].probability;
        
        for (int j = 0; j < MAX_FEATURES_NB; j++) {
            int found = 0;
            for (int k = 0; k < model->feature_count[j]; k++) {
                if (strcmp(model->feature_probs[j][k].state, features[j]) == 0 &&
                    strcmp(model->feature_probs[j][k].label, model->label_probs[i].label) == 0) {
                    prob *= model->feature_probs[j][k].probability;
                    found = 1;
                    break;
                }
            }
            if (!found) prob *= 0.001;
        }
        
        if (prob > max_prob) {
            max_prob = prob;
            strcpy(best_label, model->label_probs[i].label);
        }
    }
    
    *best_prob = max_prob;
}

static void board_to_features(const char board[9], char features[MAX_FEATURES_NB][MAX_FEATURE_LENGTH]) {
    for (int i = 0; i < 9; i++) {
        if (board[i] == 'X') strcpy(features[i], "x");
        else if (board[i] == 'O') strcpy(features[i], "o");
        else strcpy(features[i], "b");
    }
}

int nb_find_best_move(const NaiveBayesModel *model, char board[9]) {
    int empty_cells[9];
    int empty_count = 0;
    
    for (int i = 0; i < 9; i++) {
        if (board[i] != 'X' && board[i] != 'O') {
            empty_cells[empty_count++] = i;
        }
    }
    
    if (empty_count == 0) return -1;
    
    int best_move = empty_cells[0];
    double best_score = -1.0;
    
    for (int i = 0; i < empty_count; i++) {
        int move = empty_cells[i];
        char temp_board[9];
        memcpy(temp_board, board, 9);
        temp_board[move] = 'O';
        
        char features[MAX_FEATURES_NB][MAX_FEATURE_LENGTH];
        board_to_features(temp_board, features);
        
        char predicted_label[MAX_FEATURE_LENGTH];
        double prob;
        nb_predict(model, features, predicted_label, &prob);
        
        double score = 0.0;
        if (strcmp(predicted_label, "win") == 0) score = prob;
        else if (strcmp(predicted_label, "draw") == 0) score = prob * 0.5;
        else score = prob * 0.1;
        
        if (score > best_score) {
            best_score = score;
            best_move = move;
        }
    }
    
    return best_move;
}

// =====================================================
// LINEAR REGRESSION LOADING & PREDICTION
// =====================================================
int lr_load_model(const char *filename, LinearModel *model) {
    FILE *file = fopen(filename, "r");
    if (!file) return 0;
    
    char line[256];
    int weight_idx = 0;
    
    while (fgets(line, sizeof(line), file) && weight_idx < NUM_FEATURES) {
        if (strstr(line, "Weight") != NULL) {
            double weight;
            if (sscanf(line, "Weight[%*d] = %lf", &weight) == 1) {
                model->weights[weight_idx++] = weight;
            }
        }
    }
    
    fclose(file);
    return (weight_idx == NUM_FEATURES);
}

double lr_predict(const LinearModel *model, const double *features) {
    double result = 0.0;
    for (int i = 0; i < NUM_FEATURES; i++) {
        result += model->weights[i] * features[i];
    }
    return result;
}

void encode_features(const char *board, double *features) {
    features[0] = 1.0;  // Bias
    for (int i = 0; i < 9; i++) {
        if (board[i] == 'X') features[i + 1] = 1.0;
        else if (board[i] == 'O') features[i + 1] = -1.0;
        else features[i + 1] = 0.0;
    }
}

int lr_find_best_move(const LinearModel *model, char board[9]) {
    int empty_cells[9];
    int empty_count = 0;
    
    for (int i = 0; i < 9; i++) {
        if (board[i] != 'X' && board[i] != 'O') {
            empty_cells[empty_count++] = i;
        }
    }
    
    if (empty_count == 0) return -1;
    
    int best_move = empty_cells[0];
    double best_score = -1000.0;
    
    for (int i = 0; i < empty_count; i++) {
        int move = empty_cells[i];
        char temp_board[9];
        memcpy(temp_board, board, 9);
        temp_board[move] = 'O';
        
        double features[NUM_FEATURES];
        encode_features(temp_board, features);
        double score = lr_predict(model, features);
        
        if (score > best_score) {
            best_score = score;
            best_move = move;
        }
    }
    
    return best_move;
}

// =====================================================
// Q-LEARNING LOADING & PREDICTION
// =====================================================
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

unsigned long hash_board_q(char board[9]) {
    unsigned long hash = 5381;
    for (int i = 0; i < 9; i++) {
        hash = ((hash << 5) + hash) + board[i];
    }
    return hash % Q_TABLE_SIZE;
}

double get_q_value(QTable *qt, char board[9], int action) {
    unsigned long hash = hash_board_q(board);
    QEntry *entry = qt->table[hash];
    
    while (entry != NULL) {
        if (entry->action == action && 
            memcmp(entry->board, board, 9) == 0) {
            return entry->q_value;
        }
        entry = entry->next;
    }
    
    return 0.0;
}

void add_q_entry(QTable *qt, char board[9], int action, double q_value, int visits) {
    unsigned long hash = hash_board_q(board);
    
    QEntry *new_entry = (QEntry *)malloc(sizeof(QEntry));
    memcpy(new_entry->board, board, 9);
    new_entry->action = action;
    new_entry->q_value = q_value;
    new_entry->visits = visits;
    new_entry->next = qt->table[hash];
    qt->table[hash] = new_entry;
    qt->total_entries++;
}

int q_load_model(const char *filename, QTable *qt) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return 0;
    
    init_qtable(qt);
    
    char line[256];
    // Skip header lines
    while (fgets(line, sizeof(line), fp)) {
        if (line[0] != '#') break;
    }
    
    // Read Q-table entries
    do {
        if (line[0] == '#' || line[0] == '\n') continue;
        
        char board[9];
        int action, visits;
        double q_value;
        
        // Parse: b,o,o,x,b,x,o,x,b,8,0.800000,1
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
                token = strtok(NULL, ",");
                if (token != NULL) {
                    visits = atoi(token);
                    add_q_entry(qt, board, action, q_value, visits);
                }
            }
        }
    } while (fgets(line, sizeof(line), fp));
    
    fclose(fp);
    return (qt->total_entries > 0);
}

int q_find_best_move(const QTable *qt, char board[9]) {
    int empty_cells[9];
    int empty_count = 0;
    
    for (int i = 0; i < 9; i++) {
        if (board[i] != 'X' && board[i] != 'O') {
            empty_cells[empty_count++] = i;
        }
    }
    
    if (empty_count == 0) return -1;
    
    // Convert board to Q-learning format (lowercase)
    char q_board[9];
    for (int i = 0; i < 9; i++) {
        if (board[i] == 'X') q_board[i] = 'x';
        else if (board[i] == 'O') q_board[i] = 'o';
        else q_board[i] = 'b';
    }
    
    int best_move = empty_cells[0];
    double best_q = -1000.0;
    
    for (int i = 0; i < empty_count; i++) {
        int move = empty_cells[i];
        double q_val = get_q_value(qt, q_board, move);
        
        if (q_val > best_q) {
            best_q = q_val;
            best_move = move;
        }
    }
    
    return best_move;
}

// =====================================================
// GAME SIMULATION
// =====================================================
void init_board(char board[9]) {
    for (int i = 0; i < 9; i++) {
        board[i] = '0' + i;
    }
}

void print_board(const char board[9]) {
    printf("\n");
    for (int i = 0; i < 9; i += 3) {
        printf(" %c | %c | %c \n", board[i], board[i+1], board[i+2]);
        if (i < 6) printf("---+---+---\n");
    }
    printf("\n");
}

int check_winner(const char board[9]) {
    if (winBy((char*)board, 'X')) return -1;  // X wins (AI loses)
    if (winBy((char*)board, 'O')) return 1;   // O wins (AI wins)
    if (!has_space(board)) return 0;          // Draw
    return 2;  // Game continues
}

// Play a game: AI as O, Minimax as X
int play_game(void *model, int model_type, int verbose, double *time_taken) {
    char board[9];
    init_board(board);
    
    clock_t start = clock();
    int move_count = 0;
    
    // X (Minimax) goes first
    while (1) {
        // X's turn (Minimax - always perfect)
        int x_move = findBestMove(board, 'X');
        if (x_move != -1) {
            board[x_move] = 'X';
            move_count++;
            
            if (verbose) {
                printf("X (Minimax) plays position %d:\n", x_move);
                print_board(board);
            }
        }
        
        int result = check_winner(board);
        if (result != 2) {
            clock_t end = clock();
            *time_taken = ((double)(end - start)) / CLOCKS_PER_SEC * 1000.0;
            return result;
        }
        
        // O's turn (AI model)
        int o_move = -1;
        if (model_type == 0) {  // Naive Bayes
            o_move = nb_find_best_move((NaiveBayesModel*)model, board);
        } else if (model_type == 1) {  // Linear Regression
            o_move = lr_find_best_move((LinearModel*)model, board);
        } else if (model_type == 2) {  // Q-Learning
            o_move = q_find_best_move((QTable*)model, board);
        }
        
        if (o_move == -1 || board[o_move] == 'X' || board[o_move] == 'O') {
            clock_t end = clock();
            *time_taken = ((double)(end - start)) / CLOCKS_PER_SEC * 1000.0;
            if (verbose) printf("AI made illegal move!\n");
            return -2;  // Illegal move = loss
        }
        
        board[o_move] = 'O';
        move_count++;
        
        if (verbose) {
            printf("O (AI) plays position %d:\n", o_move);
            print_board(board);
        }
        
        result = check_winner(board);
        if (result != 2) {
            clock_t end = clock();
            *time_taken = ((double)(end - start)) / CLOCKS_PER_SEC * 1000.0;
            return result;
        }
    }
}

// =====================================================
// EVALUATION FUNCTIONS
// =====================================================
void evaluate_model(void *model, int model_type, const char *model_name, 
                   int num_games, ModelStats *stats) {
    printf("\n========================================\n");
    printf("Evaluating: %s\n", model_name);
    printf("========================================\n");
    
    stats->wins = 0;
    stats->losses = 0;
    stats->draws = 0;
    stats->total_games = num_games;
    stats->total_time_ms = 0.0;
    stats->illegal_moves = 0;
    
    for (int i = 0; i < num_games; i++) {
        double game_time;
        int result = play_game(model, model_type, 0, &game_time);
        
        stats->total_time_ms += game_time;
        
        if (result == 1) {
            stats->wins++;
        } else if (result == -1 || result == -2) {
            stats->losses++;
            if (result == -2) stats->illegal_moves++;
        } else {
            stats->draws++;
        }
        
        if ((i + 1) % 10 == 0) {
            printf("Completed %d/%d games...\r", i + 1, num_games);
            fflush(stdout);
        }
    }
    
    printf("\nEvaluation complete!\n");
    printf("Wins: %d (%.1f%%)\n", stats->wins, (stats->wins * 100.0) / num_games);
    printf("Draws: %d (%.1f%%)\n", stats->draws, (stats->draws * 100.0) / num_games);
    printf("Losses: %d (%.1f%%)\n", stats->losses, (stats->losses * 100.0) / num_games);
    printf("Illegal moves: %d\n", stats->illegal_moves);
    printf("Avg time per game: %.2f ms\n", stats->total_time_ms / num_games);
}

void print_comparison_report(const char *filename, ModelStats stats[], const char *names[], int count) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        printf("Error: Could not create report file\n");
        return;
    }
    
    fprintf(fp, "========================================\n");
    fprintf(fp, "MODEL EVALUATION AGAINST MINIMAX\n");
    fprintf(fp, "========================================\n");
    fprintf(fp, "Generated: %s\n", __DATE__);
    fprintf(fp, "Games per model: %d\n", MAX_GAMES);
    fprintf(fp, "\n");
    
    fprintf(fp, "GOLD STANDARD: Perfect Minimax Play\n");
    fprintf(fp, "- Against perfect play, the best an AI can do is DRAW\n");
    fprintf(fp, "- Any WIN means the AI exploited a flaw (shouldn't happen vs Minimax)\n");
    fprintf(fp, "- LOSSES indicate the AI made suboptimal moves\n");
    fprintf(fp, "\n");
    
    fprintf(fp, "========================================\n");
    fprintf(fp, "DETAILED RESULTS\n");
    fprintf(fp, "========================================\n\n");
    
    for (int i = 0; i < count; i++) {
        fprintf(fp, "Model: %s\n", names[i]);
        fprintf(fp, "  Wins:          %3d (%.1f%%)\n", stats[i].wins, 
                (stats[i].wins * 100.0) / stats[i].total_games);
        fprintf(fp, "  Draws:         %3d (%.1f%%)\n", stats[i].draws,
                (stats[i].draws * 100.0) / stats[i].total_games);
        fprintf(fp, "  Losses:        %3d (%.1f%%)\n", stats[i].losses,
                (stats[i].losses * 100.0) / stats[i].total_games);
        fprintf(fp, "  Illegal Moves: %3d\n", stats[i].illegal_moves);
        fprintf(fp, "  Avg Time:      %.2f ms/game\n", stats[i].total_time_ms / stats[i].total_games);
        fprintf(fp, "\n");
    }
    
    fprintf(fp, "========================================\n");
    fprintf(fp, "RANKING BY PERFORMANCE\n");
    fprintf(fp, "========================================\n\n");
    
    // Calculate score: draws are best (100pts), losses are bad (0pts), wins shouldn't happen (50pts)
    typedef struct {
        int index;
        double score;
    } ModelScore;
    
    ModelScore scores[10];
    for (int i = 0; i < count; i++) {
        scores[i].index = i;
        scores[i].score = (stats[i].draws * 100.0 + stats[i].wins * 50.0) / stats[i].total_games;
        scores[i].score -= stats[i].illegal_moves * 10.0;  // Penalty for illegal moves
    }
    
    // Simple bubble sort
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (scores[j].score < scores[j + 1].score) {
                ModelScore temp = scores[j];
                scores[j] = scores[j + 1];
                scores[j + 1] = temp;
            }
        }
    }
    
    fprintf(fp, "Rank | Model                                  | Score\n");
    fprintf(fp, "-----+----------------------------------------+-------\n");
    for (int i = 0; i < count; i++) {
        int idx = scores[i].index;
        fprintf(fp, " %2d  | %-38s | %.1f\n", i + 1, names[idx], scores[i].score);
    }
    
    fprintf(fp, "\n========================================\n");
    fprintf(fp, "RECOMMENDATION\n");
    fprintf(fp, "========================================\n\n");
    
    int best_idx = scores[0].index;
    fprintf(fp, "Best performing model: %s\n", names[best_idx]);
    fprintf(fp, "Draw rate: %.1f%%\n", (stats[best_idx].draws * 100.0) / stats[best_idx].total_games);
    fprintf(fp, "Loss rate: %.1f%%\n", (stats[best_idx].losses * 100.0) / stats[best_idx].total_games);
    fprintf(fp, "\nThis model should be used in your game for the best AI performance.\n");
    
    fclose(fp);
    printf("\nReport saved to: %s\n", filename);
}

// =====================================================
// MAIN
// =====================================================
int main() {
    srand(time(NULL));
    
    printf("========================================\n");
    printf("TIC-TAC-TOE MODEL EVALUATION\n");
    printf("========================================\n");
    printf("Testing all models against perfect Minimax play\n");
    printf("Games per model: %d\n\n", MAX_GAMES);
    
    // Arrays to store all models and their stats
    ModelStats all_stats[10];
    const char *model_names[10];
    int model_count = 0;
    
    // Test Naive Bayes - Non-Terminal
    NaiveBayesModel nb_non_term;
    if (nb_load_model("../models/naive_bayes_non_terminal/model_non_terminal.txt", &nb_non_term)) {
        evaluate_model(&nb_non_term, 0, "Naive Bayes (Non-Terminal)", MAX_GAMES, &all_stats[model_count]);
        model_names[model_count] = "Naive Bayes (Non-Terminal)";
        model_count++;
    } else {
        printf("Warning: Could not load Naive Bayes (Non-Terminal) model\n");
    }
    
    // Test Naive Bayes - Combined
    NaiveBayesModel nb_combined;
    if (nb_load_model("../models/naive_bayes_combined/model_combined.txt", &nb_combined)) {
        evaluate_model(&nb_combined, 0, "Naive Bayes (Combined)", MAX_GAMES, &all_stats[model_count]);
        model_names[model_count] = "Naive Bayes (Combined)";
        model_count++;
    } else {
        printf("Warning: Could not load Naive Bayes (Combined) model\n");
    }
    
    // Test Linear Regression - Non-Terminal
    LinearModel lr_non_term;
    if (lr_load_model("../models/linear_regression_non_terminal/model_non_terminal.txt", &lr_non_term)) {
        evaluate_model(&lr_non_term, 1, "Linear Regression (Non-Terminal)", MAX_GAMES, &all_stats[model_count]);
        model_names[model_count] = "Linear Regression (Non-Terminal)";
        model_count++;
    } else {
        printf("Warning: Could not load Linear Regression (Non-Terminal) model\n");
    }
    
    // Test Linear Regression - Combined
    LinearModel lr_combined;
    if (lr_load_model("../models/linear_regression_combined/model_combined.txt", &lr_combined)) {
        evaluate_model(&lr_combined, 1, "Linear Regression (Combined)", MAX_GAMES, &all_stats[model_count]);
        model_names[model_count] = "Linear Regression (Combined)";
        model_count++;
    } else {
        printf("Warning: Could not load Linear Regression (Combined) model\n");
    }
    
    // Test Q-Learning - Non-Terminal
    QTable q_non_term;
    if (q_load_model("../models/q learning/q_learning_non_terminal.txt", &q_non_term)) {
        evaluate_model(&q_non_term, 2, "Q-Learning (Non-Terminal)", MAX_GAMES, &all_stats[model_count]);
        model_names[model_count] = "Q-Learning (Non-Terminal)";
        model_count++;
    } else {
        printf("Warning: Could not load Q-Learning (Non-Terminal) model\n");
    }
    
    // Test Q-Learning - Combined
    QTable q_combined;
    if (q_load_model("../models/q learning/q_learning_combined.txt", &q_combined)) {
        evaluate_model(&q_combined, 2, "Q-Learning (Combined)", MAX_GAMES, &all_stats[model_count]);
        model_names[model_count] = "Q-Learning (Combined)";
        model_count++;
    } else {
        printf("Warning: Could not load Q-Learning (Combined) model\n");
    }
    
    // Generate comparison report
    if (model_count > 0) {
        print_comparison_report("model_evaluation_report.txt", all_stats, model_names, model_count);
        
        printf("\n========================================\n");
        printf("EVALUATION COMPLETE\n");
        printf("========================================\n");
        printf("Tested %d models\n", model_count);
        printf("Full report saved to: model_evaluation_report.txt\n");
    } else {
        printf("\nNo models were successfully loaded!\n");
        printf("Make sure you have trained models in the ../models/ directory\n");
        return 1;
    }
    
    return 0;
}
