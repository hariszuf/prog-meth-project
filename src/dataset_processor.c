#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOTAL_SAMPLES 958
#define FEATURES 9
#define TRAIN_SIZE 766  // 80% of 958
#define TEST_SIZE 192   // 20% of 958

// Structure to hold a single data sample
typedef struct {
    char features[FEATURES];  // 'x', 'o', or 'b' for each square
    char outcome;             // 'p' for positive (win), 'n' for negative (lose)
} Sample;

// Structure to hold the dataset
typedef struct {
    Sample *data;
    int size;
} Dataset;

// Function to open and read the dataset file
int readDataset(const char *filename, Dataset *dataset) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error: Could not open file %s\n", filename);
        return 0;
    }
    
    // Allocate memory for the dataset
    dataset->data = (Sample *)malloc(TOTAL_SAMPLES * sizeof(Sample));
    if (dataset->data == NULL) {
        printf("Error: Memory allocation failed\n");
        fclose(fp);
        return 0;
    }
    
    dataset->size = 0;
    char line[256];
    
    // Read each line from the file
    while (fgets(line, sizeof(line), fp) != NULL && dataset->size < TOTAL_SAMPLES) {
        // Remove newline character
        line[strcspn(line, "\n")] = 0;
        
        // Parse the line (expected format: x,x,x,x,x,x,x,x,x,positive or negative)
        char *token = strtok(line, ",");
        int i = 0;
        
        // Read 9 features
        while (token != NULL && i < FEATURES) {
            dataset->data[dataset->size].features[i] = token[0];
            token = strtok(NULL, ",");
            i++;
        }
        
        // Read the outcome (last token)
        if (token != NULL) {
            if (strcmp(token, "positive") == 0) {
                dataset->data[dataset->size].outcome = 'p';
            } else if (strcmp(token, "negative") == 0) {
                dataset->data[dataset->size].outcome = 'n';
            }
            dataset->size++;
        }
    }
    
    fclose(fp);
    printf("Successfully loaded %d samples\n", dataset->size);
    return 1;
}

// Function to split dataset into training and testing sets
void splitDataset(Dataset *full, Dataset *train, Dataset *test) {
    // Allocate memory for training set (80%)
    train->data = (Sample *)malloc(TRAIN_SIZE * sizeof(Sample));
    train->size = TRAIN_SIZE;
    
    // Allocate memory for testing set (20%)
    test->data = (Sample *)malloc(TEST_SIZE * sizeof(Sample));
    test->size = TEST_SIZE;
    
    // Copy first 80% to training set
    for (int i = 0; i < TRAIN_SIZE; i++) {
        memcpy(&train->data[i], &full->data[i], sizeof(Sample));
    }
    
    // Copy remaining 20% to testing set
    for (int i = 0; i < TEST_SIZE; i++) {
        memcpy(&test->data[i], &full->data[TRAIN_SIZE + i], sizeof(Sample));
    }
    
    printf("Training set size: %d\n", train->size);
    printf("Testing set size: %d\n", test->size);
}

// Function to print a sample
void printSample(Sample *s) {
    printf("Features: ");
    for (int i = 0; i < FEATURES; i++) {
        printf("%c ", s->features[i]);
        if ((i + 1) % 3 == 0 && i < 8) printf("| ");
    }
    printf("| Outcome: %c\n", s->outcome);
}

// Function to display tic-tac-toe board
void displayBoard(Sample *s) {
    printf("\n");
    for (int i = 0; i < 9; i++) {
        printf(" %c ", s->features[i]);
        if ((i + 1) % 3 == 0) {
            printf("\n");
            if (i < 8) printf("-----------\n");
        } else {
            printf("|");
        }
    }
    printf("Outcome: %s\n\n", s->outcome == 'p' ? "Positive (Win)" : "Negative (Lose)");
}

// Function to save dataset to file
int saveDataset(const char *filename, Dataset *dataset) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("Error: Could not create file %s\n", filename);
        return 0;
    }
    
    for (int i = 0; i < dataset->size; i++) {
        // Write features
        for (int j = 0; j < FEATURES; j++) {
            fprintf(fp, "%c", dataset->data[i].features[j]);
            if (j < FEATURES - 1) fprintf(fp, ",");
        }
        // Write outcome
        fprintf(fp, ",%s\n", dataset->data[i].outcome == 'p' ? "positive" : "negative");
    }
    
    fclose(fp);
    printf("Successfully saved %d samples to %s\n", dataset->size, filename);
    return 1;
}

// Function to save statistics report
int saveReport(const char *filename, Dataset *full, Dataset *train, Dataset *test) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("Error: Could not create file %s\n", filename);
        return 0;
    }
    
    // Count outcomes in full dataset
    int full_positive = 0, full_negative = 0;
    for (int i = 0; i < full->size; i++) {
        if (full->data[i].outcome == 'p') full_positive++;
        else full_negative++;
    }
    
    // Count outcomes in training set
    int train_positive = 0, train_negative = 0;
    for (int i = 0; i < train->size; i++) {
        if (train->data[i].outcome == 'p') train_positive++;
        else train_negative++;
    }
    
    // Count outcomes in testing set
    int test_positive = 0, test_negative = 0;
    for (int i = 0; i < test->size; i++) {
        if (test->data[i].outcome == 'p') test_positive++;
        else test_negative++;
    }
    
    // Write report
    fprintf(fp, "========================================\n");
    fprintf(fp, "TIC-TAC-TOE DATASET PROCESSING REPORT\n");
    fprintf(fp, "========================================\n\n");
    
    fprintf(fp, "FULL DATASET STATISTICS\n");
    fprintf(fp, "----------------------------------------\n");
    fprintf(fp, "Total samples: %d\n", full->size);
    fprintf(fp, "Positive outcomes (X wins): %d (%.2f%%)\n", 
            full_positive, (full_positive * 100.0) / full->size);
    fprintf(fp, "Negative outcomes (X loses): %d (%.2f%%)\n\n", 
            full_negative, (full_negative * 100.0) / full->size);
    
    fprintf(fp, "TRAINING SET STATISTICS\n");
    fprintf(fp, "----------------------------------------\n");
    fprintf(fp, "Total samples: %d (%.2f%% of full dataset)\n", 
            train->size, (train->size * 100.0) / full->size);
    fprintf(fp, "Positive outcomes: %d (%.2f%%)\n", 
            train_positive, (train_positive * 100.0) / train->size);
    fprintf(fp, "Negative outcomes: %d (%.2f%%)\n\n", 
            train_negative, (train_negative * 100.0) / train->size);
    
    fprintf(fp, "TESTING SET STATISTICS\n");
    fprintf(fp, "----------------------------------------\n");
    fprintf(fp, "Total samples: %d (%.2f%% of full dataset)\n", 
            test->size, (test->size * 100.0) / full->size);
    fprintf(fp, "Positive outcomes: %d (%.2f%%)\n", 
            test_positive, (test_positive * 100.0) / test->size);
    fprintf(fp, "Negative outcomes: %d (%.2f%%)\n\n", 
            test_negative, (test_negative * 100.0) / test->size);
    
    fprintf(fp, "DATA SPLIT CONFIGURATION\n");
    fprintf(fp, "----------------------------------------\n");
    fprintf(fp, "Training/Testing split: 80/20\n");
    fprintf(fp, "Features per sample: %d\n", FEATURES);
    fprintf(fp, "Feature encoding: x (X player), o (O player), b (blank)\n");
    fprintf(fp, "Target variable: positive (X wins), negative (X loses/draws)\n\n");
    
    fprintf(fp, "OUTPUT FILES\n");
    fprintf(fp, "----------------------------------------\n");
    fprintf(fp, "Training set: train.data\n");
    fprintf(fp, "Testing set: test.data\n");
    fprintf(fp, "Report: dataset_report.txt\n\n");
    
    fclose(fp);
    printf("Successfully saved report to %s\n", filename);
    return 1;
}

// Function to free dataset memory
void freeDataset(Dataset *dataset) {
    if (dataset->data != NULL) {
        free(dataset->data);
        dataset->data = NULL;
        dataset->size = 0;
    }
}

int main() {
    Dataset fullDataset, trainSet, testSet;
    
    // a) Read the dataset file
    printf("========================================\n");
    printf("TIC-TAC-TOE DATASET PROCESSOR\n");
    printf("========================================\n\n");
    
    printf("Reading dataset from tic-tac-toe.data...\n");
    if (!readDataset("tic-tac-toe.data", &fullDataset)) {
        return 1;
    }
    
    // Display first few samples
    printf("\nFirst 3 samples:\n");
    for (int i = 0; i < 3 && i < fullDataset.size; i++) {
        printf("\nSample %d:\n", i + 1);
        displayBoard(&fullDataset.data[i]);
    }
    
    // b) Split into training and testing sets
    printf("\nSplitting dataset into training (80%%) and testing (20%%) sets...\n");
    splitDataset(&fullDataset, &trainSet, &testSet);
    
    // c) Save training set to file
    printf("\nSaving training set to train.data...\n");
    if (!saveDataset("train.data", &trainSet)) {
        freeDataset(&fullDataset);
        freeDataset(&trainSet);
        freeDataset(&testSet);
        return 1;
    }
    
    // d) Save testing set to file
    printf("Saving testing set to test.data...\n");
    if (!saveDataset("test.data", &testSet)) {
        freeDataset(&fullDataset);
        freeDataset(&trainSet);
        freeDataset(&testSet);
        return 1;
    }
    
    // e) Save statistics report
    printf("Generating statistics report...\n");
    if (!saveReport("dataset_report.txt", &fullDataset, &trainSet, &testSet)) {
        freeDataset(&fullDataset);
        freeDataset(&trainSet);
        freeDataset(&testSet);
        return 1;
    }
    
    // Display summary
    printf("\n========================================\n");
    printf("PROCESSING COMPLETE\n");
    printf("========================================\n");
    printf("\nFiles created:\n");
    printf("  - train.data (Training set: %d samples)\n", trainSet.size);
    printf("  - test.data (Testing set: %d samples)\n", testSet.size);
    printf("  - dataset_report.txt (Detailed statistics)\n");
    printf("\nAll files saved in the current directory.\n");
    
    // Clean up
    freeDataset(&fullDataset);
    freeDataset(&trainSet);
    freeDataset(&testSet);
    
    return 0;
}