#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#define MAX_SAMPLES 10000
#define FEATURE_COUNT 9
#define Q_TABLE_SIZE 10007

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
    int errors[FEATURE_COUNT][FEATURE_COUNT];
    int total[FEATURE_COUNT][FEATURE_COUNT];
    double error_rate[FEATURE_COUNT][FEATURE_COUNT];
} ErrorMatrix;

// ============================
// LINEAR REGRESSION MODEL
// ============================

typedef struct {
    double weights[FEATURE_COUNT];
    double bias;
    double threshold;
} LinearRegressionModel;

// ============================
// NAIVE BAYES MODEL
// ============================

typedef struct {
    double prior_win;
    double prior_lose;
    double prior_draw;
    double feature_probs_win[FEATURE_COUNT][3];
    double feature_probs_lose[FEATURE_COUNT][3];
    double feature_probs_draw[FEATURE_COUNT][3];
} NaiveBayesModel;

// ============================
// Q-LEARNING MODEL
// ============================

typedef struct QEntry {
    char board[9];
    int action;
    double q_value;
    struct QEntry *next;
} QEntry;

typedef struct {
    QEntry **table;
    int total_entries;
} QLearningModel;

// ============================
// DATASET FUNCTIONS
// ============================

DatasetFormat detect_format(FILE *fp) {
    char line[256];
    long start_pos = ftell(fp);
    
    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == '#' || line[0] == '\n') continue;
        
        fseek(fp, start_pos, SEEK_SET);
        
        if (strstr(line, "x,") || strstr(line, "o,") || strstr(line, "b,")) {
            return FORMAT_CHARACTER;
        } else if (strstr(line, "1.0,") || strstr(line, "-1.0,")) {
            return FORMAT_MATRIX;
        }
        break;
    }
    
    fseek(fp, start_pos, SEEK_SET);
    return FORMAT_CHARACTER;
}

double encode_char_feature(char c) {
    if (c == 'x' || c == 'X') return 1.0;
    if (c == 'o' || c == 'O') return -1.0;
    return 0.0;
}

int encode_char_label(const char *label) {
    if (strcmp(label, "win") == 0 || strcmp(label, "positive") == 0) return 1;
    if (strcmp(label, "lose") == 0 || strcmp(label, "negative") == 0) return -1;
    return 0;  // draw
}

Dataset* load_dataset(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("Error: Cannot open dataset file: %s\n", filename);
        return NULL;
    }
    
    Dataset *dataset = (Dataset*)malloc(sizeof(Dataset));
    dataset->capacity = MAX_SAMPLES;
    dataset->data = (Sample*)malloc(sizeof(Sample) * dataset->capacity);
    dataset->count = 0;
    dataset->format = detect_format(fp);
    
    char line[256];
    while (fgets(line, sizeof(line), fp) && dataset->count < dataset->capacity) {
        if (line[0] == '#' || line[0] == '\n') continue;
        
        Sample sample;
        char *token = strtok(line, ",");
        
        if (dataset->format == FORMAT_CHARACTER) {
            for (int i = 0; i < FEATURE_COUNT && token; i++) {
                sample.features[i] = encode_char_feature(token[0]);
                token = strtok(NULL, ",");
            }
            if (token) {
                char label[20];
                sscanf(token, "%s", label);
                sample.label = encode_char_label(label);
            }
        } else {
            for (int i = 0; i < FEATURE_COUNT && token; i++) {
                sample.features[i] = atof(token);
                token = strtok(NULL, ",");
            }
            if (token) {
                sample.label = atoi(token);
            }
        }
        
        dataset->data[dataset->count++] = sample;
    }
    
    fclose(fp);
    return dataset;
}

// ============================
// LINEAR REGRESSION FUNCTIONS
// ============================

bool load_linear_regression(const char *filename, LinearRegressionModel *model) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return false;
    
    model->threshold = 0.0;
    char line[256];
    
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, "Weight[") && strstr(line, "(bias)")) {
            sscanf(line, "Weight[0] (bias): %lf", &model->bias);
        } else if (strstr(line, "Weight[")) {
            int index;
            double weight;
            if (sscanf(line, "Weight[%d]: %lf", &index, &weight) == 2) {
                if (index >= 1 && index <= FEATURE_COUNT) {
                    model->weights[index - 1] = weight;
                }
            }
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
    
    if (sum > 0.5) return 1;   // Win
    if (sum < -0.5) return -1; // Lose
    return 0;                   // Draw
}

// ============================
// NAIVE BAYES FUNCTIONS
// ============================

int feature_to_state_index(double feature) {
    if (feature > 0.5) return 2;   // X
    if (feature < -0.5) return 0;  // O
    return 1;                       // Blank
}

bool load_naive_bayes(const char *filename, NaiveBayesModel *model) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return false;
    
    model->prior_win = 0.333;
    model->prior_lose = 0.333;
    model->prior_draw = 0.334;
    
    for (int i = 0; i < FEATURE_COUNT; i++) {
        for (int j = 0; j < 3; j++) {
            model->feature_probs_win[i][j] = 0.01;
            model->feature_probs_lose[i][j] = 0.01;
            model->feature_probs_draw[i][j] = 0.01;
        }
    }
    
    char line[512];
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, "P(Label=win)")) {
            sscanf(line, "P(Label=win) = %lf", &model->prior_win);
        } else if (strstr(line, "P(Label=lose)")) {
            sscanf(line, "P(Label=lose) = %lf", &model->prior_lose);
        } else if (strstr(line, "P(Label=draw)")) {
            sscanf(line, "P(Label=draw) = %lf", &model->prior_draw);
        } else if (strstr(line, "Position") && strstr(line, "State=") && strstr(line, "Label=") && strstr(line, "P(State|Label)")) {
            int pos;
            char state[10], label[10];
            double prob;
            
            if (sscanf(line, "Position %d | State=%s | Label=%s | P(State|Label) = %lf", 
                      &pos, state, label, &prob) == 4) {
                if (pos >= 0 && pos < FEATURE_COUNT) {
                    int state_idx = (state[0] == 'o') ? 0 : ((state[0] == 'b') ? 1 : 2);
                    
                    if (strcmp(label, "win") == 0) {
                        model->feature_probs_win[pos][state_idx] = prob;
                    } else if (strcmp(label, "lose") == 0) {
                        model->feature_probs_lose[pos][state_idx] = prob;
                    } else if (strcmp(label, "draw") == 0) {
                        model->feature_probs_draw[pos][state_idx] = prob;
                    }
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
    double log_prob_draw = log(model->prior_draw);
    
    for (int i = 0; i < FEATURE_COUNT; i++) {
        int state = feature_to_state_index(features[i]);
        log_prob_win += log(model->feature_probs_win[i][state]);
        log_prob_lose += log(model->feature_probs_lose[i][state]);
        log_prob_draw += log(model->feature_probs_draw[i][state]);
    }
    
    if (log_prob_win > log_prob_lose && log_prob_win > log_prob_draw) return 1;
    if (log_prob_lose > log_prob_draw) return -1;
    return 0;
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
    
    model->table = (QEntry**)calloc(Q_TABLE_SIZE, sizeof(QEntry*));
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
    char board[9];
    for (int i = 0; i < 9; i++) {
        board[i] = decode_feature(features[i]);
    }
    
    double best_q = -999999.0;
    int best_action = -1;
    
    for (int action = 0; action < 9; action++) {
        if (board[action] == 'b') {
            double q = get_q_value(model, board, action);
            if (q > best_q) {
                best_q = q;
                best_action = action;
            }
        }
    }
    
    if (best_q > 0.5) return 1;
    if (best_q < -0.5) return -1;
    return 0;
}

// ============================
// ERROR MATRIX FUNCTIONS
// ============================

void init_error_matrix(ErrorMatrix *matrix) {
    for (int i = 0; i < FEATURE_COUNT; i++) {
        for (int j = 0; j < FEATURE_COUNT; j++) {
            matrix->errors[i][j] = 0;
            matrix->total[i][j] = 0;
            matrix->error_rate[i][j] = 0.0;
        }
    }
}

void update_error_matrix(ErrorMatrix *matrix, const double features[], int predicted, int actual) {
    bool is_error = (predicted != actual);
    
    // Update error matrix for all occupied position pairs
    for (int i = 0; i < FEATURE_COUNT; i++) {
        for (int j = 0; j < FEATURE_COUNT; j++) {
            // Only count if both positions are occupied (non-blank)
            if (fabs(features[i]) > 0.1 && fabs(features[j]) > 0.1) {
                matrix->total[i][j]++;
                if (is_error) {
                    matrix->errors[i][j]++;
                }
            }
        }
    }
}

void calculate_error_rates(ErrorMatrix *matrix) {
    for (int i = 0; i < FEATURE_COUNT; i++) {
        for (int j = 0; j < FEATURE_COUNT; j++) {
            if (matrix->total[i][j] > 0) {
                matrix->error_rate[i][j] = (double)matrix->errors[i][j] / matrix->total[i][j] * 100.0;
            }
        }
    }
}

void print_error_matrix(const ErrorMatrix *matrix, const char *model_name) {
    const char *pos_names[] = {"TL", "TM", "TR", "ML", "C", "MR", "BL", "BM", "BR"};
    
    printf("\n========================================\n");
    printf("%s - 9x9 ERROR MATRIX\n", model_name);
    printf("========================================\n");
    printf("Shows error rate (%%) for each position pair\n");
    printf("Only includes samples where both positions are occupied\n\n");
    
    // Print header
    printf("       ");
    for (int j = 0; j < FEATURE_COUNT; j++) {
        printf("%5s ", pos_names[j]);
    }
    printf("\n");
    
    // Print matrix
    for (int i = 0; i < FEATURE_COUNT; i++) {
        printf("%5s  ", pos_names[i]);
        for (int j = 0; j < FEATURE_COUNT; j++) {
            if (matrix->total[i][j] > 0) {
                printf("%5.1f ", matrix->error_rate[i][j]);
            } else {
                printf("  --- ");
            }
        }
        printf("\n");
    }
    
    // Find highest error position pairs
    printf("\n--- HIGHEST ERROR POSITION PAIRS (Top 10) ---\n");
    
    typedef struct {
        int i, j;
        double rate;
        int count;
    } ErrorPair;
    
    ErrorPair pairs[FEATURE_COUNT * FEATURE_COUNT];
    int pair_count = 0;
    
    for (int i = 0; i < FEATURE_COUNT; i++) {
        for (int j = i; j < FEATURE_COUNT; j++) {  // Only upper triangle + diagonal
            if (matrix->total[i][j] >= 10) {  // Minimum 10 samples for reliability
                pairs[pair_count].i = i;
                pairs[pair_count].j = j;
                pairs[pair_count].rate = matrix->error_rate[i][j];
                pairs[pair_count].count = matrix->total[i][j];
                pair_count++;
            }
        }
    }
    
    // Sort by error rate (descending)
    for (int i = 0; i < pair_count - 1; i++) {
        for (int j = i + 1; j < pair_count; j++) {
            if (pairs[j].rate > pairs[i].rate) {
                ErrorPair temp = pairs[i];
                pairs[i] = pairs[j];
                pairs[j] = temp;
            }
        }
    }
    
    // Print top 10
    int display_count = (pair_count < 10) ? pair_count : 10;
    for (int k = 0; k < display_count; k++) {
        printf("%2d. %s + %s: %.1f%% error (%d samples)\n", 
               k + 1, 
               pos_names[pairs[k].i], 
               pos_names[pairs[k].j],
               pairs[k].rate,
               pairs[k].count);
    }
    
    // Calculate average error rate
    double total_errors = 0;
    int total_samples = 0;
    for (int i = 0; i < FEATURE_COUNT; i++) {
        for (int j = 0; j < FEATURE_COUNT; j++) {
            total_errors += matrix->errors[i][j];
            total_samples += matrix->total[i][j];
        }
    }
    
    if (total_samples > 0) {
        printf("\nOverall Error Rate: %.2f%% (%d errors / %d position pairs)\n", 
               (total_errors / total_samples * 100.0), 
               (int)total_errors, 
               total_samples);
    }
}

// ============================
// MAIN EVALUATION
// ============================

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Usage: %s <model_file> <model_type> <test_dataset>\n", argv[0]);
        printf("Model types: linear_regression, naive_bayes, q_learning\n");
        return 1;
    }
    
    const char *model_file = argv[1];
    const char *model_type = argv[2];
    const char *test_file = argv[3];
    
    printf("========================================\n");
    printf("9x9 ERROR MATRIX GENERATOR\n");
    printf("========================================\n");
    printf("Model: %s\n", model_type);
    printf("Model File: %s\n", model_file);
    printf("Test Dataset: %s\n\n", test_file);
    
    // Load test dataset
    Dataset *test_data = load_dataset(test_file);
    if (!test_data) {
        printf("Error: Failed to load test dataset\n");
        return 1;
    }
    printf("Test samples: %d\n", test_data->count);
    
    // Initialize error matrix
    ErrorMatrix error_matrix;
    init_error_matrix(&error_matrix);
    
    // Load model and evaluate
    if (strcmp(model_type, "linear_regression") == 0) {
        LinearRegressionModel model;
        if (!load_linear_regression(model_file, &model)) {
            printf("Error: Failed to load Linear Regression model\n");
            return 1;
        }
        
        printf("Evaluating Linear Regression model...\n");
        for (int i = 0; i < test_data->count; i++) {
            int predicted = predict_linear_regression(&model, test_data->data[i].features);
            int actual = test_data->data[i].label;
            update_error_matrix(&error_matrix, test_data->data[i].features, predicted, actual);
        }
        
        calculate_error_rates(&error_matrix);
        print_error_matrix(&error_matrix, "LINEAR REGRESSION");
        
    } else if (strcmp(model_type, "naive_bayes") == 0) {
        NaiveBayesModel model;
        if (!load_naive_bayes(model_file, &model)) {
            printf("Error: Failed to load Naive Bayes model\n");
            return 1;
        }
        
        printf("Evaluating Naive Bayes model...\n");
        for (int i = 0; i < test_data->count; i++) {
            int predicted = predict_naive_bayes(&model, test_data->data[i].features);
            int actual = test_data->data[i].label;
            update_error_matrix(&error_matrix, test_data->data[i].features, predicted, actual);
        }
        
        calculate_error_rates(&error_matrix);
        print_error_matrix(&error_matrix, "NAIVE BAYES");
        
    } else if (strcmp(model_type, "q_learning") == 0) {
        QLearningModel model;
        if (!load_q_learning(model_file, &model)) {
            printf("Error: Failed to load Q-Learning model\n");
            return 1;
        }
        
        printf("Q-Learning entries loaded: %d\n", model.total_entries);
        printf("Evaluating Q-Learning model...\n");
        for (int i = 0; i < test_data->count; i++) {
            int predicted = predict_q_learning(&model, test_data->data[i].features);
            int actual = test_data->data[i].label;
            update_error_matrix(&error_matrix, test_data->data[i].features, predicted, actual);
        }
        
        calculate_error_rates(&error_matrix);
        print_error_matrix(&error_matrix, "Q-LEARNING");
        
    } else {
        printf("Error: Unknown model type '%s'\n", model_type);
        printf("Valid types: linear_regression, naive_bayes, q_learning\n");
        return 1;
    }
    
    free(test_data->data);
    free(test_data);
    
    return 0;
}
