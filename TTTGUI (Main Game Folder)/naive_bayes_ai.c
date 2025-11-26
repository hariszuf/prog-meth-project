#include "naive_bayes_ai.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int nb_load_model(const char *filename, NaiveBayesModel *model) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("Error: Could not open model file %s\n", filename);
        return 0;
    }
    
    char line[256];
    model->label_count = 0;
    for (int i = 0; i < MAX_FEATURES; i++) {
        model->feature_count[i] = 0;
    }
    
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, "Total labels:") != NULL) {
            sscanf(line, "Total labels: %d", &model->label_count);
            fgets(line, sizeof(line), fp); // Skip blank line
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

void nb_predict(const NaiveBayesModel *model, char features[MAX_FEATURES][MAX_FEATURE_LENGTH], 
                char *best_label, double *best_prob) {
    double max_prob = -1.0;
    strcpy(best_label, "draw");
    
    for (int i = 0; i < model->label_count; i++) {
        double prob = model->label_probs[i].probability;
        
        for (int j = 0; j < MAX_FEATURES; j++) {
            int found = 0;
            for (int k = 0; k < model->feature_count[j]; k++) {
                if (strcmp(model->feature_probs[j][k].state, features[j]) == 0 &&
                    strcmp(model->feature_probs[j][k].label, model->label_probs[i].label) == 0) {
                    prob *= model->feature_probs[j][k].probability;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                prob *= 0.001;
            }
        }
        
        if (prob > max_prob) {
            max_prob = prob;
            strcpy(best_label, model->label_probs[i].label);
        }
    }
    
    *best_prob = max_prob;
}

static void board_to_features(const char board[9], char features[MAX_FEATURES][MAX_FEATURE_LENGTH]) {
    for (int i = 0; i < 9; i++) {
        if (board[i] == 'X') {
            strcpy(features[i], "x");
        } else if (board[i] == 'O') {
            strcpy(features[i], "o");
        } else {
            strcpy(features[i], "b");
        }
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
    
    if (empty_count == 0) {
        return -1;
    }
    int best_move = empty_cells[0];
    double best_win_prob = -1.0;
    
    for (int i = 0; i < empty_count; i++) {
        int move = empty_cells[i];
        
        char temp_board[9];
        memcpy(temp_board, board, 9);
        temp_board[move] = 'O';
        
        char features[MAX_FEATURES][MAX_FEATURE_LENGTH];
        board_to_features(temp_board, features);
        
        char predicted_label[MAX_FEATURE_LENGTH];
        double prob;
        nb_predict(model, features, predicted_label, &prob);
        
        double score = 0.0;
        if (strcmp(predicted_label, "win") == 0) {
            score = prob;
        } else if (strcmp(predicted_label, "draw") == 0) {
            score = prob * 0.5;
        } else {
            score = prob * 0.1;
        }
        
        if (score > best_win_prob) {
            best_win_prob = score;
            best_move = move;
        }
    }
    
    if ((rand() % 100) < 20 && empty_count > 1) {
        return empty_cells[rand() % empty_count];
    }
    
    return best_move;
}
