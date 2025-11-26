#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_INSTANCES 10000
#define NUM_FEATURES 9
#define NUM_STATES 3
#define NUM_CLASSES 2

typedef struct {
    double features[NUM_FEATURES];
    int label;
} Instance;

typedef struct {
    double feature_prob[NUM_FEATURES][NUM_STATES][NUM_CLASSES];
    double class_prob[NUM_CLASSES];
    int feature_count[NUM_FEATURES][NUM_STATES][NUM_CLASSES];
    int class_count[NUM_CLASSES];
    int total_samples;
} NaiveBayesModel;
int feature_to_state(double feature) {
    if (feature > 0.5) return 0;      // x
    if (feature < -0.5) return 2;     // o
    return 1;                          // b
}

// Convert label to class index
// +1 (win) -> 0, -1 (lose) -> 1
int label_to_class(int label) {
    return (label == 1) ? 0 : 1;
}

// Load matrix format data from file
int load_matrix_data(const char *filename, Instance *data) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file: %s\n", filename);
        return 0;
    }
    
    char line[256];
    int count = 0;
    
    printf("Loading matrix format data from %s...\n", filename);
    
    while (fgets(line, sizeof(line), file) && count < MAX_INSTANCES) {
        if (line[0] == '#') {
            continue;
        }
        
        line[strcspn(line, "\n")] = 0;
        
        if (strlen(line) == 0) {
            continue;
        }
        
        double f[NUM_FEATURES];
        int outcome;
        
        int parsed = sscanf(line, "%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%d",
                           &f[0], &f[1], &f[2], &f[3], &f[4],
                           &f[5], &f[6], &f[7], &f[8], &outcome);
        
        if (parsed != 10) {
            fprintf(stderr, "Warning: Invalid line format\n");
            continue;
        }
        
        for (int i = 0; i < NUM_FEATURES; i++) {
            data[count].features[i] = f[i];
        }
        data[count].label = outcome;
        count++;
    }
    
    fclose(file);
    printf("Loaded %d samples\n", count);
    return count;
}

// Initialize model
void init_model(NaiveBayesModel *model) {
    model->total_samples = 0;
    
    for (int c = 0; c < NUM_CLASSES; c++) {
        model->class_prob[c] = 0.0;
        model->class_count[c] = 0;
    }
    
    for (int f = 0; f < NUM_FEATURES; f++) {
        for (int s = 0; s < NUM_STATES; s++) {
            for (int c = 0; c < NUM_CLASSES; c++) {
                model->feature_prob[f][s][c] = 0.0;
                model->feature_count[f][s][c] = 0;
            }
        }
    }
}

// Train Naive Bayes model
void train_model(NaiveBayesModel *model, Instance *train_data, int train_size) {
    printf("\nTraining Naive Bayes model (Matrix Format)...\n");
    printf("Training samples: %d\n\n", train_size);
    
    init_model(model);
    model->total_samples = train_size;
    
    // Count occurrences
    for (int i = 0; i < train_size; i++) {
        int class_idx = label_to_class(train_data[i].label);
        model->class_count[class_idx]++;
        
        for (int f = 0; f < NUM_FEATURES; f++) {
            int state_idx = feature_to_state(train_data[i].features[f]);
            model->feature_count[f][state_idx][class_idx]++;
        }
    }
    
    // Calculate class probabilities: P(class)
    for (int c = 0; c < NUM_CLASSES; c++) {
        model->class_prob[c] = (double)model->class_count[c] / train_size;
    }
    
    // Calculate feature probabilities with Laplace smoothing: P(feature=state | class)
    double alpha = 1.0;  // Laplace smoothing parameter
    
    for (int f = 0; f < NUM_FEATURES; f++) {
        for (int c = 0; c < NUM_CLASSES; c++) {
            double denominator = model->class_count[c] + alpha * NUM_STATES;
            
            for (int s = 0; s < NUM_STATES; s++) {
                double numerator = model->feature_count[f][s][c] + alpha;
                model->feature_prob[f][s][c] = numerator / denominator;
            }
        }
    }
    
    printf("Training completed!\n");
    printf("Class distribution:\n");
    printf("  Win (+1):  %d samples (%.2f%%)\n", 
           model->class_count[0], model->class_prob[0] * 100.0);
    printf("  Lose (-1): %d samples (%.2f%%)\n", 
           model->class_count[1], model->class_prob[1] * 100.0);
}

// Predict class for a single instance
int predict(NaiveBayesModel *model, double *features, double *confidence) {
    double log_prob[NUM_CLASSES];
    
    // Calculate log probabilities for numerical stability
    for (int c = 0; c < NUM_CLASSES; c++) {
        log_prob[c] = log(model->class_prob[c]);
        
        for (int f = 0; f < NUM_FEATURES; f++) {
            int state_idx = feature_to_state(features[f]);
            log_prob[c] += log(model->feature_prob[f][state_idx][c]);
        }
    }
    
    // Find class with maximum probability
    int best_class = (log_prob[0] > log_prob[1]) ? 0 : 1;
    
    // Convert log probabilities to probabilities for confidence
    double max_log = (log_prob[0] > log_prob[1]) ? log_prob[0] : log_prob[1];
    double sum_exp = exp(log_prob[0] - max_log) + exp(log_prob[1] - max_log);
    *confidence = exp(log_prob[best_class] - max_log) / sum_exp;
    
    // Convert class index back to label
    return (best_class == 0) ? 1 : -1;
}

// Evaluate model on test set
double evaluate_model(NaiveBayesModel *model, Instance *test_data, int test_size) {
    int correct = 0;
    int tp = 0, tn = 0, fp = 0, fn = 0;
    
    printf("\nEvaluating model on test set (%d samples)...\n", test_size);
    
    for (int i = 0; i < test_size; i++) {
        double confidence;
        int predicted = predict(model, test_data[i].features, &confidence);
        int actual = test_data[i].label;
        
        if (predicted == actual) {
            correct++;
            if (actual == 1) tp++;
            else tn++;
        } else {
            if (predicted == 1) fp++;
            else fn++;
        }
    }
    
    double accuracy = (double)correct / test_size * 100.0;
    
    printf("\nTest Results:\n");
    printf("  Accuracy: %.2f%% (%d/%d correct)\n", accuracy, correct, test_size);
    printf("\nConfusion Matrix:\n");
    printf("                Predicted\n");
    printf("              Win    Lose\n");
    printf("  Actual Win  %4d   %4d\n", tp, fn);
    printf("       Lose   %4d   %4d\n", fp, tn);
    
    if (tp + fp > 0) {
        printf("\nPrecision (Win): %.2f%%\n", (double)tp / (tp + fp) * 100.0);
    }
    if (tp + fn > 0) {
        printf("Recall (Win):    %.2f%%\n", (double)tp / (tp + fn) * 100.0);
    }
    if (tp + fp > 0 && tp + fn > 0) {
        double precision = (double)tp / (tp + fp);
        double recall = (double)tp / (tp + fn);
        double f1 = 2 * (precision * recall) / (precision + recall);
        printf("F1-Score (Win):  %.2f%%\n", f1 * 100.0);
    }
    
    return accuracy;
}

// Save model to text file
void save_model(NaiveBayesModel *model, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Error: Could not save model to %s\n", filename);
        return;
    }
    
    fprintf(file, "# Naive Bayes Model (Matrix Format)\n");
    fprintf(file, "# Binary classification: +1 (win), -1 (lose)\n");
    fprintf(file, "# Feature states: 0=x(1.0), 1=b(0.0), 2=o(-1.0)\n\n");
    
    fprintf(file, "TOTAL_SAMPLES %d\n\n", model->total_samples);
    
    fprintf(file, "CLASS_PROBABILITIES\n");
    for (int c = 0; c < NUM_CLASSES; c++) {
        fprintf(file, "%d %.10f %d\n", c, model->class_prob[c], model->class_count[c]);
    }
    fprintf(file, "\n");
    
    fprintf(file, "FEATURE_PROBABILITIES\n");
    for (int f = 0; f < NUM_FEATURES; f++) {
        for (int s = 0; s < NUM_STATES; s++) {
            for (int c = 0; c < NUM_CLASSES; c++) {
                fprintf(file, "%d %d %d %.10f %d\n", 
                       f, s, c, 
                       model->feature_prob[f][s][c],
                       model->feature_count[f][s][c]);
            }
        }
    }
    
    fclose(file);
    printf("\nModel saved to %s\n", filename);
}

// Save model in binary format
void save_model_binary(NaiveBayesModel *model, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        printf("Error: Could not save binary model to %s\n", filename);
        return;
    }
    
    fwrite(model, sizeof(NaiveBayesModel), 1, file);
    fclose(file);
    printf("Binary model saved to %s\n", filename);
}

// Load model from text file
int load_model(NaiveBayesModel *model, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not load model from %s\n", filename);
        return 0;
    }
    
    char line[256];
    
    // Skip comments
    while (fgets(line, sizeof(line), file)) {
        if (line[0] != '#') break;
    }
    
    // Read total samples
    sscanf(line, "TOTAL_SAMPLES %d", &model->total_samples);
    
    // Skip to class probabilities
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "CLASS_PROBABILITIES")) break;
    }
    
    // Read class probabilities
    for (int c = 0; c < NUM_CLASSES; c++) {
        fgets(line, sizeof(line), file);
        int idx;
        sscanf(line, "%d %lf %d", &idx, &model->class_prob[c], &model->class_count[c]);
    }
    
    // Skip to feature probabilities
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "FEATURE_PROBABILITIES")) break;
    }
    
    // Read feature probabilities
    while (fgets(line, sizeof(line), file)) {
        int f, s, c, count;
        double prob;
        if (sscanf(line, "%d %d %d %lf %d", &f, &s, &c, &prob, &count) == 5) {
            model->feature_prob[f][s][c] = prob;
            model->feature_count[f][s][c] = count;
        }
    }
    
    fclose(file);
    return 1;
}

int main(int argc, char *argv[]) {
    NaiveBayesModel model;
    Instance train_data[MAX_INSTANCES];
    Instance test_data[MAX_INSTANCES];
    
    printf("========================================\n");
    printf("NAIVE BAYES - MATRIX FORMAT\n");
    printf("Binary Classification (Win/Lose)\n");
    printf("========================================\n\n");
    
    // Default file paths
    const char *train_file = "../../dataset/new processed/train_combined_matrix.data";
    const char *test_file = "../../dataset/new processed/test_combined_matrix.data";
    const char *model_file = "naive_bayes_model.txt";
    const char *model_bin = "naive_bayes_model.bin";
    
    // Allow custom file paths
    if (argc > 1) {
        train_file = argv[1];
    }
    if (argc > 2) {
        test_file = argv[2];
    }
    
    // Load training data
    int train_size = load_matrix_data(train_file, train_data);
    if (train_size == 0) {
        printf("Error: No training data loaded\n");
        return 1;
    }
    
    // Load testing data
    int test_size = load_matrix_data(test_file, test_data);
    if (test_size == 0) {
        printf("Error: No testing data loaded\n");
        return 1;
    }
    
    // Train model
    train_model(&model, train_data, train_size);
    
    // Evaluate model
    double accuracy = evaluate_model(&model, test_data, test_size);
    
    // Save model
    save_model(&model, model_file);
    save_model_binary(&model, model_bin);
    
    printf("\n========================================\n");
    printf("TRAINING SUMMARY\n");
    printf("========================================\n");
    printf("Training samples: %d\n", train_size);
    printf("Testing samples:  %d\n", test_size);
    printf("Test accuracy:    %.2f%%\n", accuracy);
    printf("Model files:      %s\n", model_file);
    printf("                  %s\n", model_bin);
    printf("========================================\n");
    
    return 0;
}
