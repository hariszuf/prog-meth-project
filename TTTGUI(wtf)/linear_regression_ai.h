// linear_regression_ai.h - Linear Regression AI for Tic-Tac-Toe
#ifndef LINEAR_REGRESSION_AI_H
#define LINEAR_REGRESSION_AI_H

#define LR_NUM_FEATURES 10  // 9 board positions + 1 bias term

// Linear Regression Model structure
typedef struct {
    double weights[LR_NUM_FEATURES];
} LinearRegressionModel;

// Load the Linear Regression model from a text file
int lr_load_model(const char *filename, LinearRegressionModel *model);

// Find the best move for 'O' using Linear Regression prediction
int lr_find_best_move(const LinearRegressionModel *model, char board[9]);

// Helper: predict outcome for given board state
double lr_predict(const LinearRegressionModel *model, const char board[9]);

#endif // LINEAR_REGRESSION_AI_H
