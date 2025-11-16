// frozen_q_model.h - Read-only Q-Learning model loader for deployment
#ifndef FROZEN_Q_MODEL_H
#define FROZEN_Q_MODEL_H

#define BOARD_SIZE 9
#define Q_TABLE_SIZE 20000

// Opaque structure - users can't modify internal data
typedef struct FrozenQModel FrozenQModel;

// API Functions

/**
 * Load a frozen (read-only) Q-Learning model from file
 * Returns NULL on failure
 */
FrozenQModel* frozen_q_load(const char *filename);

/**
 * Get the best action for a given board state
 * Returns -1 if no valid moves or model is NULL
 */
int frozen_q_get_best_action(const FrozenQModel *model, const char board[BOARD_SIZE]);

/**
 * Get the Q-value for a specific state-action pair
 * Returns 0.0 if state-action not found or model is NULL
 */
double frozen_q_get_q_value(const FrozenQModel *model, const char board[BOARD_SIZE], int action);

/**
 * Get model statistics
 */
int frozen_q_get_entry_count(const FrozenQModel *model);

/**
 * Free model memory
 */
void frozen_q_free(FrozenQModel *model);

#endif // FROZEN_Q_MODEL_H
