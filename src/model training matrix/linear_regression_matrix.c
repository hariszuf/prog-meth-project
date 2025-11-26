#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define MAX_INSTANCES 10000
#define NUM_FEATURES 10
#define MAX_LINE_LENGTH 256

typedef struct {
    double features[NUM_FEATURES];
    double label;
} Instance;

typedef struct {
    double weights[NUM_FEATURES];
} LinearModel;
int load_matrix_data(const char *filename, Instance *data) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file: %s\n", filename);
        return 0;
    }
    
    char line[MAX_LINE_LENGTH];
    int count = 0;
    
    printf("Loading matrix format data from %s...\n", filename);
    
    while (fgets(line, sizeof(line), file) && count < MAX_INSTANCES) {
        if (line[0] == '#') {
            continue;
        }
        
        line[strcspn(line, "\n")] = 0;  // Remove newline
        
        if (strlen(line) == 0) {
            continue;
        }
        
        double f[9];
        int outcome;
        
        int parsed = sscanf(line, "%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%d",
                           &f[0], &f[1], &f[2], &f[3], &f[4],
                           &f[5], &f[6], &f[7], &f[8], &outcome);
        
        if (parsed != 10) {
            fprintf(stderr, "Warning: Invalid line format (expected 10 values, got %d)\n", parsed);
            continue;
        }
        
        data[count].features[0] = 1.0;
        for (int i = 0; i < 9; i++) {
            data[count].features[i + 1] = f[i];
        }
        data[count].label = (double)outcome;
        
        count++;
    }
    
    fclose(file);
    printf("Loaded %d samples\n", count);
    return count;
}

// Compute prediction (dot product of weights and features)
double predict(const LinearModel *model, const double *features) {
    double result = 0.0;
    for (int i = 0; i < NUM_FEATURES; i++) {
        result += model->weights[i] * features[i];
    }
    return result;
}

// Mean Squared Error loss
double compute_mse(const LinearModel *model, Instance *data, int data_size) {
    double total_error = 0.0;
    for (int i = 0; i < data_size; i++) {
        double pred = predict(model, data[i].features);
        double error = pred - data[i].label;
        total_error += error * error;
    }
    return total_error / data_size;
}

// Train using gradient descent
void train_model(LinearModel *model, Instance *train_data, int train_size, 
                 int epochs, double learning_rate) {
    // Initialize weights to small random values
    srand(time(NULL));
    for (int i = 0; i < NUM_FEATURES; i++) {
        model->weights[i] = ((double)rand() / RAND_MAX - 0.5) * 0.1;
    }
    
    printf("\nTraining linear regression model (Matrix Format)...\n");
    printf("Epochs: %d, Learning rate: %.4f\n", epochs, learning_rate);
    printf("Training samples: %d\n\n", train_size);
    
    // Training loop
    for (int epoch = 0; epoch < epochs; epoch++) {
        double total_loss = 0.0;
        
        // Batch gradient descent
        double gradients[NUM_FEATURES] = {0};
        
        for (int i = 0; i < train_size; i++) {
            double pred = predict(model, train_data[i].features);
            double error = pred - train_data[i].label;
            
            // Accumulate gradients
            for (int j = 0; j < NUM_FEATURES; j++) {
                gradients[j] += error * train_data[i].features[j];
            }
            
            total_loss += error * error;
        }
        
        // Update weights
        for (int j = 0; j < NUM_FEATURES; j++) {
            model->weights[j] -= learning_rate * gradients[j] / train_size;
        }
        
        // Print progress every 100 epochs
        if ((epoch + 1) % 100 == 0 || epoch == 0) {
            double mse = total_loss / train_size;
            printf("Epoch %4d: MSE = %.6f\n", epoch + 1, mse);
        }
    }
    
    printf("\nTraining completed!\n");
}

// Evaluate model on test set
double evaluate_model(const LinearModel *model, Instance *test_data, int test_size) {
    int correct = 0;
    int total = test_size;
    
    printf("\nEvaluating model on test set (%d samples)...\n", test_size);
    
    // Confusion matrix
    int tp = 0, tn = 0, fp = 0, fn = 0;
    
    for (int i = 0; i < test_size; i++) {
        double pred_value = predict(model, test_data[i].features);
        int predicted = (pred_value > 0.0) ? 1 : -1;
        int actual = (int)test_data[i].label;
        
        if (predicted == actual) {
            correct++;
            if (actual == 1) tp++;
            else tn++;
        } else {
            if (predicted == 1) fp++;
            else fn++;
        }
    }
    
    double accuracy = (double)correct / total * 100.0;
    
    printf("\nTest Results:\n");
    printf("  Accuracy: %.2f%% (%d/%d correct)\n", accuracy, correct, total);
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
    
    return accuracy;
}

// Save model to file
void save_model(const LinearModel *model, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Error: Could not save model to %s\n", filename);
        return;
    }
    
    fprintf(file, "# Linear Regression Model (Matrix Format)\n");
    fprintf(file, "# Trained on binary classification data (+1 win, -1 lose)\n");
    fprintf(file, "# Format: 10 weights (bias + 9 features)\n");
    
    for (int i = 0; i < NUM_FEATURES; i++) {
        fprintf(file, "%.10f\n", model->weights[i]);
    }
    
    fclose(file);
    printf("\nModel saved to %s\n", filename);
}

// Save model in binary format for faster loading
void save_model_binary(const LinearModel *model, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        printf("Error: Could not save binary model to %s\n", filename);
        return;
    }
    
    fwrite(model->weights, sizeof(double), NUM_FEATURES, file);
    fclose(file);
    printf("Binary model saved to %s\n", filename);
}

int main(int argc, char *argv[]) {
    LinearModel model;
    Instance train_data[MAX_INSTANCES];
    Instance test_data[MAX_INSTANCES];
    
    printf("========================================\n");
    printf("LINEAR REGRESSION - MATRIX FORMAT\n");
    printf("Binary Classification (Win/Lose)\n");
    printf("========================================\n\n");
    
    // Default file paths
    const char *train_file = "../../dataset/new processed/train_combined_matrix.data";
    const char *test_file = "../../dataset/new processed/test_combined_matrix.data";
    const char *model_file = "linear_regression_model.txt";
    const char *model_bin = "linear_regression_model.bin";
    
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
    
    // Training parameters
    int epochs = 1000;
    double learning_rate = 0.01;
    
    // Train model
    train_model(&model, train_data, train_size, epochs, learning_rate);
    
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
