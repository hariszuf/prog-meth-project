#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#define MAX_SAMPLES 10000
#define FEATURE_COUNT 9

typedef enum {
    FORMAT_CHARACTER,
    FORMAT_MATRIX
} DatasetFormat;

typedef struct {
    double features[FEATURE_COUNT];
    int label;
} Sample;

typedef struct {
    Sample *data;
    int count;
    int capacity;
    DatasetFormat format;
} Dataset;

typedef struct {
    int win_pred_win;
    int win_pred_draw;
    int win_pred_lose;
    int draw_pred_win;
    int draw_pred_draw;
    int draw_pred_lose;
    int lose_pred_win;
    int lose_pred_draw;
    int lose_pred_lose;
} ConfusionMatrix;

typedef struct {
    int count;
    int pred_win;
    int pred_draw;
    int pred_lose;
} PositionStats;

typedef struct {
    PositionStats x_state[FEATURE_COUNT];
    PositionStats o_state[FEATURE_COUNT];
    PositionStats b_state[FEATURE_COUNT];
} PerPositionAnalysis;

typedef struct {
    int pred_win[FEATURE_COUNT][FEATURE_COUNT];
    int pred_draw[FEATURE_COUNT][FEATURE_COUNT];
    int pred_lose[FEATURE_COUNT][FEATURE_COUNT];
    int count[FEATURE_COUNT][FEATURE_COUNT];
} MoveInteractionMatrix;

typedef struct {
    double accuracy;
    double error_rate;  // Probability of error = 1 - accuracy
    double precision;
    double recall;
    double f1_score;
    double specificity;
} Metrics;

// ============================
// LINEAR REGRESSION MODEL
// ============================

typedef struct {
    double weights[FEATURE_COUNT];
    double bias;
    double threshold;  // Classification threshold (default 0.0)
} LinearRegressionModel;

// ============================
// NAIVE BAYES MODEL
// ============================

typedef struct {
    double prior_win;
    double prior_lose;
    double feature_probs_win[FEATURE_COUNT][3];   // 3 states: -1, 0, 1
    double feature_probs_lose[FEATURE_COUNT][3];
    int feature_counts_win[FEATURE_COUNT][3];
    int feature_counts_lose[FEATURE_COUNT][3];
} NaiveBayesModel;

// ============================
// Q-LEARNING MODEL
// ============================

#define Q_TABLE_SIZE 20000

typedef struct QEntry {
    char board[9];
    int action;
    double q_value;
    struct QEntry *next;
} QEntry;

typedef struct {
    QEntry *table[Q_TABLE_SIZE];
    int total_entries;
} QLearningModel;

// ============================
// DATASET FUNCTIONS
// ============================

Dataset* create_dataset(int capacity) {
    Dataset *ds = (Dataset*)malloc(sizeof(Dataset));
    ds->data = (Sample*)malloc(sizeof(Sample) * capacity);
    ds->count = 0;
    ds->capacity = capacity;
    ds->format = FORMAT_MATRIX;  // Default, will be auto-detected
    return ds;
}

double encode_char_feature(char c) {
    if (c == 'x') return 1.0;
    if (c == 'o') return -1.0;
    return 0.0;  // 'b' or empty
}

int encode_char_label(const char *str) {
    if (strcmp(str, "win") == 0 || strcmp(str, "positive") == 0) return 1;
    if (strcmp(str, "lose") == 0 || strcmp(str, "negative") == 0) return -1;
    if (strcmp(str, "draw") == 0) return 0;
    return 0;  // default to draw
}

void free_dataset(Dataset *ds) {
    free(ds->data);
    free(ds);
}

DatasetFormat detect_format(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return FORMAT_MATRIX;
    
    char line[512];
    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == '#' || strlen(line) < 5) continue;
        
        // Check if line contains 'x', 'o', 'b' = character format
        if (strchr(line, 'x') || strchr(line, 'o')) {
            fclose(fp);
            return FORMAT_CHARACTER;
        }
        
        // Check if line contains decimal points = matrix format
        if (strchr(line, '.')) {
            fclose(fp);
            return FORMAT_MATRIX;
        }
        break;
    }
    fclose(fp);
    return FORMAT_MATRIX;
}

int load_dataset(const char *filename, Dataset *ds) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("Error: Cannot open dataset file: %s\n", filename);
        return 0;
    }
    
    // Auto-detect format
    ds->format = detect_format(filename);
    fclose(fp);
    
    // Reopen for reading
    fp = fopen(filename, "r");
    if (!fp) return 0;
    
    char line[512];
    ds->count = 0;
    
    if (ds->format == FORMAT_CHARACTER) {
        // Parse character format: x,o,b,...,win/lose/draw
        while (fgets(line, sizeof(line), fp) && ds->count < ds->capacity) {
            if (line[0] == '#' || strlen(line) < 5) continue;
            
            Sample s;
            char *token = strtok(line, ",");
            int idx = 0;
            
            // Read 9 features (x, o, b)
            while (token && idx < FEATURE_COUNT) {
                s.features[idx++] = encode_char_feature(token[0]);
                token = strtok(NULL, ",");
            }
            
            // Read label (win/lose/draw)
            if (token) {
                // Remove newline
                char *newline = strchr(token, '\n');
                if (newline) *newline = '\0';
                token = strtok(token, "\r\n");
                
                s.label = encode_char_label(token);
                // Include all outcomes: win (+1), lose (-1), draw (0)
                ds->data[ds->count++] = s;
            }
        }
    } else {
        // Parse matrix format: 1.0,-1.0,0.0,...,+1/-1
        while (fgets(line, sizeof(line), fp) && ds->count < ds->capacity) {
            if (line[0] == '#' || strlen(line) < 5) continue;
            
            Sample s;
            char *token = strtok(line, ",");
            int idx = 0;
            
            // Read 9 features
            while (token && idx < FEATURE_COUNT) {
                s.features[idx++] = atof(token);
                token = strtok(NULL, ",");
            }
            
            // Read label
            if (token) {
                s.label = atoi(token);
                ds->data[ds->count++] = s;
            }
        }
    }
    
    fclose(fp);
    return ds->count;
}

// ============================
// LINEAR REGRESSION FUNCTIONS
// ============================

bool load_linear_regression(const char *filename, LinearRegressionModel *model) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return false;
    
    model->threshold = 0.0;
    char line[256];
    
    // Skip header lines until we find "Weight[0] (bias):"
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, "Weight[0]") && strstr(line, "bias")) {
            // Read bias value from this line
            char *colon = strchr(line, ':');
            if (colon) {
                model->bias = atof(colon + 1);
                break;
            }
        }
    }
    
    // Read remaining 9 weights (Weight[1] through Weight[9])
    for (int i = 0; i < FEATURE_COUNT; i++) {
        if (!fgets(line, sizeof(line), fp)) {
            fclose(fp);
            return false;
        }
        
        // Extract value after colon
        char *colon = strchr(line, ':');
        if (colon) {
            model->weights[i] = atof(colon + 1);
        } else {
            fclose(fp);
            return false;
        }
    }
    
    fclose(fp);
    return true;
}

int predict_linear_regression(const LinearRegressionModel *model, const double features[]) {
    double sum = model->bias;
    for (int i = 0; i < FEATURE_COUNT; i++) {
        sum += model->weights[i] * features[i];
    }
    // Trinary classification: >0.5=win, <-0.5=lose, else=draw
    if (sum > 0.5) return 1;
    if (sum < -0.5) return -1;
    return 0;
}

// ============================
// NAIVE BAYES FUNCTIONS
// ============================

int feature_to_state_index(double val) {
    if (val < -0.5) return 0;  // -1.0
    if (val > 0.5) return 2;   // +1.0
    return 1;                   // 0.0
}

bool load_naive_bayes(const char *filename, NaiveBayesModel *model) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return false;
    
    char line[256];
    
    // Initialize to defaults
    model->prior_win = 0.5;
    model->prior_lose = 0.5;
    
    // Parse the formatted file
    // First, find the label probabilities
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, "Label: win") && strstr(line, "P(Label)")) {
            char *eq = strchr(line, '=');
            if (eq) model->prior_win = atof(eq + 1);
        }
        if (strstr(line, "Label: lose") && strstr(line, "P(Label)")) {
            char *eq = strchr(line, '=');
            if (eq) model->prior_lose = atof(eq + 1);
        }
        if (strstr(line, "FEATURE PROBABILITIES")) break;
    }
    
    // Initialize all probabilities with uniform distribution
    for (int i = 0; i < FEATURE_COUNT; i++) {
        for (int j = 0; j < 3; j++) {
            model->feature_probs_win[i][j] = 0.333;
            model->feature_probs_lose[i][j] = 0.333;
        }
    }
    
    // Parse feature probabilities
    int current_feature = -1;
    while (fgets(line, sizeof(line), fp)) {
        // Check for feature header "Feature N:"
        if (strstr(line, "Feature ") && strchr(line, ':')) {
            sscanf(line, "Feature %d:", &current_feature);
            continue;
        }
        
        // Parse probability lines: "  State=x     | Label=win        | P(State|Label) = 0.455078"
        if (strstr(line, "State=") && strstr(line, "Label=") && strstr(line, "P(State|Label)")) {
            char state_char;
            char label[10];
            double prob;
            
            // Extract state character
            char *state_ptr = strstr(line, "State=");
            if (state_ptr) {
                state_char = *(state_ptr + 6);
            }
            
            // Extract label
            char *label_ptr = strstr(line, "Label=");
            if (label_ptr) {
                sscanf(label_ptr + 6, "%s", label);
            }
            
            // Extract probability
            char *prob_ptr = strstr(line, "P(State|Label) = ");
            if (prob_ptr) {
                prob = atof(prob_ptr + 17);
            }
            
            // Map state character to index: 'o'=-1→0, 'b'=0→1, 'x'=1→2
            int state_idx = (state_char == 'o') ? 0 : (state_char == 'b') ? 1 : 2;
            
            // Store in appropriate array
            if (current_feature >= 0 && current_feature < FEATURE_COUNT) {
                if (strstr(label, "win")) {
                    model->feature_probs_win[current_feature][state_idx] = prob;
                } else if (strstr(label, "lose")) {
                    model->feature_probs_lose[current_feature][state_idx] = prob;
                }
            }
        }
    }
    
    fclose(fp);
    return true;
}

int predict_naive_bayes(const NaiveBayesModel *model, const double features[]) {
    double log_prob_win = log(model->prior_win);
    double log_prob_lose = log(model->prior_lose);
    
    for (int i = 0; i < FEATURE_COUNT; i++) {
        int state = feature_to_state_index(features[i]);
        log_prob_win += log(model->feature_probs_win[i][state]);
        log_prob_lose += log(model->feature_probs_lose[i][state]);
    }
    
    return (log_prob_win > log_prob_lose) ? 1 : -1;
}

// ============================
// Q-LEARNING FUNCTIONS
// ============================

unsigned long hash_board(const char board[9]) {
    unsigned long hash = 5381;
    for (int i = 0; i < 9; i++) {
        hash = ((hash << 5) + hash) + board[i];
    }
    return hash % Q_TABLE_SIZE;
}

void add_q_entry(QLearningModel *model, char board[9], int action, double q_value) {
    unsigned long hash = hash_board(board);
    QEntry *entry = (QEntry*)malloc(sizeof(QEntry));
    memcpy(entry->board, board, 9);
    entry->action = action;
    entry->q_value = q_value;
    entry->next = model->table[hash];
    model->table[hash] = entry;
    model->total_entries++;
}

bool load_q_learning(const char *filename, QLearningModel *model) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return false;
    
    for (int i = 0; i < Q_TABLE_SIZE; i++) {
        model->table[i] = NULL;
    }
    model->total_entries = 0;
    
    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == '#' || line[0] == '\n') continue;
        
        char board[9];
        int action;
        double q_value;
        
        char *token = strtok(line, ",");
        for (int i = 0; i < 9 && token; i++) {
            board[i] = token[0];
            token = strtok(NULL, ",");
        }
        
        if (token) {
            action = atoi(token);
            token = strtok(NULL, ",");
            if (token) {
                q_value = atof(token);
                add_q_entry(model, board, action, q_value);
            }
        }
    }
    
    fclose(fp);
    return model->total_entries > 0;
}

double get_q_value(const QLearningModel *model, const char board[9], int action) {
    unsigned long hash = hash_board(board);
    QEntry *entry = model->table[hash];
    
    while (entry) {
        if (entry->action == action && memcmp(entry->board, board, 9) == 0) {
            return entry->q_value;
        }
        entry = entry->next;
    }
    return 0.0;
}

char decode_feature(double val) {
    if (val > 0.5) return 'x';
    if (val < -0.5) return 'o';
    return 'b';
}

int predict_q_learning(const QLearningModel *model, const double features[]) {
    // Convert features to board state
    char board[9];
    for (int i = 0; i < 9; i++) {
        board[i] = decode_feature(features[i]);
    }
    
    // Find best Q-value across all possible actions
    double best_q = -999999.0;
    int best_action = -1;
    
    for (int action = 0; action < 9; action++) {
        if (board[action] == 'b') {  // Valid move
            double q = get_q_value(model, board, action);
            if (best_action == -1 || q > best_q) {
                best_q = q;
                best_action = action;
            }
        }
    }
    
    // Predict win if Q-value is positive
    return (best_q > 0.0) ? 1 : -1;
}

void free_q_learning(QLearningModel *model) {
    for (int i = 0; i < Q_TABLE_SIZE; i++) {
        QEntry *entry = model->table[i];
        while (entry) {
            QEntry *next = entry->next;
            free(entry);
            entry = next;
        }
    }
}

// ============================
// CONFUSION MATRIX FUNCTIONS
// ============================

void init_confusion_matrix(ConfusionMatrix *cm) {
    cm->win_pred_win = 0;
    cm->win_pred_draw = 0;
    cm->win_pred_lose = 0;
    cm->draw_pred_win = 0;
    cm->draw_pred_draw = 0;
    cm->draw_pred_lose = 0;
    cm->lose_pred_win = 0;
    cm->lose_pred_draw = 0;
    cm->lose_pred_lose = 0;
}

void init_per_position_analysis(PerPositionAnalysis *ppa) {
    for (int i = 0; i < FEATURE_COUNT; i++) {
        ppa->x_state[i].count = 0;
        ppa->x_state[i].pred_win = 0;
        ppa->x_state[i].pred_draw = 0;
        ppa->x_state[i].pred_lose = 0;
        
        ppa->o_state[i].count = 0;
        ppa->o_state[i].pred_win = 0;
        ppa->o_state[i].pred_draw = 0;
        ppa->o_state[i].pred_lose = 0;
        
        ppa->b_state[i].count = 0;
        ppa->b_state[i].pred_win = 0;
        ppa->b_state[i].pred_draw = 0;
        ppa->b_state[i].pred_lose = 0;
    }
}

void init_move_interaction_matrix(MoveInteractionMatrix *mim) {
    for (int i = 0; i < FEATURE_COUNT; i++) {
        for (int j = 0; j < FEATURE_COUNT; j++) {
            mim->pred_win[i][j] = 0;
            mim->pred_draw[i][j] = 0;
            mim->pred_lose[i][j] = 0;
            mim->count[i][j] = 0;
        }
    }
}

void update_confusion_matrix(ConfusionMatrix *cm, int predicted, int actual) {
    // 3x3 confusion matrix: predicted (rows) × actual (columns)
    if (predicted == 1) {  // Predicted Win
        if (actual == 1) cm->win_pred_win++;
        else if (actual == 0) cm->win_pred_draw++;
        else cm->win_pred_lose++;
    } else if (predicted == 0) {  // Predicted Draw
        if (actual == 1) cm->draw_pred_win++;
        else if (actual == 0) cm->draw_pred_draw++;
        else cm->draw_pred_lose++;
    } else {  // Predicted Lose
        if (actual == 1) cm->lose_pred_win++;
        else if (actual == 0) cm->lose_pred_draw++;
        else cm->lose_pred_lose++;
    }
}

void update_per_position_analysis(PerPositionAnalysis *ppa, const double features[], int predicted) {
    for (int pos = 0; pos < FEATURE_COUNT; pos++) {
        PositionStats *stats;
        
        // Determine which state array to use
        if (features[pos] > 0.5) {
            stats = &ppa->x_state[pos];  // X occupies this position
        } else if (features[pos] < -0.5) {
            stats = &ppa->o_state[pos];  // O occupies this position
        } else {
            stats = &ppa->b_state[pos];  // Blank position
        }
        
        stats->count++;
        if (predicted == 1) stats->pred_win++;
        else if (predicted == 0) stats->pred_draw++;
        else stats->pred_lose++;
    }
}

void update_move_interaction_matrix(MoveInteractionMatrix *mim, const double features[], int predicted) {
    // For each pair of positions, track prediction patterns
    for (int i = 0; i < FEATURE_COUNT; i++) {
        for (int j = 0; j < FEATURE_COUNT; j++) {
            // Only count meaningful interactions (at least one position occupied)
            if ((features[i] > 0.5 || features[i] < -0.5) && 
                (features[j] > 0.5 || features[j] < -0.5)) {
                
                mim->count[i][j]++;
                if (predicted == 1) mim->pred_win[i][j]++;
                else if (predicted == 0) mim->pred_draw[i][j]++;
                else mim->pred_lose[i][j]++;
            }
        }
    }
}

void calculate_metrics(const ConfusionMatrix *cm, Metrics *m) {
    // Total samples
    int total = cm->win_pred_win + cm->win_pred_draw + cm->win_pred_lose +
                cm->draw_pred_win + cm->draw_pred_draw + cm->draw_pred_lose +
                cm->lose_pred_win + cm->lose_pred_draw + cm->lose_pred_lose;
    
    // Correct predictions (diagonal)
    int correct = cm->win_pred_win + cm->draw_pred_draw + cm->lose_pred_lose;
    
    m->accuracy = (total > 0) ? (double)correct / total : 0.0;
    m->error_rate = (total > 0) ? (double)(total - correct) / total : 0.0;
    
    // Metrics for Win class (treating as binary: Win vs Not-Win)
    int tp_win = cm->win_pred_win;
    int fp_win = cm->win_pred_draw + cm->win_pred_lose;
    int fn_win = cm->draw_pred_win + cm->lose_pred_win;
    int tn_win = cm->draw_pred_draw + cm->draw_pred_lose + cm->lose_pred_draw + cm->lose_pred_lose;
    
    m->precision = (tp_win + fp_win > 0) ? (double)tp_win / (tp_win + fp_win) : 0.0;
    m->recall = (tp_win + fn_win > 0) ? (double)tp_win / (tp_win + fn_win) : 0.0;
    m->specificity = (tn_win + fp_win > 0) ? (double)tn_win / (tn_win + fp_win) : 0.0;
    m->f1_score = (m->precision + m->recall > 0) ?
        2 * (m->precision * m->recall) / (m->precision + m->recall) : 0.0;
}

void print_per_position_analysis(const PerPositionAnalysis *ppa) {
    const char *position_names[] = {
        "Top-Left", "Top-Center", "Top-Right",
        "Mid-Left", "Center", "Mid-Right",
        "Bot-Left", "Bot-Center", "Bot-Right"
    };
    
    printf("\n");
    printf("========================================\n");
    printf(" PER-POSITION PREDICTION ANALYSIS\n");
    printf("========================================\n");
    printf("Shows how predictions vary based on board position occupancy\n\n");
    
    for (int pos = 0; pos < FEATURE_COUNT; pos++) {
        printf("Position %d (%s):\n", pos, position_names[pos]);
        printf("  -----------------------------------------------\n");
        
        // X state
        if (ppa->x_state[pos].count > 0) {
            double win_pct = 100.0 * ppa->x_state[pos].pred_win / ppa->x_state[pos].count;
            double draw_pct = 100.0 * ppa->x_state[pos].pred_draw / ppa->x_state[pos].count;
            double lose_pct = 100.0 * ppa->x_state[pos].pred_lose / ppa->x_state[pos].count;
            printf("  When X: Win=%.1f%%, Draw=%.1f%%, Lose=%.1f%% (n=%d)\n",
                   win_pct, draw_pct, lose_pct, ppa->x_state[pos].count);
        } else {
            printf("  When X: (no samples)\n");
        }
        
        // O state
        if (ppa->o_state[pos].count > 0) {
            double win_pct = 100.0 * ppa->o_state[pos].pred_win / ppa->o_state[pos].count;
            double draw_pct = 100.0 * ppa->o_state[pos].pred_draw / ppa->o_state[pos].count;
            double lose_pct = 100.0 * ppa->o_state[pos].pred_lose / ppa->o_state[pos].count;
            printf("  When O: Win=%.1f%%, Draw=%.1f%%, Lose=%.1f%% (n=%d)\n",
                   win_pct, draw_pct, lose_pct, ppa->o_state[pos].count);
        } else {
            printf("  When O: (no samples)\n");
        }
        
        // Blank state
        if (ppa->b_state[pos].count > 0) {
            double win_pct = 100.0 * ppa->b_state[pos].pred_win / ppa->b_state[pos].count;
            double draw_pct = 100.0 * ppa->b_state[pos].pred_draw / ppa->b_state[pos].count;
            double lose_pct = 100.0 * ppa->b_state[pos].pred_lose / ppa->b_state[pos].count;
            printf("  When B: Win=%.1f%%, Draw=%.1f%%, Lose=%.1f%% (n=%d)\n",
                   win_pct, draw_pct, lose_pct, ppa->b_state[pos].count);
        } else {
            printf("  When B: (no samples)\n");
        }
        
        printf("\n");
    }
    
    // Summary: Which positions matter most
    printf("========================================\n");
    printf(" POSITION IMPORTANCE ANALYSIS\n");
    printf("========================================\n");
    printf("Variance in predictions indicates position importance\n\n");
    
    for (int pos = 0; pos < FEATURE_COUNT; pos++) {
        double x_win = (ppa->x_state[pos].count > 0) ? 
            100.0 * ppa->x_state[pos].pred_win / ppa->x_state[pos].count : 0;
        double o_win = (ppa->o_state[pos].count > 0) ? 
            100.0 * ppa->o_state[pos].pred_win / ppa->o_state[pos].count : 0;
        double b_win = (ppa->b_state[pos].count > 0) ? 
            100.0 * ppa->b_state[pos].pred_win / ppa->b_state[pos].count : 0;
        
        double variance = (x_win - o_win) * (x_win - o_win) + 
                         (x_win - b_win) * (x_win - b_win) + 
                         (o_win - b_win) * (o_win - b_win);
        
        printf("Position %d (%s): Variance=%.1f ", pos, position_names[pos], variance);
        if (variance > 1500) printf("[HIGH IMPORTANCE]\n");
        else if (variance > 500) printf("[MODERATE]\n");
        else printf("[LOW]\n");
    }
    printf("\n");
}

void print_move_interaction_matrix(const MoveInteractionMatrix *mim) {
    const char *pos_labels[] = {"TL", "TM", "TR", "ML", "C ", "MR", "BL", "BM", "BR"};
    
    printf("\n");
    printf("========================================\n");
    printf(" 9x9 MOVE INTERACTION ANALYSIS\n");
    printf("========================================\n");
    printf("Shows win prediction % when position pairs are occupied\n");
    printf("(Rows = Position, Columns = Position)\n\n");
    
    // Header
    printf("      ");
    for (int j = 0; j < FEATURE_COUNT; j++) {
        printf(" %s  ", pos_labels[j]);
    }
    printf("\n");
    printf("    +");
    for (int j = 0; j < FEATURE_COUNT; j++) {
        printf("-----");
    }
    printf("+\n");
    
    // Matrix
    for (int i = 0; i < FEATURE_COUNT; i++) {
        printf(" %s |", pos_labels[i]);
        for (int j = 0; j < FEATURE_COUNT; j++) {
            if (mim->count[i][j] >= 10) {  // Only show if enough samples
                double win_pct = 100.0 * mim->pred_win[i][j] / mim->count[i][j];
                printf(" %2.0f  ", win_pct);
            } else {
                printf("  -  ");
            }
        }
        printf("|\n");
    }
    
    printf("    +");
    for (int j = 0; j < FEATURE_COUNT; j++) {
        printf("-----");
    }
    printf("+\n");
    printf("\nLegend: TL=Top-Left, TM=Top-Mid, TR=Top-Right, ML=Mid-Left, C=Center,\n");
    printf("        MR=Mid-Right, BL=Bot-Left, BM=Bot-Mid, BR=Bot-Right\n");
    printf("Values show win prediction percentage. '-' means insufficient data (<10 samples)\n");
    
    // Find strongest combinations
    printf("\n========================================\n");
    printf(" STRONGEST POSITION COMBINATIONS\n");
    printf("========================================\n");
    printf("Position pairs that most strongly predict wins:\n\n");
    
    typedef struct { int i; int j; double win_pct; int count; } Combo;
    Combo top_combos[10] = {0};
    int top_count = 0;
    
    for (int i = 0; i < FEATURE_COUNT; i++) {
        for (int j = i+1; j < FEATURE_COUNT; j++) {
            if (mim->count[i][j] >= 20) {
                double win_pct = 100.0 * mim->pred_win[i][j] / mim->count[i][j];
                
                // Insert into top 10
                for (int k = 0; k < 10; k++) {
                    if (top_count < 10 && k == top_count) {
                        top_combos[k].i = i;
                        top_combos[k].j = j;
                        top_combos[k].win_pct = win_pct;
                        top_combos[k].count = mim->count[i][j];
                        top_count++;
                        break;
                    } else if (win_pct > top_combos[k].win_pct) {
                        // Shift down
                        for (int m = 9; m > k; m--) {
                            top_combos[m] = top_combos[m-1];
                        }
                        top_combos[k].i = i;
                        top_combos[k].j = j;
                        top_combos[k].win_pct = win_pct;
                        top_combos[k].count = mim->count[i][j];
                        if (top_count < 10) top_count++;
                        break;
                    }
                }
            }
        }
    }
    
    for (int k = 0; k < top_count && k < 10; k++) {
        printf("%d. %s + %s: %.1f%% win prediction (n=%d)\n",
               k+1, pos_labels[top_combos[k].i], pos_labels[top_combos[k].j],
               top_combos[k].win_pct, top_combos[k].count);
    }
    printf("\n");
}

void print_confusion_matrix(const char *model_name, const char *dataset_name,
                           const ConfusionMatrix *cm, const Metrics *m) {
    printf("========================================\n");
    printf(" %s - %s Dataset\n", model_name, dataset_name);
    printf("========================================\n\n");
    
    printf("3x3 Confusion Matrix:\n");
    printf("                      Actual\n");
    printf("              Win       Draw      Lose\n");
    printf("        Win | %5d  |  %5d  |  %5d  |\n", 
           cm->win_pred_win, cm->win_pred_draw, cm->win_pred_lose);
    printf("Predicted   |--------+---------+--------|\n");
    printf("       Draw | %5d  |  %5d  |  %5d  |\n", 
           cm->draw_pred_win, cm->draw_pred_draw, cm->draw_pred_lose);
    printf("            |--------+---------+--------|\n");
    printf("       Lose | %5d  |  %5d  |  %5d  |\n", 
           cm->lose_pred_win, cm->lose_pred_draw, cm->lose_pred_lose);
    printf("\n");
    
    // Calculate per-class accuracies
    int total_win = cm->win_pred_win + cm->draw_pred_win + cm->lose_pred_win;
    int total_draw = cm->win_pred_draw + cm->draw_pred_draw + cm->lose_pred_draw;
    int total_lose = cm->win_pred_lose + cm->draw_pred_lose + cm->lose_pred_lose;
    
    printf("Per-Class Statistics:\n");
    printf("  Win:  %d samples, %d correct (%.2f%% recall)\n", 
           total_win, cm->win_pred_win, 
           total_win > 0 ? (double)cm->win_pred_win / total_win * 100 : 0.0);
    printf("  Draw: %d samples, %d correct (%.2f%% recall)\n", 
           total_draw, cm->draw_pred_draw,
           total_draw > 0 ? (double)cm->draw_pred_draw / total_draw * 100 : 0.0);
    printf("  Lose: %d samples, %d correct (%.2f%% recall)\n", 
           total_lose, cm->lose_pred_lose,
           total_lose > 0 ? (double)cm->lose_pred_lose / total_lose * 100 : 0.0);
    printf("\n");
    
    int total = cm->win_pred_win + cm->win_pred_draw + cm->win_pred_lose +
                cm->draw_pred_win + cm->draw_pred_draw + cm->draw_pred_lose +
                cm->lose_pred_win + cm->lose_pred_draw + cm->lose_pred_lose;
    int correct = cm->win_pred_win + cm->draw_pred_draw + cm->lose_pred_lose;
    
    printf("Overall Performance Metrics:\n");
    printf("  Overall Accuracy:  %.2f%% (%d/%d correct)\n", 
           m->accuracy * 100, correct, total);
    printf("  Error Rate:        %.2f%% (%d/%d errors)\n",
           m->error_rate * 100, total - correct, total);
    printf("\n");
    
    printf("Win Class Metrics (Binary: Win vs Not-Win):\n");
    printf("  Precision:         %.2f%% (Win predictions)\n", m->precision * 100);
    printf("  Recall (Sensitivity): %.2f%% (Win detection)\n", m->recall * 100);
    printf("  Specificity:       %.2f%% (Not-Win detection)\n", m->specificity * 100);
    printf("  F1-Score:          %.4f\n", m->f1_score);
    printf("\n");
}

// ============================
// MAIN EVALUATION
// ============================

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Usage: %s <model_path> <train_dataset> <test_dataset>\n", argv[0]);
        printf("\nExample:\n");
        printf("  %s models/linear_regression_non_terminal/model_non_terminal_matrix.txt \\\n", argv[0]);
        printf("         dataset/new processed/train_non_terminal_matrix.data \\\n");
        printf("         dataset/new processed/test_non_terminal_matrix.data\n");
        return 1;
    }
    
    const char *model_path = argv[1];
    const char *train_path = argv[2];
    const char *test_path = argv[3];
    
    printf("========================================\n");
    printf(" CONFUSION MATRIX EVALUATION\n");
    printf("========================================\n");
    printf("Model:         %s\n", model_path);
    printf("Train Dataset: %s\n", train_path);
    printf("Test Dataset:  %s\n", test_path);
    printf("\n");
    
    // Detect model type
    bool is_lr = (strstr(model_path, "linear_regression") != NULL);
    bool is_nb = (strstr(model_path, "naive_bayes") != NULL);
    bool is_ql = (strstr(model_path, "q_learning") != NULL);
    
    if (!is_lr && !is_nb && !is_ql) {
        printf("Error: Cannot determine model type from filename.\n");
        printf("Expected 'linear_regression', 'naive_bayes', or 'q_learning' in path.\n");
        return 1;
    }
    
    // Load model
    LinearRegressionModel lr_model;
    NaiveBayesModel nb_model;
    QLearningModel ql_model;
    
    bool loaded = false;
    const char *model_name = "";
    
    if (is_lr) {
        loaded = load_linear_regression(model_path, &lr_model);
        model_name = "Linear Regression";
    } else if (is_nb) {
        loaded = load_naive_bayes(model_path, &nb_model);
        model_name = "Naive Bayes";
    } else if (is_ql) {
        loaded = load_q_learning(model_path, &ql_model);
        model_name = "Q-Learning";
    }
    
    if (!loaded) {
        printf("Error: Failed to load model from %s\n", model_path);
        return 1;
    }
    
    printf("✓ Model loaded successfully: %s\n", model_name);
    
    // Load datasets
    Dataset *train_ds = create_dataset(MAX_SAMPLES);
    Dataset *test_ds = create_dataset(MAX_SAMPLES);
    
    int train_count = load_dataset(train_path, train_ds);
    int test_count = load_dataset(test_path, test_ds);
    
    if (train_count == 0 || test_count == 0) {
        printf("Error: Failed to load datasets.\n");
        free_dataset(train_ds);
        free_dataset(test_ds);
        return 1;
    }
    
    printf("✓ Dataset format: %s\n", 
           train_ds->format == FORMAT_CHARACTER ? "Character (x,o,b)" : "Matrix (numerical)");
    printf("✓ Training samples: %d\n", train_count);
    printf("✓ Testing samples:  %d\n", test_count);
    
    // Evaluate on training set
    ConfusionMatrix train_cm;
    init_confusion_matrix(&train_cm);
    PerPositionAnalysis train_ppa;
    init_per_position_analysis(&train_ppa);
    MoveInteractionMatrix train_mim;
    init_move_interaction_matrix(&train_mim);
    
    for (int i = 0; i < train_ds->count; i++) {
        int predicted;
        if (is_lr) predicted = predict_linear_regression(&lr_model, train_ds->data[i].features);
        else if (is_nb) predicted = predict_naive_bayes(&nb_model, train_ds->data[i].features);
        else predicted = predict_q_learning(&ql_model, train_ds->data[i].features);
        
        update_confusion_matrix(&train_cm, predicted, train_ds->data[i].label);
        update_per_position_analysis(&train_ppa, train_ds->data[i].features, predicted);
        update_move_interaction_matrix(&train_mim, train_ds->data[i].features, predicted);
    }
    
    Metrics train_metrics;
    calculate_metrics(&train_cm, &train_metrics);
    print_confusion_matrix(model_name, "Training", &train_cm, &train_metrics);
    print_per_position_analysis(&train_ppa);
    print_move_interaction_matrix(&train_mim);
    
    // Evaluate on test set
    ConfusionMatrix test_cm;
    init_confusion_matrix(&test_cm);
    PerPositionAnalysis test_ppa;
    init_per_position_analysis(&test_ppa);
    MoveInteractionMatrix test_mim;
    init_move_interaction_matrix(&test_mim);
    
    for (int i = 0; i < test_ds->count; i++) {
        int predicted;
        if (is_lr) predicted = predict_linear_regression(&lr_model, test_ds->data[i].features);
        else if (is_nb) predicted = predict_naive_bayes(&nb_model, test_ds->data[i].features);
        else predicted = predict_q_learning(&ql_model, test_ds->data[i].features);
        
        update_confusion_matrix(&test_cm, predicted, test_ds->data[i].label);
        update_per_position_analysis(&test_ppa, test_ds->data[i].features, predicted);
        update_move_interaction_matrix(&test_mim, test_ds->data[i].features, predicted);
    }
    
    Metrics test_metrics;
    calculate_metrics(&test_cm, &test_metrics);
    print_confusion_matrix(model_name, "Testing", &test_cm, &test_metrics);
    print_per_position_analysis(&test_ppa);
    print_move_interaction_matrix(&test_mim);
    
    // Comparison summary
    printf("========================================\n");
    printf(" TRAINING vs TESTING COMPARISON\n");
    printf("========================================\n");
    printf("                Training    Testing    Difference\n");
    printf("Accuracy:       %.2f%%      %.2f%%     %+.2f%%\n", 
           train_metrics.accuracy * 100, test_metrics.accuracy * 100,
           (test_metrics.accuracy - train_metrics.accuracy) * 100);
    printf("Precision:      %.2f%%      %.2f%%     %+.2f%%\n",
           train_metrics.precision * 100, test_metrics.precision * 100,
           (test_metrics.precision - train_metrics.precision) * 100);
    printf("Recall:         %.2f%%      %.2f%%     %+.2f%%\n",
           train_metrics.recall * 100, test_metrics.recall * 100,
           (test_metrics.recall - train_metrics.recall) * 100);
    printf("F1-Score:       %.4f      %.4f     %+.4f\n",
           train_metrics.f1_score, test_metrics.f1_score,
           test_metrics.f1_score - train_metrics.f1_score);
    printf("\n");
    
    // Check for overfitting
    double accuracy_diff = train_metrics.accuracy - test_metrics.accuracy;
    if (accuracy_diff > 0.05) {
        printf("⚠ Warning: Potential overfitting detected!\n");
        printf("   Training accuracy is %.2f%% higher than testing.\n", accuracy_diff * 100);
    } else if (accuracy_diff < -0.05) {
        printf("ℹ Note: Testing accuracy exceeds training (unusual but possible).\n");
    } else {
        printf("✓ Good generalization: Training and testing performance are balanced.\n");
    }
    
    // Cleanup
    free_dataset(train_ds);
    free_dataset(test_ds);
    if (is_ql) free_q_learning(&ql_model);
    
    return 0;
}