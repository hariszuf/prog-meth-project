#ifndef Q_LEARNING_AI_H
#define Q_LEARNING_AI_H

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

int ql_load_model(const char *filename, QLearningModel *model);
int ql_find_best_move(const QLearningModel *model, char board[9]);
void ql_free_model(QLearningModel *model);

#endif // Q_LEARNING_AI_H
