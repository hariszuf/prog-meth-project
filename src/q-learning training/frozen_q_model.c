#include "frozen_q_model.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct QEntry {
    char board[BOARD_SIZE];
    int action;
    double q_value;
    struct QEntry *next;
} QEntry;

struct FrozenQModel {
    QEntry *table[Q_TABLE_SIZE];
    int total_entries;
    int is_frozen;
    char model_name[256];
};

static unsigned long hash_board(const char board[BOARD_SIZE]) {
    unsigned long hash = 5381;
    for (int i = 0; i < BOARD_SIZE; i++) {
        hash = ((hash << 5) + hash) + board[i];
    }
    return hash % Q_TABLE_SIZE;
}

FrozenQModel* frozen_q_load(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "[ERROR] Cannot open model file: %s\n", filename);
        return NULL;
    }
    
    FrozenQModel *model = (FrozenQModel *)malloc(sizeof(FrozenQModel));
    if (!model) {
        fclose(fp);
        return NULL;
    }
    
    for (int i = 0; i < Q_TABLE_SIZE; i++) {
        model->table[i] = NULL;
    }
    model->total_entries = 0;
    model->is_frozen = 1;
    strncpy(model->model_name, filename, sizeof(model->model_name) - 1);
    model->model_name[sizeof(model->model_name) - 1] = '\0';
    
    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == '#' || line[0] == '\n') continue;
        
        char board[BOARD_SIZE];
        int action;
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
                
                unsigned long hash = hash_board(board);
                QEntry *new_entry = (QEntry *)malloc(sizeof(QEntry));
                memcpy(new_entry->board, board, BOARD_SIZE);
                new_entry->action = action;
                new_entry->q_value = q_value;
                new_entry->next = model->table[hash];
                model->table[hash] = new_entry;
                model->total_entries++;
            }
        }
    }
    
    fclose(fp);
    
    if (model->total_entries == 0) {
        fprintf(stderr, "[WARNING] Loaded model has 0 entries from %s\n", filename);
    } else {
        printf("[INFO] Loaded frozen model: %s (%d entries)\n", filename, model->total_entries);
    }
    
    return model;
}

// Get Q-value for a specific state-action pair
double frozen_q_get_q_value(const FrozenQModel *model, const char board[BOARD_SIZE], int action) {
    if (!model || !model->is_frozen) return 0.0;
    
    unsigned long hash = hash_board(board);
    QEntry *entry = model->table[hash];
    
    while (entry != NULL) {
        if (entry->action == action && memcmp(entry->board, board, BOARD_SIZE) == 0) {
            return entry->q_value;
        }
        entry = entry->next;
    }
    
    return 0.0; // Default for unseen state-action pairs
}

// Get the best action for a given board state
int frozen_q_get_best_action(const FrozenQModel *model, const char board[BOARD_SIZE]) {
    if (!model || !model->is_frozen) return -1;
    
    // Find all valid moves
    int valid_moves[BOARD_SIZE];
    int num_moves = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (board[i] == 'b') {
            valid_moves[num_moves++] = i;
        }
    }
    
    if (num_moves == 0) return -1;
    
    // Find move with highest Q-value
    int best_move = valid_moves[0];
    double best_q = frozen_q_get_q_value(model, board, best_move);
    
    for (int i = 1; i < num_moves; i++) {
        double q = frozen_q_get_q_value(model, board, valid_moves[i]);
        if (q > best_q) {
            best_q = q;
            best_move = valid_moves[i];
        }
    }
    
    return best_move;
}

// Get model statistics
int frozen_q_get_entry_count(const FrozenQModel *model) {
    return model ? model->total_entries : 0;
}

// Free model memory
void frozen_q_free(FrozenQModel *model) {
    if (!model) return;
    
    for (int i = 0; i < Q_TABLE_SIZE; i++) {
        QEntry *entry = model->table[i];
        while (entry != NULL) {
            QEntry *temp = entry;
            entry = entry->next;
            free(temp);
        }
    }
    
    free(model);
}
