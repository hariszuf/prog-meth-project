#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define MAX_INSTANCES 1000
#define NUM_FEATURES 10  // 9 board positions + 1 bias term
#define MAX_LINE_LENGTH 256

// Instance structure
typedef struct {
    double features[NUM_FEATURES];  // features[0] = bias (1.0), features[1-9] = board state
    double label;  // 1.0 for win, -1.0 for lose, 0.0 for draw
} Instance;

// Model structure
typedef struct {
    double weights[NUM_FEATURES];
} LinearModel;

// Convert board state string to numerical features
void encode_features(const char *board_str, double *features) {
    features[0] = 1.0;  // Bias term
    
    for (int i = 0; i < 9; i++) {
        char cell = board_str[i * 2];  // Skip commas
        
        if (cell == 'x') {
            features[i + 1] = 1.0;
        } else if (cell == 'o') {
            features[i + 1] = -1.0;
        } else {  // 'b' for blank
            features[i + 1] = 0.0;
        }
    }
}

// Convert label string to numerical value
double encode_label(const char *label_str) {
    if (strcmp(label_str, "win") == 0) {
        return 1.0;
    } else if (strcmp(label_str, "lose") == 0) {
        return -1.0;
    } else {  // "draw"
        return 0.0;
    }
}

// Load data from file
int load_data(const char *filename, Instance *data) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file: %s\n", filename);
        return 0;
    }
    
    char line[MAX_LINE_LENGTH];
    int count = 0;
    
    while (fgets(line, sizeof(line), file) && count < MAX_INSTANCES) {
        line[strcspn(line, "\n")] = 0;  // Remove newline
        
        // Split into board state and label
        char *last_comma = strrchr(line, ',');
        if (last_comma) {
            *last_comma = '\0';
            char *label_str = last_comma + 1;
            
            // Encode features and label
            encode_features(line, data[count].features);
            data[count].label = encode_label(label_str);
            count++;
        }
    }
    
    fclose(file);
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

// Train using gradient descent
void train_model(LinearModel *model, Instance *train_data, int train_size, 
                 int epochs, double learning_rate) {
    // Initialize weights to small random values
    srand(time(NULL));
    for (int i = 0; i < NUM_FEATURES; i++) {
        model->weights[i] = ((double)rand() / RAND_MAX - 0.5) * 0.1;
    }
    
    printf("Training linear regression model...\n");
    printf("Epochs: %d, Learning rate: %.4f\n\n", epochs, learning_rate);
    
    for (int epoch = 0; epoch < epochs; epoch++) {
        double total_loss = 0.0;
        
        // Stochastic gradient descent
        for (int i = 0; i < train_size; i++) {
            // Forward pass
            double prediction = predict(model, train_data[i].features);
            double error = train_data[i].label - prediction;
            
            // Backward pass (update weights)
            for (int j = 0; j < NUM_FEATURES; j++) {
                model->weights[j] += learning_rate * error * train_data[i].features[j];
            }
            
            // Accumulate loss (MSE)
            total_loss += error * error;
        }
        
        double mse = total_loss / train_size;
        
        // Print progress every 100 epochs
        if ((epoch + 1) % 100 == 0 || epoch == 0) {
            printf("Epoch %d/%d - MSE: %.6f\n", epoch + 1, epochs, mse);
        }
    }
    
    printf("\nTraining complete!\n\n");
}

// Evaluate model on test set
void evaluate_model(const LinearModel *model, Instance *test_data, int test_size) {
    int correct = 0;
    int win_correct = 0, win_total = 0;
    int lose_correct = 0, lose_total = 0;
    int draw_correct = 0, draw_total = 0;
    
    for (int i = 0; i < test_size; i++) {
        double prediction = predict(model, test_data[i].features);
        
        // Convert continuous prediction to class
        char predicted_class;
        if (prediction > 0.5) {
            predicted_class = 'w';  // win
        } else if (prediction < -0.5) {
            predicted_class = 'l';  // lose
        } else {
            predicted_class = 'd';  // draw
        }
        
        // Convert actual label to class
        char actual_class;
        if (test_data[i].label > 0.5) {
            actual_class = 'w';
            win_total++;
        } else if (test_data[i].label < -0.5) {
            actual_class = 'l';
            lose_total++;
        } else {
            actual_class = 'd';
            draw_total++;
        }
        
        // Count correct predictions
        if (predicted_class == actual_class) {
            correct++;
            if (actual_class == 'w') win_correct++;
            else if (actual_class == 'l') lose_correct++;
            else draw_correct++;
        }
    }
    
    double accuracy = (double)correct / test_size * 100.0;
    
    printf("Test Set Evaluation:\n");
    printf("  Total accuracy: %.2f%% (%d/%d)\n", accuracy, correct, test_size);
    printf("  Win predictions: %.2f%% (%d/%d)\n", 
           win_total > 0 ? (double)win_correct / win_total * 100.0 : 0.0, 
           win_correct, win_total);
    printf("  Lose predictions: %.2f%% (%d/%d)\n", 
           lose_total > 0 ? (double)lose_correct / lose_total * 100.0 : 0.0, 
           lose_correct, lose_total);
    printf("  Draw predictions: %.2f%% (%d/%d)\n", 
           draw_total > 0 ? (double)draw_correct / draw_total * 100.0 : 0.0, 
           draw_correct, draw_total);
}

// Save model to text file
void save_model_text(const char *filename, const LinearModel *model) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        printf("Error: Could not create file %s\n", filename);
        return;
    }
    
    fprintf(fp, "========================================\n");
    fprintf(fp, "LINEAR REGRESSION MODEL\n");
    fprintf(fp, "========================================\n\n");
    
    fprintf(fp, "WEIGHTS (for Tic-Tac-Toe)\n");
    fprintf(fp, "----------------------------------------\n");
    fprintf(fp, "Feature encoding: x=1.0, o=-1.0, blank=0.0\n");
    fprintf(fp, "Output: >0.5=win, <-0.5=lose, else=draw\n\n");
    
    fprintf(fp, "Weight[0] (bias): %.10f\n", model->weights[0]);
    
    const char *positions[] = {
        "top-left", "top-middle", "top-right",
        "middle-left", "center", "middle-right",
        "bottom-left", "bottom-middle", "bottom-right"
    };
    
    for (int i = 0; i < 9; i++) {
        fprintf(fp, "Weight[%d] (%s): %.10f\n", 
                i + 1, positions[i], model->weights[i + 1]);
    }
    
    fprintf(fp, "\n========================================\n");
    fprintf(fp, "END OF MODEL\n");
    fprintf(fp, "========================================\n");
    
    fclose(fp);
    printf("Model saved to %s\n", filename);
}

// Save model to binary file
void save_model_binary(const char *filename, const LinearModel *model) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        printf("Error: Could not create file %s\n", filename);
        return;
    }
    
    fwrite(model, sizeof(LinearModel), 1, fp);
    fclose(fp);
    
    printf("Model saved to %s (binary format)\n", filename);
}

int main() {
    Instance train_data[MAX_INSTANCES];
    Instance test_data[MAX_INSTANCES];
    
    // Load datasets
    printf("Loading datasets...\n");
    int train_size = load_data("../dataset/processed/train.data", train_data);
    int test_size = load_data("../dataset/processed/test.data", test_data);
    
    if (train_size == 0) {
        printf("No training data loaded.\n");
        return 1;
    }
    
    if (test_size == 0) {
        printf("No test data loaded.\n");
        return 1;
    }
    
    printf("Loaded %d training instances\n", train_size);
    printf("Loaded %d test instances\n\n", test_size);
    
    // Train model
    LinearModel model;
    int epochs = 1000;
    double learning_rate = 0.01;
    
    train_model(&model, train_data, train_size, epochs, learning_rate);
    
    // Print learned weights
    printf("Learned weights:\n");
    printf("  Bias: %.4f\n", model.weights[0]);
    printf("  Board weights: ");
    for (int i = 1; i < NUM_FEATURES; i++) {
        printf("%.4f ", model.weights[i]);
    }
    printf("\n\n");
    
    // Evaluate on test set
    evaluate_model(&model, test_data, test_size);
    printf("\n");
    
    // Save model
    save_model_text("../models/linear regression/model.txt", &model);
    save_model_binary("../models/linear regression/model.bin", &model);
    
    printf("\nModel training complete!\n");
    printf("You can now integrate this model into the game.\n");
    
    return 0;
}
