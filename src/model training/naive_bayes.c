#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_FEATURES 9
#define MAX_STATES 10
#define MAX_LABELS 10
#define MAX_INSTANCES 1000
#define MAX_LINE_LENGTH 256
#define MAX_FEATURE_LENGTH 32

typedef struct {
    char features[MAX_FEATURES][MAX_FEATURE_LENGTH];
    char label[MAX_FEATURE_LENGTH];
} Instance;

typedef struct {
    char state[MAX_FEATURE_LENGTH];
    char label[MAX_FEATURE_LENGTH];
    double probability;
} FeatureProbability;

typedef struct {
    char label[MAX_FEATURE_LENGTH];
    double probability;
} LabelProbability;

typedef struct {
    FeatureProbability feature_probs[MAX_FEATURES][MAX_STATES * MAX_LABELS];
    int feature_count[MAX_FEATURES];
    LabelProbability label_probs[MAX_LABELS];
    int label_count;
} Model;

// Function to shuffle data
void shuffle_data(Instance *data, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Instance temp = data[i];
        data[i] = data[j];
        data[j] = temp;
    }
}

// Function to find or add label probability
int find_label_index(Model *model, const char *label) {
    for (int i = 0; i < model->label_count; i++) {
        if (strcmp(model->label_probs[i].label, label) == 0) {
            return i;
        }
    }
    strcpy(model->label_probs[model->label_count].label, label);
    model->label_probs[model->label_count].probability = 0.0;
    return model->label_count++;
}

// Function to find or add feature probability
int find_feature_prob_index(Model *model, int feature_idx, const char *state, const char *label) {
    for (int i = 0; i < model->feature_count[feature_idx]; i++) {
        if (strcmp(model->feature_probs[feature_idx][i].state, state) == 0 &&
            strcmp(model->feature_probs[feature_idx][i].label, label) == 0) {
            return i;
        }
    }
    strcpy(model->feature_probs[feature_idx][model->feature_count[feature_idx]].state, state);
    strcpy(model->feature_probs[feature_idx][model->feature_count[feature_idx]].label, label);
    model->feature_probs[feature_idx][model->feature_count[feature_idx]].probability = 0.0;
    return model->feature_count[feature_idx]++;
}

// Learn function - trains the Naive Bayes model
void learn(Instance *data, int data_size, Model *model) {
    // Initialize model
    model->label_count = 0;
    for (int i = 0; i < MAX_FEATURES; i++) {
        model->feature_count[i] = 0;
    }
    
    // Count occurrences
    for (int i = 0; i < data_size; i++) {
        int label_idx = find_label_index(model, data[i].label);
        model->label_probs[label_idx].probability += 1.0;
        
        for (int j = 0; j < MAX_FEATURES; j++) {
            int feat_idx = find_feature_prob_index(model, j, data[i].features[j], data[i].label);
            model->feature_probs[j][feat_idx].probability += 1.0;
        }
    }
    
    // Normalize feature probabilities
    for (int i = 0; i < MAX_FEATURES; i++) {
        for (int j = 0; j < model->feature_count[i]; j++) {
            const char *label = model->feature_probs[i][j].label;
            for (int k = 0; k < model->label_count; k++) {
                if (strcmp(model->label_probs[k].label, label) == 0) {
                    model->feature_probs[i][j].probability /= model->label_probs[k].probability;
                    break;
                }
            }
        }
    }
    
    // Normalize label probabilities
    for (int i = 0; i < model->label_count; i++) {
        model->label_probs[i].probability /= data_size;
    }
}

// Predict function - predicts the label for a new instance
void predict(Model *model, char features[MAX_FEATURES][MAX_FEATURE_LENGTH], 
             char *best_label, double *best_prob) {
    double max_prob = -1.0;
    
    for (int i = 0; i < model->label_count; i++) {
        double prob = model->label_probs[i].probability;
        
        for (int j = 0; j < MAX_FEATURES; j++) {
            for (int k = 0; k < model->feature_count[j]; k++) {
                if (strcmp(model->feature_probs[j][k].state, features[j]) == 0 &&
                    strcmp(model->feature_probs[j][k].label, model->label_probs[i].label) == 0) {
                    prob *= model->feature_probs[j][k].probability;
                    break;
                }
            }
        }
        
        if (prob > max_prob) {
            max_prob = prob;
            strcpy(best_label, model->label_probs[i].label);
        }
    }
    
    *best_prob = max_prob;
}

// Save model to binary file
int saveModelBinary(const char *filename, Model *model) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        printf("Error: Could not create file %s\n", filename);
        return 0;
    }
    
    // Write the entire model struct to file
    fwrite(model, sizeof(Model), 1, fp);
    fclose(fp);
    
    printf("Model saved to %s (binary format)\n", filename);
    return 1;
}

// Load model from binary file
int loadModelBinary(const char *filename, Model *model) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        printf("Error: Could not open file %s\n", filename);
        return 0;
    }
    
    // Read the model struct from file
    fread(model, sizeof(Model), 1, fp);
    fclose(fp);
    
    printf("Model loaded from %s (binary format)\n", filename);
    return 1;
}

// Save model to human-readable text file
int saveModelText(const char *filename, Model *model) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        printf("Error: Could not create file %s\n", filename);
        return 0;
    }
    
    fprintf(fp, "========================================\n");
    fprintf(fp, "NAIVE BAYES MODEL\n");
    fprintf(fp, "========================================\n\n");
    
    fprintf(fp, "LABEL PROBABILITIES\n");
    fprintf(fp, "----------------------------------------\n");
    fprintf(fp, "Total labels: %d\n\n", model->label_count);
    for (int i = 0; i < model->label_count; i++) {
        fprintf(fp, "Label: %-10s P(Label) = %.6f\n", 
                model->label_probs[i].label, 
                model->label_probs[i].probability);
    }
    
    fprintf(fp, "\n\nFEATURE PROBABILITIES\n");
    fprintf(fp, "========================================\n");
    
    for (int i = 0; i < MAX_FEATURES; i++) {
        if (model->feature_count[i] == 0) continue;
        
        fprintf(fp, "\nFeature %d: (%d unique state-label pairs)\n", 
                i, model->feature_count[i]);
        fprintf(fp, "----------------------------------------\n");
        
        for (int j = 0; j < model->feature_count[i]; j++) {
            fprintf(fp, "  State=%-5s | Label=%-10s | P(State|Label) = %.6f\n",
                    model->feature_probs[i][j].state,
                    model->feature_probs[i][j].label,
                    model->feature_probs[i][j].probability);
        }
    }
    
    fprintf(fp, "\n========================================\n");
    fprintf(fp, "END OF MODEL\n");
    fprintf(fp, "========================================\n");
    
    fclose(fp);
    printf("Model saved to %s (text format)\n", filename);
    return 1;
}

// Load model from text file
int loadModelText(const char *filename, Model *model) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("Error: Could not open file %s\n", filename);
        return 0;
    }
    
    char line[256];
    model->label_count = 0;
    for (int i = 0; i < MAX_FEATURES; i++) {
        model->feature_count[i] = 0;
    }
    
    // Skip header lines until we find "Total labels:"
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, "Total labels:") != NULL) {
            sscanf(line, "Total labels: %d", &model->label_count);
            fgets(line, sizeof(line), fp); // Skip blank line
            break;
        }
    }
    
    // Read label probabilities
    for (int i = 0; i < model->label_count; i++) {
        if (fgets(line, sizeof(line), fp)) {
            sscanf(line, "Label: %s P(Label) = %lf", 
                   model->label_probs[i].label, 
                   &model->label_probs[i].probability);
        }
    }
    
    // Read feature probabilities
    int current_feature = -1;
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, "Feature") != NULL && strstr(line, ":") != NULL) {
            int count;
            sscanf(line, "Feature %d: (%d unique", &current_feature, &count);
            model->feature_count[current_feature] = 0;
        } else if (strstr(line, "State=") != NULL && current_feature >= 0) {
            int idx = model->feature_count[current_feature];
            sscanf(line, "  State=%s | Label=%s | P(State|Label) = %lf",
                   model->feature_probs[current_feature][idx].state,
                   model->feature_probs[current_feature][idx].label,
                   &model->feature_probs[current_feature][idx].probability);
            model->feature_count[current_feature]++;
        }
    }
    
    fclose(fp);
    printf("Model loaded from %s (text format)\n", filename);
    return 1;
}

// Cross-validation function
void cross_validate(Instance *data, int data_size, double *accuracy) {
    int part_size = data_size / 6;
    
    for (int fold = 0; fold < 6; fold++) {
        int test_start = fold * part_size;
        int test_end = (fold + 1) * part_size;
        
        // Create train set
        Instance train[MAX_INSTANCES];
        int train_size = 0;
        for (int i = 0; i < data_size; i++) {
            if (i < test_start || i >= test_end) {
                train[train_size++] = data[i];
            }
        }
        
        // Train model
        Model model;
        learn(train, train_size, &model);
        
        // Test model
        int correct = 0;
        for (int i = test_start; i < test_end; i++) {
            char predicted_label[MAX_FEATURE_LENGTH];
            double prob;
            predict(&model, data[i].features, predicted_label, &prob);
            
            if (strcmp(predicted_label, data[i].label) == 0) {
                correct++;
            }
        }
        
        accuracy[fold] = (double)correct / part_size;
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
        line[strcspn(line, "\n")] = 0; // Remove newline
        
        char *token = strtok(line, ",");
        int feature_idx = 0;
        
        while (token != NULL && feature_idx < MAX_FEATURES) {
            strcpy(data[count].features[feature_idx], token);
            token = strtok(NULL, ",");
            feature_idx++;
        }
        
        if (token != NULL) {
            strcpy(data[count].label, token);
        }
        
        count++;
    }
    
    fclose(file);
    return count;
}

int main() {
    srand(time(NULL));
    
    Instance train_data[MAX_INSTANCES];
    Instance test_data[MAX_INSTANCES];
    
    int train_size = load_data("train.data", train_data);
    int test_size = load_data("test.data", test_data);
    
    if (train_size == 0) {
        printf("No training data loaded.\n");
        return 1;
    }
    
    if (test_size == 0) {
        printf("No test data loaded.\n");
        return 1;
    }
    
    printf("Loaded %d training instances\n", train_size);
    printf("Loaded %d test instances\n", test_size);
    
    // Train model
    printf("\nTraining model...\n");
    Model model;
    learn(train_data, train_size, &model);
    printf("Training complete!\n");
    
    // Save model in both formats
    printf("\nSaving model...\n");
    saveModelBinary("model.bin", &model);
    saveModelText("model.txt", &model);
    
    // Print label probabilities
    printf("\nLabel probabilities:\n");
    for (int i = 0; i < model.label_count; i++) {
        printf("  %s: %.4f\n", model.label_probs[i].label, model.label_probs[i].probability);
    }
    
    // Test on test data
    int correct = 0;
    printf("\nTesting on test data:\n");
    for (int i = 0; i < test_size; i++) {
        char predicted_label[MAX_FEATURE_LENGTH];
        double prob;
        predict(&model, test_data[i].features, predicted_label, &prob);
        
        if (strcmp(predicted_label, test_data[i].label) == 0) {
            correct++;
        }
        
        // Print first 5 predictions as examples
        if (i < 5) {
            printf("  Instance %d: Actual=%s, Predicted=%s, Prob=%.6f %s\n", 
                   i + 1, test_data[i].label, predicted_label, prob,
                   strcmp(predicted_label, test_data[i].label) == 0 ? "✓" : "✗");
        }
    }
    
    double test_accuracy = (double)correct / test_size;
    printf("\nTest Accuracy: %.4f (%d/%d correct)\n", test_accuracy, correct, test_size);
    
    // Cross-validation on training data
    printf("\nPerforming 6-fold cross-validation on training data:\n");
    double accuracy[6];
    cross_validate(train_data, train_size, accuracy);
    
    double sum = 0.0;
    for (int i = 0; i < 6; i++) {
        printf("  Fold %d: %.4f\n", i + 1, accuracy[i]);
        sum += accuracy[i];
    }
    printf("Average CV Accuracy: %.4f\n", sum / 6);
    
    // Demonstrate loading model from file
    printf("\n========================================\n");
    printf("Demonstrating model loading...\n");
    printf("========================================\n");
    Model loaded_model;
    if (loadModelBinary("model.bin", &loaded_model)) {
        printf("Successfully loaded model from binary file\n");
        printf("Loaded model has %d labels\n", loaded_model.label_count);
    }
    
    return 0;
}