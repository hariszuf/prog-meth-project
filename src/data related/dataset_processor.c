#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define FEATURES 9
#define INITIAL_CAPACITY 1000  // Start with 1000, will expand as needed

// Structure to hold a single data sample
typedef struct {
    char features[FEATURES];  // 'x', 'o', or 'b' for each square
    char outcome;             // 'w' for win, 'l' for lose, 'd' for draw
} Sample;

// Structure to hold the dataset
typedef struct {
    Sample *data;
    int size;
    int capacity;
} Dataset;

// Function prototypes
void initDataset(Dataset *dataset);
void expandDataset(Dataset *dataset);
void freeDataset(Dataset *dataset);
int readDataset(const char *filename, Dataset *dataset);
void shuffleDataset(Dataset *dataset);
void splitDataset(Dataset *full, Dataset *train, Dataset *test, double train_ratio);
int saveDataset(const char *filename, Dataset *dataset);
int saveReport(const char *filename, Dataset *full, Dataset *train, Dataset *test);
void printSample(Sample *s);
void displayBoard(Sample *s);

// Initialize dataset with dynamic memory
void initDataset(Dataset *dataset) {
    dataset->capacity = INITIAL_CAPACITY;
    dataset->data = (Sample *)malloc(dataset->capacity * sizeof(Sample));
    if (dataset->data == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(1);
    }
    dataset->size = 0;
}

// Expand dataset capacity when needed
void expandDataset(Dataset *dataset) {
    dataset->capacity *= 2;
    Sample *new_data = (Sample *)realloc(dataset->data, dataset->capacity * sizeof(Sample));
    if (new_data == NULL) {
        fprintf(stderr, "Error: Memory reallocation failed\n");
        freeDataset(dataset);
        exit(1);
    }
    dataset->data = new_data;
    printf("Dataset capacity expanded to %d samples\n", dataset->capacity);
}

// Free dataset memory
void freeDataset(Dataset *dataset) {
    if (dataset->data != NULL) {
        free(dataset->data);
        dataset->data = NULL;
        dataset->size = 0;
        dataset->capacity = 0;
    }
}

// Function to shuffle the dataset using Fisher-Yates algorithm
void shuffleDataset(Dataset *dataset) {
    srand(time(NULL));
    
    for (int i = dataset->size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        
        // Swap dataset->data[i] and dataset->data[j]
        Sample temp = dataset->data[i];
        dataset->data[i] = dataset->data[j];
        dataset->data[j] = temp;
    }
    
    printf("Dataset shuffled randomly\n");
}

// Function to open and read the dataset file (DYNAMIC SIZE)
int readDataset(const char *filename, Dataset *dataset) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error: Could not open file %s\n", filename);
        return 0;
    }
    
    char line[256];
    int line_num = 0;
    
    // Read each line from the file
    while (fgets(line, sizeof(line), fp) != NULL) {
        line_num++;
        
        // Expand dataset if needed
        if (dataset->size >= dataset->capacity) {
            expandDataset(dataset);
        }
        
        // Remove newline character
        line[strcspn(line, "\n")] = 0;
        
        // Skip empty lines
        if (strlen(line) == 0) {
            continue;
        }
        
        // Parse the line (expected format: x,o,b,x,x,o,b,b,x,win)
        char *token = strtok(line, ",");
        int i = 0;
        
        // Read 9 features
        while (token != NULL && i < FEATURES) {
            // Validate token length
            if (strlen(token) != 1) {
                fprintf(stderr, "Warning: Invalid feature at line %d, position %d\n", 
                        line_num, i + 1);
                break;
            }
            
            // Validate feature value
            char feature = token[0];
            if (feature != 'x' && feature != 'o' && feature != 'b') {
                fprintf(stderr, "Warning: Invalid feature value '%c' at line %d, position %d\n", 
                        feature, line_num, i + 1);
                break;
            }
            
            dataset->data[dataset->size].features[i] = feature;
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
        if (token != NULL) {
            if (strcmp(token, "win") == 0) {
                dataset->data[dataset->size].outcome = 'w';
            } else if (strcmp(token, "lose") == 0) {
                dataset->data[dataset->size].outcome = 'l';
            } else if (strcmp(token, "draw") == 0) {
                dataset->data[dataset->size].outcome = 'd';
            } else {
                fprintf(stderr, "Warning: Invalid outcome '%s' at line %d, skipping\n", 
                        token, line_num);
                continue;
            }
            dataset->size++;
        } else {
            fprintf(stderr, "Warning: Missing outcome at line %d, skipping\n", line_num);
        }
    }
    
    fclose(fp);
    printf("Successfully loaded %d samples from %s\n", dataset->size, filename);
    return 1;
}

// Function to split dataset into training and testing sets (DYNAMIC)
void splitDataset(Dataset *full, Dataset *train, Dataset *test, double train_ratio) {
    // Calculate split sizes
    int train_size = (int)(full->size * train_ratio);
    int test_size = full->size - train_size;
    
    // Initialize train and test datasets
    initDataset(train);
    initDataset(test);
    
    // Ensure capacity
    if (train_size > train->capacity) {
        train->capacity = train_size;
        train->data = (Sample *)realloc(train->data, train->capacity * sizeof(Sample));
    }
    if (test_size > test->capacity) {
        test->capacity = test_size;
        test->data = (Sample *)realloc(test->data, test->capacity * sizeof(Sample));
    }
    
    // Copy data
    train->size = train_size;
    test->size = test_size;
    
    // Copy first train_ratio% to training set
    for (int i = 0; i < train_size; i++) {
        memcpy(&train->data[i], &full->data[i], sizeof(Sample));
    }
    
    // Copy remaining to testing set
    for (int i = 0; i < test_size; i++) {
        memcpy(&test->data[i], &full->data[train_size + i], sizeof(Sample));
    }
    
    printf("\nDataset split:\n");
    printf("  Training set: %d samples (%.1f%%)\n", train_size, train_ratio * 100);
    printf("  Testing set:  %d samples (%.1f%%)\n", test_size, (1.0 - train_ratio) * 100);
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
    printf("Outcome: %s\n\n", 
           s->outcome == 'w' ? "Win" : (s->outcome == 'l' ? "Lose" : "Draw"));
}

// Function to save dataset to file
int saveDataset(const char *filename, Dataset *dataset) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        fprintf(stderr, "Error: Could not create file %s\n", filename);
        return 0;
    }
    
    for (int i = 0; i < dataset->size; i++) {
        // Write features
        for (int j = 0; j < FEATURES; j++) {
            fprintf(fp, "%c", dataset->data[i].features[j]);
            if (j < FEATURES - 1) fprintf(fp, ",");
        }
        // Write outcome
        fprintf(fp, ",%s\n", 
                dataset->data[i].outcome == 'w' ? "win" : 
                (dataset->data[i].outcome == 'l' ? "lose" : "draw"));
    }
    
    fclose(fp);
    printf("Successfully saved %d samples to %s\n", dataset->size, filename);
    return 1;
}

// Function to save statistics report
int saveReport(const char *filename, Dataset *full, Dataset *train, Dataset *test) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        fprintf(stderr, "Error: Could not create file %s\n", filename);
        return 0;
    }
    
    // Count outcomes in full dataset
    int full_win = 0, full_lose = 0, full_draw = 0;
    for (int i = 0; i < full->size; i++) {
        if (full->data[i].outcome == 'w') full_win++;
        else if (full->data[i].outcome == 'l') full_lose++;
        else if (full->data[i].outcome == 'd') full_draw++;
    }
    
    // Count outcomes in training set
    int train_win = 0, train_lose = 0, train_draw = 0;
    for (int i = 0; i < train->size; i++) {
        if (train->data[i].outcome == 'w') train_win++;
        else if (train->data[i].outcome == 'l') train_lose++;
        else if (train->data[i].outcome == 'd') train_draw++;
    }
    
    // Count outcomes in testing set
    int test_win = 0, test_lose = 0, test_draw = 0;
    for (int i = 0; i < test->size; i++) {
        if (test->data[i].outcome == 'w') test_win++;
        else if (test->data[i].outcome == 'l') test_lose++;
        else if (test->data[i].outcome == 'd') test_draw++;
    }
    
    // Write report
    fprintf(fp, "========================================\n");
    fprintf(fp, "TIC-TAC-TOE DATASET PROCESSING REPORT\n");
    fprintf(fp, "Generated from Minimax Optimal Dataset\n");
    fprintf(fp, "========================================\n\n");
    
    fprintf(fp, "FULL DATASET STATISTICS\n");
    fprintf(fp, "----------------------------------------\n");
    fprintf(fp, "Total samples: %d\n", full->size);
    fprintf(fp, "Win outcomes:  %d (%.2f%%)\n", 
            full_win, (full_win * 100.0) / full->size);
    fprintf(fp, "Lose outcomes: %d (%.2f%%)\n", 
            full_lose, (full_lose * 100.0) / full->size);
    fprintf(fp, "Draw outcomes: %d (%.2f%%)\n\n", 
            full_draw, (full_draw * 100.0) / full->size);
    
    fprintf(fp, "TRAINING SET STATISTICS\n");
    fprintf(fp, "----------------------------------------\n");
    fprintf(fp, "Total samples: %d (%.2f%% of full dataset)\n", 
            train->size, (train->size * 100.0) / full->size);
    fprintf(fp, "Win outcomes:  %d (%.2f%%)\n", 
            train_win, (train_win * 100.0) / train->size);
    fprintf(fp, "Lose outcomes: %d (%.2f%%)\n", 
            train_lose, (train_lose * 100.0) / train->size);
    fprintf(fp, "Draw outcomes: %d (%.2f%%)\n\n", 
            train_draw, (train_draw * 100.0) / train->size);
    
    fprintf(fp, "TESTING SET STATISTICS\n");
    fprintf(fp, "----------------------------------------\n");
    fprintf(fp, "Total samples: %d (%.2f%% of full dataset)\n", 
            test->size, (test->size * 100.0) / full->size);
    fprintf(fp, "Win outcomes:  %d (%.2f%%)\n", 
            test_win, (test_win * 100.0) / test->size);
    fprintf(fp, "Lose outcomes: %d (%.2f%%)\n", 
            test_lose, (test_lose * 100.0) / test->size);
    fprintf(fp, "Draw outcomes: %d (%.2f%%)\n\n", 
            test_draw, (test_draw * 100.0) / test->size);
    
    fprintf(fp, "DATA SPLIT CONFIGURATION\n");
    fprintf(fp, "----------------------------------------\n");
    fprintf(fp, "Training/Testing split: %.0f/%.0f\n", 
            (train->size * 100.0) / full->size, 
            (test->size * 100.0) / full->size);
    fprintf(fp, "Features per sample: %d\n", FEATURES);
    fprintf(fp, "Feature encoding: x (X player), o (O player), b (blank)\n");
    fprintf(fp, "Target variable: win, lose, draw (3 classes)\n");
    fprintf(fp, "Shuffling: YES (Random seed based on system time)\n");
    fprintf(fp, "Data source: Minimax algorithm (optimal play)\n\n");
    
    fprintf(fp, "OUTPUT FILES\n");
    fprintf(fp, "----------------------------------------\n");
    fprintf(fp, "Training set: train.data\n");
    fprintf(fp, "Testing set: test.data\n");
    fprintf(fp, "Report: dataset_report.txt\n\n");
    
    fprintf(fp, "========================================\n");
    fprintf(fp, "Note: All outcomes represent optimal play\n");
    fprintf(fp, "========================================\n");
    
    fclose(fp);
    printf("Successfully saved report to %s\n", filename);
    return 1;
}

int main(int argc, char *argv[]) {
    Dataset fullDataset, trainSet, testSet;
    char input_filename[256];
    char train_filename[256];
    char test_filename[256];
    char report_filename[256];
    double train_ratio = 0.8;  // Default 80/20 split
    
    // Print header
    printf("========================================\n");
    printf("TIC-TAC-TOE DATASET PROCESSOR\n");
    printf("Dynamic Size - Works with dataset-gen.c\n");
    printf("========================================\n\n");
    
    // Get input filename
    if (argc > 1) {
        // Use command-line argument
        strncpy(input_filename, argv[1], sizeof(input_filename) - 1);
        input_filename[sizeof(input_filename) - 1] = '\0';
    } else {
        // Prompt user
        printf("Enter input filename [default: tic-tac-toe-minimax-complete.data]: ");
        if (fgets(input_filename, sizeof(input_filename), stdin) != NULL) {
            // Remove newline
            input_filename[strcspn(input_filename, "\n")] = 0;
            
            // Use default if empty
            if (strlen(input_filename) == 0) {
                strcpy(input_filename, "tic-tac-toe-minimax-complete.data");
            }
        } else {
            strcpy(input_filename, "tic-tac-toe-minimax-complete.data");
        }
    }
    
    // Determine output filenames based on input filename
    if (strstr(input_filename, "non-terminal") != NULL) {
        strcpy(train_filename, "train_non_terminal.data");
        strcpy(test_filename, "test_non_terminal.data");
        strcpy(report_filename, "report_non_terminal.txt");
        printf("\n📊 Processing NON-TERMINAL dataset\n");
        printf("   Output: train_non_terminal.data, test_non_terminal.data\n\n");
    } else if (strstr(input_filename, "complete") != NULL) {
        strcpy(train_filename, "train_combined.data");
        strcpy(test_filename, "test_combined.data");
        strcpy(report_filename, "report_combined.txt");
        printf("\n📊 Processing COMBINED dataset\n");
        printf("   Output: train_combined.data, test_combined.data\n\n");
    } else {
        strcpy(train_filename, "train.data");
        strcpy(test_filename, "test.data");
        strcpy(report_filename, "dataset_report.txt");
        printf("\n📊 Processing dataset\n");
        printf("   Output: train.data, test.data\n\n");
    }
    
    // Get train/test split ratio
    if (argc > 2) {
        train_ratio = atof(argv[2]);
        if (train_ratio <= 0 || train_ratio >= 1) {
            fprintf(stderr, "Invalid train ratio: %.2f, using default 0.8\n", train_ratio);
            train_ratio = 0.8;
        }
    }
    
    // Initialize and read dataset
    printf("\nReading dataset from %s...\n", input_filename);
    initDataset(&fullDataset);
    
    if (!readDataset(input_filename, &fullDataset)) {
        freeDataset(&fullDataset);
        return 1;
    }
    
    if (fullDataset.size == 0) {
        fprintf(stderr, "Error: No valid samples loaded\n");
        freeDataset(&fullDataset);
        return 1;
    }
    
    // Display first few samples BEFORE shuffling
    printf("\nFirst 3 samples (before shuffling):\n");
    for (int i = 0; i < 3 && i < fullDataset.size; i++) {
        printf("\nSample %d:\n", i + 1);
        displayBoard(&fullDataset.data[i]);
    }
    
    // CRITICAL: Shuffle the dataset before splitting
    printf("\n*** SHUFFLING DATASET ***\n");
    shuffleDataset(&fullDataset);
    
    // Display first few samples AFTER shuffling
    printf("\nFirst 3 samples (after shuffling):\n");
    for (int i = 0; i < 3 && i < fullDataset.size; i++) {
        printf("\nSample %d:\n", i + 1);
        displayBoard(&fullDataset.data[i]);
    }
    
    // Split into training and testing sets
    printf("\nSplitting dataset into training and testing sets...\n");
    splitDataset(&fullDataset, &trainSet, &testSet, train_ratio);
    
    // Save training set to file
    printf("\nSaving training set to %s...\n", train_filename);
    if (!saveDataset(train_filename, &trainSet)) {
        freeDataset(&fullDataset);
        freeDataset(&trainSet);
        freeDataset(&testSet);
        return 1;
    }
    
    // Save testing set to file
    printf("Saving testing set to %s...\n", test_filename);
    if (!saveDataset(test_filename, &testSet)) {
        freeDataset(&fullDataset);
        freeDataset(&trainSet);
        freeDataset(&testSet);
        return 1;
    }
    
    // Save statistics report
    printf("Generating statistics report...\n");
    if (!saveReport(report_filename, &fullDataset, &trainSet, &testSet)) {
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
    printf("  - %s (Training set: %d samples)\n", train_filename, trainSet.size);
    printf("  - %s (Testing set: %d samples)\n", test_filename, testSet.size);
    printf("  - %s (Detailed statistics)\n", report_filename);
    printf("\nAll files saved in the current directory.\n");
    printf("\n*** IMPORTANT: Data was randomly shuffled before splitting ***\n");
    printf("\nUsage: %s [input_file] [train_ratio]\n", argv[0]);
    printf("Example: %s tic-tac-toe-minimax-complete.data 0.8\n", argv[0]);
    printf("Example: %s tic-tac-toe-minimax-non-terminal.data 0.8\n", argv[0]);
    
    // Clean up
    freeDataset(&fullDataset);
    freeDataset(&trainSet);
    freeDataset(&testSet);
    
    return 0;
}
