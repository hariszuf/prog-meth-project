// q_learning_ai.h - Q-Learning AI for Tic-Tac-Toe
#ifndef Q_LEARNING_AI_H
#define Q_LEARNING_AI_H

#define Q_TABLE_SIZE 20000

// Q-table entry structure
typedef struct QEntry {
    char board[9];
    int action;
    double q_value;
    int visits;
    struct QEntry *next;
} QEntry;

// Q-Learning model structure
typedef struct {
    QEntry *table[Q_TABLE_SIZE];
    int total_entries;
} QLearningModel;

// Load the Q-Learning model from a text file
int ql_load_model(const char *filename, QLearningModel *model);

// Find the best move for 'O' using Q-Learning
int ql_find_best_move(const QLearningModel *model, char board[9]);

// Free Q-Learning model memory
void ql_free_model(QLearningModel *model);

#endif // Q_LEARNING_AI_H
