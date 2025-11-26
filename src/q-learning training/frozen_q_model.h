#ifndef FROZEN_Q_MODEL_H
#define FROZEN_Q_MODEL_H

#define BOARD_SIZE 9
#define Q_TABLE_SIZE 20000

typedef struct FrozenQModel FrozenQModel;

FrozenQModel* frozen_q_load(const char *filename);
int frozen_q_get_best_action(const FrozenQModel *model, const char board[BOARD_SIZE]);
double frozen_q_get_q_value(const FrozenQModel *model, const char board[BOARD_SIZE], int action);
int frozen_q_get_entry_count(const FrozenQModel *model);
void frozen_q_free(FrozenQModel *model);

#endif // FROZEN_Q_MODEL_H
