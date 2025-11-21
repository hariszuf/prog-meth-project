/*
 * dataset_processor_matrix.c
 * 
 * Matrix-based dataset processor following strict academic specification:
 * - Features stored as 2D array: X[m][n] where m=rows, n=9 features
 * - Outcomes stored as 1D array: y[m] where values are +1 (win) or -1 (lose)
 * - Draws are excluded from training data (binary classification only)
 * 
 * DIFFERENCES FROM dataset_processor.c:
 * 1. Uses proper matrix format instead of array of structures
 * 2. Binary classification only (win/lose) - excludes draws
 * 3. Numerical encoding: +1 for win, -1 for lose (instead of 'w', 'l', 'd')
 * 4. Direct matrix operations possible (suitable for linear algebra)
 * 
 * IMPACT ON MODELS:
 * - Better suited for mathematical ML algorithms
 * - More efficient memory access patterns
 * - Cleaner separation of features and labels
 * - Standard format for most ML libraries
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_SAMPLES 10000
#define FEATURES 9

// Matrix-based dataset structure
typedef struct {
    double **X;      // 2D array: X[m][n] - features (m samples, n=9 features)
    int *y;          // 1D array: y[m] - outcomes (+1 win, -1 lose)
    int num_samples; // m - number of samples
    int num_features;// n - number of features (always 9)
} MatrixDataset;

// Function prototypes
void initMatrixDataset(MatrixDataset *dataset, int max_samples);
void freeMatrixDataset(MatrixDataset *dataset);
int readDatasetToMatrix(const char *filename, MatrixDataset *dataset);
void shuffleMatrix(MatrixDataset *dataset);
void splitMatrix(MatrixDataset *full, MatrixDataset *train, MatrixDataset *test, double train_ratio);
int saveMatrixDataset(const char *filename, MatrixDataset *dataset);
int saveMatrixReport(const char *filename, MatrixDataset *full, MatrixDataset *train, MatrixDataset *test);
void displayMatrixSample(MatrixDataset *dataset, int index);
double encodeFeature(char c);
char decodeFeature(double val);

// Encode feature to numerical value
// x -> 1.0, o -> -1.0, b -> 0.0
double encodeFeature(char c) {
    switch(c) {
        case 'x': return 1.0;
        case 'o': return -1.0;
        case 'b': return 0.0;
        default: return 0.0;
    }
}

// Decode numerical value back to character (for display)
char decodeFeature(double val) {
    if (val > 0.5) return 'x';
    if (val < -0.5) return 'o';
    return 'b';
}

// Initialize matrix dataset with dynamic memory allocation
void initMatrixDataset(MatrixDataset *dataset, int max_samples) {
    dataset->num_features = FEATURES;
    dataset->num_samples = 0;
    
    // Allocate 2D array for features X[m][n]
    dataset->X = (double **)malloc(max_samples * sizeof(double *));
    if (dataset->X == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for feature matrix\n");
        exit(1);
    }
    
    for (int i = 0; i < max_samples; i++) {
        dataset->X[i] = (double *)malloc(FEATURES * sizeof(double));
        if (dataset->X[i] == NULL) {
            fprintf(stderr, "Error: Memory allocation failed for feature row %d\n", i);
            exit(1);
        }
    }
    
    // Allocate 1D array for outcomes y[m]
    dataset->y = (int *)malloc(max_samples * sizeof(int));
    if (dataset->y == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for outcome vector\n");
        exit(1);
    }
    
    printf("Matrix dataset initialized: X[%d][%d], y[%d]\n", 
           max_samples, FEATURES, max_samples);
}

// Free matrix dataset memory
void freeMatrixDataset(MatrixDataset *dataset) {
    if (dataset->X != NULL) {
        // Free each row
        for (int i = 0; i < MAX_SAMPLES; i++) {
            if (dataset->X[i] != NULL) {
                free(dataset->X[i]);
            }
        }
        free(dataset->X);
        dataset->X = NULL;
    }
    
    if (dataset->y != NULL) {
        free(dataset->y);
        dataset->y = NULL;
    }
    
    dataset->num_samples = 0;
}

// Read dataset file and convert to matrix format
// Extracts (x_{m,1}, x_{m,2}, ..., x_{m,9}, y_{m,1}) into X[m][n] and y[m]
int readDatasetToMatrix(const char *filename, MatrixDataset *dataset) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error: Could not open file %s\n", filename);
        return 0;
    }
    
    char line[256];
    int line_num = 0;
    int valid_samples = 0;
    int skipped_draws = 0;
    
    printf("\nReading dataset into matrix format...\n");
    printf("Format: X[m][n] where m=samples, n=9 features\n");
    printf("        y[m] where values are +1 (win) or -1 (lose)\n\n");
    
    // Read each line from the file
    while (fgets(line, sizeof(line), fp) != NULL && valid_samples < MAX_SAMPLES) {
        line_num++;
        
        // Remove newline character
        line[strcspn(line, "\n")] = 0;
        
        // Skip empty lines
        if (strlen(line) == 0) {
            continue;
        }
        
        // Parse the line (expected format: x,o,b,x,x,o,b,b,x,win)
        char features[FEATURES];
        char outcome_str[10];
        
        char *token = strtok(line, ",");
        int i = 0;
        
        // Read 9 features into temporary array
        while (token != NULL && i < FEATURES) {
            if (strlen(token) != 1) {
                fprintf(stderr, "Warning: Invalid feature at line %d, position %d\n", 
                        line_num, i + 1);
                break;
            }
            
            char feature = token[0];
            if (feature != 'x' && feature != 'o' && feature != 'b') {
                fprintf(stderr, "Warning: Invalid feature value '%c' at line %d, position %d\n", 
                        feature, line_num, i + 1);
                break;
            }
            
            features[i] = feature;
            token = strtok(NULL, ",");
            i++;
        }
        
        // Check if we read exactly 9 features
        if (i != FEATURES) {
            fprintf(stderr, "Warning: Line %d has %d features (expected %d), skipping\n", 
                    line_num, i, FEATURES);
            continue;
        }
        
        // Read the outcome (last token)
        if (token == NULL) {
            fprintf(stderr, "Warning: Missing outcome at line %d, skipping\n", line_num);
            continue;
        }
        
        // CRITICAL: Binary classification only - skip draws
        int outcome;
        if (strcmp(token, "win") == 0) {
            outcome = +1;  // Positive class
        } else if (strcmp(token, "lose") == 0) {
            outcome = -1;  // Negative class
        } else if (strcmp(token, "draw") == 0) {
            skipped_draws++;
            continue;  // Skip draws for binary classification
        } else {
            fprintf(stderr, "Warning: Invalid outcome '%s' at line %d, skipping\n", 
                    token, line_num);
            continue;
        }
        
        // Convert features to numerical matrix format
        // x_{m,n} where n=1..9 (square positions)
        for (int n = 0; n < FEATURES; n++) {
            dataset->X[valid_samples][n] = encodeFeature(features[n]);
        }
        
        // Store outcome: y_{m,1}
        dataset->y[valid_samples] = outcome;
        
        valid_samples++;
    }
    
    fclose(fp);
    dataset->num_samples = valid_samples;
    
    printf("Successfully loaded %d samples from %s\n", valid_samples, filename);
    printf("  - Win samples: stored as y[m] = +1\n");
    printf("  - Lose samples: stored as y[m] = -1\n");
    printf("  - Draw samples: %d (excluded from binary classification)\n", skipped_draws);
    printf("\nMatrix dimensions: X[%d][%d], y[%d]\n", 
           valid_samples, FEATURES, valid_samples);
    
    return 1;
}

// Shuffle matrix dataset using Fisher-Yates algorithm
void shuffleMatrix(MatrixDataset *dataset) {
    srand(time(NULL));
    
    for (int i = dataset->num_samples - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        
        // Swap rows in feature matrix X
        double *temp_row = dataset->X[i];
        dataset->X[i] = dataset->X[j];
        dataset->X[j] = temp_row;
        
        // Swap corresponding outcomes in y
        int temp_y = dataset->y[i];
        dataset->y[i] = dataset->y[j];
        dataset->y[j] = temp_y;
    }
    
    printf("Matrix dataset shuffled randomly\n");
}

// Split matrix dataset into training and testing sets
void splitMatrix(MatrixDataset *full, MatrixDataset *train, MatrixDataset *test, double train_ratio) {
    int train_size = (int)(full->num_samples * train_ratio);
    int test_size = full->num_samples - train_size;
    
    // Initialize train and test datasets
    initMatrixDataset(train, train_size);
    initMatrixDataset(test, test_size);
    
    train->num_samples = train_size;
    test->num_samples = test_size;
    
    // Copy first train_ratio% to training set
    for (int i = 0; i < train_size; i++) {
        // Copy feature row
        for (int j = 0; j < FEATURES; j++) {
            train->X[i][j] = full->X[i][j];
        }
        // Copy outcome
        train->y[i] = full->y[i];
    }
    
    // Copy remaining to testing set
    for (int i = 0; i < test_size; i++) {
        // Copy feature row
        for (int j = 0; j < FEATURES; j++) {
            test->X[i][j] = full->X[train_size + i][j];
        }
        // Copy outcome
        test->y[i] = full->y[train_size + i];
    }
    
    printf("\nMatrix dataset split:\n");
    printf("  Training set: X_train[%d][%d], y_train[%d] (%.1f%%)\n", 
           train_size, FEATURES, train_size, train_ratio * 100);
    printf("  Testing set:  X_test[%d][%d], y_test[%d] (%.1f%%)\n", 
           test_size, FEATURES, test_size, (1.0 - train_ratio) * 100);
}

// Display a single sample from matrix dataset
void displayMatrixSample(MatrixDataset *dataset, int index) {
    if (index < 0 || index >= dataset->num_samples) {
        fprintf(stderr, "Error: Invalid index %d\n", index);
        return;
    }
    
    printf("\nSample %d:\n", index + 1);
    printf("Features X[%d][n]:\n", index);
    
    // Display as tic-tac-toe board
    for (int i = 0; i < 9; i++) {
        printf(" %c ", decodeFeature(dataset->X[index][i]));
        if ((i + 1) % 3 == 0) {
            printf("  (row %d)\n", i/3 + 1);
            if (i < 8) printf("-----------\n");
        } else {
            printf("|");
        }
    }
    
    printf("Outcome y[%d]: %s (%+d)\n", index, 
           dataset->y[index] == 1 ? "Win" : "Lose",
           dataset->y[index]);
}

// Save matrix dataset to file
int saveMatrixDataset(const char *filename, MatrixDataset *dataset) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        fprintf(stderr, "Error: Could not create file %s\n", filename);
        return 0;
    }
    
    // Write header comment
    fprintf(fp, "# Matrix dataset format: x1,x2,x3,x4,x5,x6,x7,x8,x9,outcome\n");
    fprintf(fp, "# Features encoded as: x=1.0, o=-1.0, b=0.0\n");
    fprintf(fp, "# Outcomes: win=+1, lose=-1\n");
    
    for (int m = 0; m < dataset->num_samples; m++) {
        // Write features X[m][n]
        for (int n = 0; n < FEATURES; n++) {
            fprintf(fp, "%.1f", dataset->X[m][n]);
            if (n < FEATURES - 1) fprintf(fp, ",");
        }
        // Write outcome y[m]
        fprintf(fp, ",%+d\n", dataset->y[m]);
    }
    
    fclose(fp);
    printf("Successfully saved %d samples to %s\n", dataset->num_samples, filename);
    return 1;
}

// Save comprehensive statistics report
int saveMatrixReport(const char *filename, MatrixDataset *full, MatrixDataset *train, MatrixDataset *test) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        fprintf(stderr, "Error: Could not create file %s\n", filename);
        return 0;
    }
    
    // Count outcomes in full dataset
    int full_win = 0, full_lose = 0;
    for (int i = 0; i < full->num_samples; i++) {
        if (full->y[i] == +1) full_win++;
        else if (full->y[i] == -1) full_lose++;
    }
    
    // Count outcomes in training set
    int train_win = 0, train_lose = 0;
    for (int i = 0; i < train->num_samples; i++) {
        if (train->y[i] == +1) train_win++;
        else if (train->y[i] == -1) train_lose++;
    }
    
    // Count outcomes in testing set
    int test_win = 0, test_lose = 0;
    for (int i = 0; i < test->num_samples; i++) {
        if (test->y[i] == +1) test_win++;
        else if (test->y[i] == -1) test_lose++;
    }
    
    // Write report
    fprintf(fp, "========================================\n");
    fprintf(fp, "MATRIX DATASET PROCESSING REPORT\n");
    fprintf(fp, "Binary Classification (Win/Lose Only)\n");
    fprintf(fp, "========================================\n\n");
    
    fprintf(fp, "DATA FORMAT\n");
    fprintf(fp, "----------------------------------------\n");
    fprintf(fp, "Feature Matrix: X[m][n] where m=samples, n=9\n");
    fprintf(fp, "Outcome Vector: y[m] with values {+1, -1}\n");
    fprintf(fp, "Encoding:\n");
    fprintf(fp, "  Features: x=+1.0, o=-1.0, b=0.0\n");
    fprintf(fp, "  Outcomes: win=+1, lose=-1\n\n");
    
    fprintf(fp, "FULL DATASET STATISTICS\n");
    fprintf(fp, "----------------------------------------\n");
    fprintf(fp, "Total samples: %d\n", full->num_samples);
    fprintf(fp, "Matrix dimensions: X[%d][%d]\n", full->num_samples, full->num_features);
    fprintf(fp, "Win samples (y=+1):  %d (%.2f%%)\n", 
            full_win, (double)full_win/full->num_samples * 100);
    fprintf(fp, "Lose samples (y=-1): %d (%.2f%%)\n\n", 
            full_lose, (double)full_lose/full->num_samples * 100);
    
    fprintf(fp, "TRAINING SET STATISTICS\n");
    fprintf(fp, "----------------------------------------\n");
    fprintf(fp, "Total samples: %d (%.2f%%)\n", 
            train->num_samples, 
            (double)train->num_samples/full->num_samples * 100);
    fprintf(fp, "Matrix dimensions: X_train[%d][%d]\n", 
            train->num_samples, train->num_features);
    fprintf(fp, "Win samples (y=+1):  %d (%.2f%%)\n", 
            train_win, (double)train_win/train->num_samples * 100);
    fprintf(fp, "Lose samples (y=-1): %d (%.2f%%)\n\n", 
            train_lose, (double)train_lose/train->num_samples * 100);
    
    fprintf(fp, "TESTING SET STATISTICS\n");
    fprintf(fp, "----------------------------------------\n");
    fprintf(fp, "Total samples: %d (%.2f%%)\n", 
            test->num_samples, 
            (double)test->num_samples/full->num_samples * 100);
    fprintf(fp, "Matrix dimensions: X_test[%d][%d]\n", 
            test->num_samples, test->num_features);
    fprintf(fp, "Win samples (y=+1):  %d (%.2f%%)\n", 
            test_win, (double)test_win/test->num_samples * 100);
    fprintf(fp, "Lose samples (y=-1): %d (%.2f%%)\n\n", 
            test_lose, (double)test_lose/test->num_samples * 100);
    
    fprintf(fp, "CLASS BALANCE ANALYSIS\n");
    fprintf(fp, "----------------------------------------\n");
    fprintf(fp, "Full dataset balance: %.2f%% win, %.2f%% lose\n",
            (double)full_win/full->num_samples * 100,
            (double)full_lose/full->num_samples * 100);
    fprintf(fp, "Training set balance: %.2f%% win, %.2f%% lose\n",
            (double)train_win/train->num_samples * 100,
            (double)train_lose/train->num_samples * 100);
    fprintf(fp, "Testing set balance:  %.2f%% win, %.2f%% lose\n\n",
            (double)test_win/test->num_samples * 100,
            (double)test_lose/test->num_samples * 100);
    
    fprintf(fp, "ADVANTAGES OF MATRIX FORMAT\n");
    fprintf(fp, "----------------------------------------\n");
    fprintf(fp, "1. Standard ML format (X, y notation)\n");
    fprintf(fp, "2. Direct linear algebra operations\n");
    fprintf(fp, "3. Efficient memory access patterns\n");
    fprintf(fp, "4. Compatible with numerical libraries\n");
    fprintf(fp, "5. Binary classification (no ambiguous draws)\n\n");
    
    fclose(fp);
    return 1;
}

// Main function
int main(int argc, char *argv[]) {
    MatrixDataset fullDataset, trainSet, testSet;
    char input_filename[256];
    char train_filename[256];
    char test_filename[256];
    char report_filename[256];
    double train_ratio = 0.8;  // default 80/20 split
    
    printf("\n");
    printf("========================================\n");
    printf("MATRIX-BASED DATASET PROCESSOR\n");
    printf("Binary Classification (Win/Lose Only)\n");
    printf("========================================\n");
    printf("\n");
    printf("Format: X[m][n] where m=samples, n=9\n");
    printf("        y[m] where values ∈ {+1, -1}\n");
    printf("\n");
    
    // Get input filename
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_file> [train_ratio]\n", argv[0]);
        fprintf(stderr, "Example: %s tic-tac-toe-minimax-complete.data 0.8\n", argv[0]);
        return 1;
    }
    
    strcpy(input_filename, argv[1]);
    
    // Determine output filenames based on input
    if (strstr(input_filename, "non-terminal") != NULL) {
        strcpy(train_filename, "../dataset/new processed/train_non_terminal_matrix.data");
        strcpy(test_filename, "../dataset/new processed/test_non_terminal_matrix.data");
        strcpy(report_filename, "../dataset/new results/report_non_terminal_matrix.txt");
        printf("📊 Processing NON-TERMINAL dataset (matrix format)\n\n");
    } else if (strstr(input_filename, "complete") != NULL) {
        strcpy(train_filename, "../dataset/new processed/train_combined_matrix.data");
        strcpy(test_filename, "../dataset/new processed/test_combined_matrix.data");
        strcpy(report_filename, "../dataset/new results/report_combined_matrix.txt");
        printf("📊 Processing COMBINED dataset (matrix format)\n\n");
    } else {
        strcpy(train_filename, "../dataset/new processed/train_matrix.data");
        strcpy(test_filename, "../dataset/new processed/test_matrix.data");
        strcpy(report_filename, "../dataset/new results/report_matrix.txt");
        printf("📊 Processing dataset (matrix format)\n\n");
    }
    
    // Get train/test split ratio
    if (argc > 2) {
        train_ratio = atof(argv[2]);
        if (train_ratio <= 0 || train_ratio >= 1) {
            fprintf(stderr, "Invalid train ratio: %.2f, using default 0.8\n", train_ratio);
            train_ratio = 0.8;
        }
    }
    
    // Initialize and read dataset into matrix format
    initMatrixDataset(&fullDataset, MAX_SAMPLES);
    
    if (!readDatasetToMatrix(input_filename, &fullDataset)) {
        freeMatrixDataset(&fullDataset);
        return 1;
    }
    
    if (fullDataset.num_samples == 0) {
        fprintf(stderr, "Error: No valid samples loaded\n");
        freeMatrixDataset(&fullDataset);
        return 1;
    }
    
    // Display first few samples BEFORE shuffling
    printf("\n=== SAMPLES BEFORE SHUFFLING ===\n");
    for (int i = 0; i < 3 && i < fullDataset.num_samples; i++) {
        displayMatrixSample(&fullDataset, i);
    }
    
    // CRITICAL: Shuffle the dataset before splitting
    printf("\n*** SHUFFLING MATRIX DATASET ***\n");
    shuffleMatrix(&fullDataset);
    
    // Display first few samples AFTER shuffling
    printf("\n=== SAMPLES AFTER SHUFFLING ===\n");
    for (int i = 0; i < 3 && i < fullDataset.num_samples; i++) {
        displayMatrixSample(&fullDataset, i);
    }
    
    // Split into training and testing sets
    printf("\n*** SPLITTING MATRIX DATASET ***\n");
    splitMatrix(&fullDataset, &trainSet, &testSet, train_ratio);
    
    // Save training set to file
    printf("\nSaving training matrix to %s...\n", train_filename);
    if (!saveMatrixDataset(train_filename, &trainSet)) {
        freeMatrixDataset(&fullDataset);
        freeMatrixDataset(&trainSet);
        freeMatrixDataset(&testSet);
        return 1;
    }
    
    // Save testing set to file
    printf("Saving testing matrix to %s...\n", test_filename);
    if (!saveMatrixDataset(test_filename, &testSet)) {
        freeMatrixDataset(&fullDataset);
        freeMatrixDataset(&trainSet);
        freeMatrixDataset(&testSet);
        return 1;
    }
    
    // Save statistics report
    printf("Generating matrix statistics report...\n");
    if (!saveMatrixReport(report_filename, &fullDataset, &trainSet, &testSet)) {
        freeMatrixDataset(&fullDataset);
        freeMatrixDataset(&trainSet);
        freeMatrixDataset(&testSet);
        return 1;
    }
    
    // Display summary
    printf("\n========================================\n");
    printf("MATRIX PROCESSING COMPLETE\n");
    printf("========================================\n");
    printf("\nFiles created:\n");
    printf("  - %s\n", train_filename);
    printf("  - %s\n", test_filename);
    printf("  - %s\n", report_filename);
    printf("\nMatrix format:\n");
    printf("  Features: X[m][n] = numerical values\n");
    printf("  Outcomes: y[m] = {+1, -1}\n");
    printf("\nKey differences from standard processor:\n");
    printf("  ✓ True matrix format (2D array + 1D array)\n");
    printf("  ✓ Numerical encoding (not characters)\n");
    printf("  ✓ Binary classification (draws excluded)\n");
    printf("  ✓ Standard ML notation (X, y)\n");
    printf("  ✓ Direct linear algebra ready\n");
    
    // Clean up
    freeMatrixDataset(&fullDataset);
    freeMatrixDataset(&trainSet);
    freeMatrixDataset(&testSet);
    
    return 0;
}
