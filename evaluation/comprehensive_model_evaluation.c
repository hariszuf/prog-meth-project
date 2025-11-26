#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

// =====================================================
// MODEL CONFIGURATION (matching TTTGUI framework)
// =====================================================
typedef enum {
    AI_MODEL_NAIVE_BAYES,
    AI_MODEL_LINEAR_REGRESSION,
    AI_MODEL_Q_LEARNING,
    AI_MODEL_MINIMAX_EASY,
    AI_MODEL_MINIMAX_HARD
} AIModelType;

// =====================================================
// NAIVE BAYES STRUCTURES
// =====================================================
#define MAX_FEATURES_NB 9
#define MAX_STATES 10
#define MAX_LABELS 10
#define MAX_FEATURE_LENGTH 32

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
#define NUM_FEATURES 10

typedef struct {
    double weights[NUM_FEATURES];
} LinearRegressionModel;

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
} QLearningModel;

// =====================================================
// CONFUSION MATRIX
// =====================================================
typedef struct {
    int matrix[3][3];  // [actual][predicted] - 0=Win, 1=Loss, 2=Draw
    int total;
    double accuracy;
} ConfusionMatrix;

// =====================================================
// MOVE EVALUATION STATS
// =====================================================
typedef struct {
    int total_positions;
    int moves_agree_with_minimax;
    int optimal_moves;
    int suboptimal_moves;
    int blunders;  // Moves that lose winning positions
    
    // Position-specific breakdown
    int opening_correct;   // First 3 moves
    int midgame_correct;   // Moves 4-6
    int endgame_correct;   // Moves 7-9
    
    int opening_total;
    int midgame_total;
    int endgame_total;
} MoveEvalStats;

// =====================================================
// GAME FUNCTIONS
// =====================================================
int winBy(const char b[9], char p) {
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

int has_space(const char b[9]) {
    for (int i = 0; i < 9; i++) {
        if (b[i] != 'X' && b[i] != 'O') return 1;
    }
    return 0;
}

int count_moves(const char b[9]) {
    int count = 0;
    for (int i = 0; i < 9; i++) {
        if (b[i] == 'X' || b[i] == 'O') count++;
    }
    return count;
}

// =====================================================
// MINIMAX ORACLE (Ground Truth)
// =====================================================
int eval_board(const char b[9]) {
    if (winBy(b, 'O')) return 10;
    if (winBy(b, 'X')) return -10;
    return 0;
}

int minimax(char b[9], int isMax, int depth) {
    int score = eval_board(b);
    
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

int minimax_best_move(char b[9]) {
    int best_move = -1;
    int best_val = -1000;
    
    for (int i = 0; i < 9; i++) {
        if (b[i] != 'X' && b[i] != 'O') {
            char save = b[i];
            b[i] = 'O';
            int val = minimax(b, 0, 0);
            b[i] = save;
            
            if (val > best_val) {
                best_val = val;
                best_move = i;
            }
        }
    }
    
    return best_move;
}

// Classify position as Win/Loss/Draw (for confusion matrix)
int minimax_classify_position(char b[9]) {
    // Make optimal move and check eventual outcome
    char temp[9];
    memcpy(temp, b, 9);
    
    // Simulate optimal play to completion
    char player = 'O';  // O's turn (AI)
    
    while (has_space(temp) && eval_board(temp) == 0) {
        int move = (player == 'O') ? minimax_best_move(temp) : minimax_best_move(temp);
        if (move == -1) break;
        
        temp[move] = player;
        player = (player == 'O') ? 'X' : 'O';
    }
    
    // Classify outcome
    if (winBy(temp, 'O')) return 0;      // Win
    if (winBy(temp, 'X')) return 1;      // Loss
    return 2;                             // Draw
}

// =====================================================
// MODEL LOADING (Naive Bayes)
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

void board_to_features(const char board[9], char features[MAX_FEATURES_NB][MAX_FEATURE_LENGTH]) {
    for (int i = 0; i < 9; i++) {
        if (board[i] == 'X') strcpy(features[i], "x");
        else if (board[i] == 'O') strcpy(features[i], "o");
        else strcpy(features[i], "b");
    }
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

int nb_classify(const NaiveBayesModel *model, char board[9]) {
    char features[MAX_FEATURES_NB][MAX_FEATURE_LENGTH];
    board_to_features(board, features);
    
    char label[MAX_FEATURE_LENGTH];
    double prob;
    nb_predict(model, features, label, &prob);
    
    if (strcmp(label, "win") == 0) return 0;
    if (strcmp(label, "lose") == 0) return 1;  // Fixed: model uses "lose" not "loss"
    return 2;  // draw
}

// =====================================================
// MODEL LOADING (Linear Regression)
// =====================================================
int lr_load_model(const char *filename, LinearRegressionModel *model) {
    FILE *file = fopen(filename, "r");
    if (!file) return 0;
    
    char line[256];
    int weight_idx = 0;
    
    while (fgets(line, sizeof(line), file) && weight_idx < NUM_FEATURES) {
        if (strstr(line, "Weight") != NULL) {
            double weight;
            // Try format 1: Weight[0] = 0.123
            if (sscanf(line, "Weight[%*d] = %lf", &weight) == 1) {
                model->weights[weight_idx++] = weight;
            }
            // Try format 2: Weight[0] (bias): 0.123
            else if (sscanf(line, "Weight[%*d]%*[^:]: %lf", &weight) == 1) {
                model->weights[weight_idx++] = weight;
            }
        }
    }
    
    fclose(file);
    return (weight_idx == NUM_FEATURES);
}

void encode_features(const char *board, double *features) {
    features[0] = 1.0;  // Bias
    for (int i = 0; i < 9; i++) {
        // Fixed: Swap encoding to match model's training expectations
        // Model has negative weights, so O (AI) should be positive
        if (board[i] == 'X') features[i + 1] = -1.0;  // Opponent
        else if (board[i] == 'O') features[i + 1] = 1.0;  // AI
        else features[i + 1] = 0.0;
    }
}

double lr_predict(const LinearRegressionModel *model, const double *features) {
    double result = 0.0;
    for (int i = 0; i < NUM_FEATURES; i++) {
        result += model->weights[i] * features[i];
    }
    return result;
}

int lr_find_best_move(const LinearRegressionModel *model, char board[9]) {
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

int lr_classify(const LinearRegressionModel *model, char board[9]) {
    double features[NUM_FEATURES];
    encode_features(board, features);
    double score = lr_predict(model, features);
    
    if (score > 0.5) return 0;      // Win
    if (score < -0.5) return 1;     // Loss
    return 2;                        // Draw
}

// =====================================================
// MODEL LOADING (Q-Learning)
// =====================================================
void init_qtable(QLearningModel *model) {
    for (int i = 0; i < Q_TABLE_SIZE; i++) {
        model->table[i] = NULL;
    }
    model->total_entries = 0;
}

unsigned long hash_board(char board[9]) {
    unsigned long hash = 5381;
    for (int i = 0; i < 9; i++) {
        hash = ((hash << 5) + hash) + board[i];
    }
    return hash % Q_TABLE_SIZE;
}

double get_q_value(const QLearningModel *model, char board[9], int action) {
    unsigned long hash = hash_board(board);
    QEntry *entry = model->table[hash];
    
    while (entry != NULL) {
        if (entry->action == action && 
            memcmp(entry->board, board, 9) == 0) {
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
    
    init_qtable(model);
    
    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        if (line[0] != '#') break;
    }
    
    do {
        if (line[0] == '#' || line[0] == '\n') continue;
        
        char board[9];
        int action, visits;
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
                token = strtok(NULL, ",");
                if (token != NULL) {
                    visits = atoi(token);
                    add_q_entry(model, board, action, q_value, visits);
                }
            }
        }
    } while (fgets(line, sizeof(line), fp));
    
    fclose(fp);
    return (model->total_entries > 0);
}

int ql_find_best_move(const QLearningModel *model, char board[9]) {
    int empty_cells[9];
    int empty_count = 0;
    
    for (int i = 0; i < 9; i++) {
        if (board[i] != 'X' && board[i] != 'O') {
            empty_cells[empty_count++] = i;
        }
    }
    
    if (empty_count == 0) return -1;
    
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
        double q_val = get_q_value(model, q_board, move);
        
        if (q_val > best_q) {
            best_q = q_val;
            best_move = move;
        }
    }
    
    return best_move;
}

int ql_classify(const QLearningModel *model, char board[9]) {
    // Use highest Q-value to classify
    char q_board[9];
    for (int i = 0; i < 9; i++) {
        if (board[i] == 'X') q_board[i] = 'x';
        else if (board[i] == 'O') q_board[i] = 'o';
        else q_board[i] = 'b';
    }
    
    double max_q = -1000.0;
    for (int i = 0; i < 9; i++) {
        if (board[i] != 'X' && board[i] != 'O') {
            double q = get_q_value(model, q_board, i);
            if (q > max_q) max_q = q;
        }
    }
    
    // Fixed: Adjusted thresholds for actual Q-value range (-0.67 to +0.68)
    if (max_q > 0.5) return 0;       // Win (high positive Q)
    if (max_q < -0.5) return 1;      // Loss (high negative Q)
    return 2;                         // Draw (near zero)
}

// =====================================================
// EVALUATION FUNCTIONS
// =====================================================
void init_confusion_matrix(ConfusionMatrix *cm) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            cm->matrix[i][j] = 0;
        }
    }
    cm->total = 0;
    cm->accuracy = 0.0;
}

void update_confusion_matrix(ConfusionMatrix *cm, int actual, int predicted) {
    cm->matrix[actual][predicted]++;
    cm->total++;
}

void finalize_confusion_matrix(ConfusionMatrix *cm) {
    int correct = cm->matrix[0][0] + cm->matrix[1][1] + cm->matrix[2][2];
    cm->accuracy = (cm->total > 0) ? ((double)correct / cm->total * 100.0) : 0.0;
}

void print_confusion_matrix(const char *model_name, const ConfusionMatrix *cm) {
    printf("\n========================================\n");
    printf("CONFUSION MATRIX: %s\n", model_name);
    printf("========================================\n");
    printf("               Predicted\n");
    printf("             Win  Loss Draw\n");
    printf("Actual Win  %4d  %4d %4d\n", cm->matrix[0][0], cm->matrix[0][1], cm->matrix[0][2]);
    printf("      Loss  %4d  %4d %4d\n", cm->matrix[1][0], cm->matrix[1][1], cm->matrix[1][2]);
    printf("      Draw  %4d  %4d %4d\n", cm->matrix[2][0], cm->matrix[2][1], cm->matrix[2][2]);
    printf("\nTotal samples: %d\n", cm->total);
    printf("Accuracy: %.2f%%\n", cm->accuracy);
    
    // Calculate precision/recall for each class
    for (int i = 0; i < 3; i++) {
        int true_positive = cm->matrix[i][i];
        int false_positive = 0, false_negative = 0;
        
        for (int j = 0; j < 3; j++) {
            if (j != i) false_negative += cm->matrix[i][j];
            if (j != i) false_positive += cm->matrix[j][i];
        }
        
        double precision = (true_positive + false_positive > 0) ? 
            (double)true_positive / (true_positive + false_positive) * 100.0 : 0.0;
        double recall = (true_positive + false_negative > 0) ? 
            (double)true_positive / (true_positive + false_negative) * 100.0 : 0.0;
        
        const char *class_name = (i == 0) ? "Win" : (i == 1) ? "Loss" : "Draw";
        printf("  %s  - Precision: %.2f%%, Recall: %.2f%%\n", class_name, precision, recall);
    }
}

void init_move_eval_stats(MoveEvalStats *stats) {
    memset(stats, 0, sizeof(MoveEvalStats));
}

void evaluate_move_quality(void *model, AIModelType type, char board[9], MoveEvalStats *stats) {
    if (!has_space(board) || eval_board(board) != 0) return;
    
    // Get model's move
    int model_move = -1;
    switch (type) {
        case AI_MODEL_NAIVE_BAYES:
            model_move = nb_find_best_move((NaiveBayesModel*)model, board);
            break;
        case AI_MODEL_LINEAR_REGRESSION:
            model_move = lr_find_best_move((LinearRegressionModel*)model, board);
            break;
        case AI_MODEL_Q_LEARNING:
            model_move = ql_find_best_move((QLearningModel*)model, board);
            break;
        case AI_MODEL_MINIMAX_EASY:
        case AI_MODEL_MINIMAX_HARD:
            model_move = minimax_best_move(board);
            break;
        default:
            return;
    }
    
    // Get minimax optimal move
    int optimal_move = minimax_best_move(board);
    
    if (model_move == -1 || optimal_move == -1) return;
    
    stats->total_positions++;
    
    // Count moves played
    int move_count = count_moves(board);
    int *phase_total = (move_count <= 3) ? &stats->opening_total :
                       (move_count <= 6) ? &stats->midgame_total :
                                            &stats->endgame_total;
    (*phase_total)++;
    
    // Check if moves agree
    if (model_move == optimal_move) {
        stats->moves_agree_with_minimax++;
        stats->optimal_moves++;
        
        if (move_count <= 3) stats->opening_correct++;
        else if (move_count <= 6) stats->midgame_correct++;
        else stats->endgame_correct++;
    } else {
        // Evaluate if suboptimal or blunder
        char temp[9];
        memcpy(temp, board, 9);
        temp[model_move] = 'O';
        int model_score = minimax(temp, 0, 0);
        
        memcpy(temp, board, 9);
        temp[optimal_move] = 'O';
        int optimal_score = minimax(temp, 0, 0);
        
        if (optimal_score - model_score > 5) {
            stats->blunders++;
        } else {
            stats->suboptimal_moves++;
        }
    }
}

void print_move_eval_stats(const char *model_name, const MoveEvalStats *stats) {
    printf("\n========================================\n");
    printf("MOVE QUALITY ANALYSIS: %s\n", model_name);
    printf("========================================\n");
    printf("Total positions evaluated: %d\n", stats->total_positions);
    printf("\nMove Agreement with Minimax: %d/%d (%.2f%%)\n",
           stats->moves_agree_with_minimax, stats->total_positions,
           (double)stats->moves_agree_with_minimax / stats->total_positions * 100.0);
    
    printf("\nMove Classification:\n");
    printf("  Optimal moves:    %4d (%.1f%%)\n", stats->optimal_moves,
           (double)stats->optimal_moves / stats->total_positions * 100.0);
    printf("  Suboptimal moves: %4d (%.1f%%)\n", stats->suboptimal_moves,
           (double)stats->suboptimal_moves / stats->total_positions * 100.0);
    printf("  Blunders:         %4d (%.1f%%)\n", stats->blunders,
           (double)stats->blunders / stats->total_positions * 100.0);
    
    printf("\nPhase-Specific Performance:\n");
    if (stats->opening_total > 0)
        printf("  Opening (1-3):  %d/%d (%.1f%%)\n", stats->opening_correct, stats->opening_total,
               (double)stats->opening_correct / stats->opening_total * 100.0);
    if (stats->midgame_total > 0)
        printf("  Midgame (4-6):  %d/%d (%.1f%%)\n", stats->midgame_correct, stats->midgame_total,
               (double)stats->midgame_correct / stats->midgame_total * 100.0);
    if (stats->endgame_total > 0)
        printf("  Endgame (7-9):  %d/%d (%.1f%%)\n", stats->endgame_correct, stats->endgame_total,
               (double)stats->endgame_correct / stats->endgame_total * 100.0);
}

// =====================================================
// TEST DATASET GENERATION
// =====================================================
void generate_test_positions(char positions[][9], int *count, int max_positions) {
    *count = 0;
    char board[9];
    
    // Generate diverse test positions through random play
    srand(time(NULL));
    
    for (int game = 0; game < max_positions / 5; game++) {
        // Initialize board
        for (int i = 0; i < 9; i++) board[i] = '0' + i;
        
        char player = 'X';
        
        while (has_space(board) && eval_board(board) == 0 && *count < max_positions) {
            // Save position if O's turn (AI)
            if (player == 'O') {
                memcpy(positions[*count], board, 9);
                (*count)++;
            }
            
            // Make random move
            int empty_cells[9], empty_count = 0;
            for (int i = 0; i < 9; i++) {
                if (board[i] != 'X' && board[i] != 'O') {
                    empty_cells[empty_count++] = i;
                }
            }
            
            if (empty_count == 0) break;
            
            int move = empty_cells[rand() % empty_count];
            board[move] = player;
            player = (player == 'X') ? 'O' : 'X';
        }
    }
}

// =====================================================
// MAIN EVALUATION
// =====================================================
void evaluate_model_comprehensive(void *model, AIModelType type, const char *model_name,
                                   char test_positions[][9], int num_positions) {
    printf("\n\n========================================\n");
    printf("EVALUATING: %s\n", model_name);
    printf("========================================\n");
    
    ConfusionMatrix cm;
    init_confusion_matrix(&cm);
    
    MoveEvalStats move_stats;
    init_move_eval_stats(&move_stats);
    
    // Evaluate each position
    for (int i = 0; i < num_positions; i++) {
        char board[9];
        memcpy(board, test_positions[i], 9);
        
        // Skip if terminal position
        if (!has_space(board) || eval_board(board) != 0) continue;
        
        // Get ground truth classification from Minimax
        int actual = minimax_classify_position(board);
        
        // Get model's classification
        int predicted = -1;
        switch (type) {
            case AI_MODEL_NAIVE_BAYES:
                predicted = nb_classify((NaiveBayesModel*)model, board);
                break;
            case AI_MODEL_LINEAR_REGRESSION:
                predicted = lr_classify((LinearRegressionModel*)model, board);
                break;
            case AI_MODEL_Q_LEARNING:
                predicted = ql_classify((QLearningModel*)model, board);
                break;
            case AI_MODEL_MINIMAX_EASY:
            case AI_MODEL_MINIMAX_HARD:
                predicted = minimax_classify_position(board);
                break;
            default:
                continue;
        }
        
        if (predicted >= 0 && predicted <= 2) {
            update_confusion_matrix(&cm, actual, predicted);
        }
        
        // Evaluate move quality
        evaluate_move_quality(model, type, board, &move_stats);
    }
    
    finalize_confusion_matrix(&cm);
    
    // Print results
    print_confusion_matrix(model_name, &cm);
    print_move_eval_stats(model_name, &move_stats);
}

// =====================================================
// MAIN
// =====================================================
int main() {
    printf("========================================\n");
    printf("COMPREHENSIVE MODEL EVALUATION\n");
    printf("Confusion Matrix + Move Quality Analysis\n");
    printf("========================================\n\n");
    
    // Generate test positions
    printf("Generating test positions...\n");
    char test_positions[500][9];
    int num_positions;
    generate_test_positions(test_positions, &num_positions, 500);
    printf("Generated %d test positions\n", num_positions);
    
    // Load and evaluate Naive Bayes (Non-Terminal)
    NaiveBayesModel nb_model_nt;
    if (nb_load_model("../models/naive_bayes_non_terminal/model_non_terminal.txt", &nb_model_nt)) {
        evaluate_model_comprehensive(&nb_model_nt, AI_MODEL_NAIVE_BAYES, 
                                     "Naive Bayes (Non-Terminal)", 
                                     test_positions, num_positions);
    } else {
        printf("\nWarning: Could not load Naive Bayes (Non-Terminal) model\n");
    }
    
    // Load and evaluate Naive Bayes (Combined)
    NaiveBayesModel nb_model_comb;
    if (nb_load_model("../models/naive_bayes_combined/model_combined.txt", &nb_model_comb)) {
        evaluate_model_comprehensive(&nb_model_comb, AI_MODEL_NAIVE_BAYES, 
                                     "Naive Bayes (Combined)", 
                                     test_positions, num_positions);
    } else {
        printf("\nWarning: Could not load Naive Bayes (Combined) model\n");
    }
    
    // Load and evaluate Linear Regression (Non-Terminal)
    LinearRegressionModel lr_model_nt;
    if (lr_load_model("../models/linear_regression_non_terminal/model_non_terminal.txt", &lr_model_nt)) {
        evaluate_model_comprehensive(&lr_model_nt, AI_MODEL_LINEAR_REGRESSION,
                                     "Linear Regression (Non-Terminal)",
                                     test_positions, num_positions);
    } else {
        printf("\nWarning: Could not load Linear Regression (Non-Terminal) model\n");
    }
    
    // Load and evaluate Linear Regression (Combined)
    LinearRegressionModel lr_model_comb;
    if (lr_load_model("../models/linear_regression_combined/model_combined.txt", &lr_model_comb)) {
        evaluate_model_comprehensive(&lr_model_comb, AI_MODEL_LINEAR_REGRESSION,
                                     "Linear Regression (Combined)",
                                     test_positions, num_positions);
    } else {
        printf("\nWarning: Could not load Linear Regression (Combined) model\n");
    }
    
    // Load and evaluate Q-Learning (Non-Terminal)
    QLearningModel ql_model_nt;
    if (ql_load_model("../models/q learning/q_learning_non_terminal.txt", &ql_model_nt)) {
        evaluate_model_comprehensive(&ql_model_nt, AI_MODEL_Q_LEARNING,
                                     "Q-Learning (Non-Terminal)",
                                     test_positions, num_positions);
    } else {
        printf("\nWarning: Could not load Q-Learning (Non-Terminal) model\n");
    }
    
    // Load and evaluate Q-Learning (Combined/Dataset)
    QLearningModel ql_model_comb;
    if (ql_load_model("../models/q learning/q_learning_dataset.txt", &ql_model_comb)) {
        evaluate_model_comprehensive(&ql_model_comb, AI_MODEL_Q_LEARNING,
                                     "Q-Learning (Dataset-Init)",
                                     test_positions, num_positions);
    } else {
        printf("\nWarning: Could not load Q-Learning (Dataset-Init) model\n");
    }
    
    // Evaluate Minimax Easy (Baseline - should be 100% optimal)
    printf("\nEvaluating Minimax Easy (Perfect AI - Baseline)...\n");
    evaluate_model_comprehensive(NULL, AI_MODEL_MINIMAX_EASY,
                                 "Minimax Easy (Depth Limited)",
                                 test_positions, num_positions);
    
    // Evaluate Minimax Hard (Perfect AI - should be 100% optimal)
    printf("\nEvaluating Minimax Hard (Perfect AI - Gold Standard)...\n");
    evaluate_model_comprehensive(NULL, AI_MODEL_MINIMAX_HARD,
                                 "Minimax Hard (Full Depth)",
                                 test_positions, num_positions);
    
    printf("\n========================================\n");
    printf("EVALUATION COMPLETE\n");
    printf("========================================\n");
    printf("\nKEY METRICS EXPLAINED:\n");
    printf("- Confusion Matrix: Shows how well model predicts Win/Loss/Draw\n");
    printf("- Move Agreement: How often model picks same move as Minimax\n");
    printf("- Optimal Moves: Percentage of perfect decisions\n");
    printf("- Blunders: Critical mistakes that lose winning positions\n");
    printf("- Phase Performance: Accuracy in opening/mid/endgame\n");
    printf("\nBASELINES:\n");
    printf("- Minimax Easy/Hard should show 100%% optimal (gold standard)\n");
    printf("- ML models are compared against this perfect baseline\n");
    printf("- 90%% agreement with Minimax = near-perfect ML model\n");
    
    return 0;
}
