// naive_bayes_ai.h - Naive Bayes AI for Tic-Tac-Toe
#ifndef NAIVE_BAYES_AI_H
#define NAIVE_BAYES_AI_H

#define MAX_FEATURES 9
#define MAX_STATES 10
#define MAX_LABELS 10
#define MAX_FEATURE_LENGTH 32

// Probability structure for feature-state-label combinations
typedef struct {
    char state[MAX_FEATURE_LENGTH];
    char label[MAX_FEATURE_LENGTH];
    double probability;
} FeatureProbability;

// Probability structure for labels
typedef struct {
    char label[MAX_FEATURE_LENGTH];
    double probability;
} LabelProbability;

// The Naive Bayes model structure
typedef struct {
    FeatureProbability feature_probs[MAX_FEATURES][MAX_STATES * MAX_LABELS];
    int feature_count[MAX_FEATURES];
    LabelProbability label_probs[MAX_LABELS];
    int label_count;
} NaiveBayesModel;

// Load the Naive Bayes model from a text file
int nb_load_model(const char *filename, NaiveBayesModel *model);

// Find the best move for 'O' using Naive Bayes prediction
int nb_find_best_move(const NaiveBayesModel *model, char board[9]);

// Helper: predict label for given board state
void nb_predict(const NaiveBayesModel *model, char features[MAX_FEATURES][MAX_FEATURE_LENGTH], 
                char *best_label, double *best_prob);

#endif // NAIVE_BAYES_AI_H
