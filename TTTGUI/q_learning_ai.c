#include "q_learning_ai.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned long hash_board(char board[9]) {
    unsigned long hash = 5381;
    for (int i = 0; i < 9; i++) {
        hash = ((hash << 5) + hash) + board[i];
    }
    return hash % Q_TABLE_SIZE;
}

static void init_qtable(QLearningModel *model) {
    for (int i = 0; i < Q_TABLE_SIZE; i++) {
        model->table[i] = NULL;
    }
    model->total_entries = 0;
}

static double get_q_value(const QLearningModel *model, char board[9], int action) {
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

static void add_q_entry(QLearningModel *model, char board[9], int action, double q_value, int visits) {
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
    if (!fp) {
        printf("Error: Could not open Q-Learning model file %s\n", filename);
        return 0;
    }
    
    init_qtable(model);
    
    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        if (line[0] != '#') break;
    }
    
    int entries_loaded = 0;
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
                    entries_loaded++;
                }
            }
        }
    } while (fgets(line, sizeof(line), fp));
    
    fclose(fp);
    printf("Q-Learning model loaded successfully: %d entries\n", entries_loaded);
    return (model->total_entries > 0);
}

static void convert_board_format(const char *game_board, char *q_board) {
    for (int i = 0; i < 9; i++) {
        if (game_board[i] == 'X') {
            q_board[i] = 'x';
        } else if (game_board[i] == 'O') {
            q_board[i] = 'o';
        } else {
            q_board[i] = 'b';
        }
    }
}

int ql_find_best_move(const QLearningModel *model, char board[9]) {
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
    char q_board[9];
    convert_board_format(board, q_board);
    
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

void ql_free_model(QLearningModel *model) {
    for (int i = 0; i < Q_TABLE_SIZE; i++) {
        QEntry *entry = model->table[i];
        while (entry != NULL) {
            QEntry *temp = entry;
            entry = entry->next;
            free(temp);
        }
        model->table[i] = NULL;
    }
    model->total_entries = 0;
}
