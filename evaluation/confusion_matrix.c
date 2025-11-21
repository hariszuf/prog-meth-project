#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX 10000   // max number of rows
#define FEATURES 9  // 9 board positions
#define LINE_LEN 100

// Convert char token to number
int encode(char *token) {
    if (strcmp(token, "x") == 0) return 1;
    if (strcmp(token, "o") == 0) return -1;
    return 0; // "b"
}

// Load dataset (.data file)
int load_dataset(char *filename, int X[][FEATURES], char y[][10]) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("Error opening %s\n", filename);
        exit(1);
    }

    char line[LINE_LEN];
    int row = 0;

    while (fgets(line, sizeof(line), fp)) {
        char *token = strtok(line, ",");
        int col = 0;

        while (token && col < FEATURES) {
            X[row][col] = encode(token);
            token = strtok(NULL, ",");
            col++;
        }

        // last token = class label (win/loss/draw)
        if (token)
            strcpy(y[row], token);

        row++;
    }

    fclose(fp);
    return row; // number of rows loaded
}

// Compute Euclidean distance between two rows
double distance(int a[], int b[]) {
    double sum = 0;
    for (int i = 0; i < FEATURES; i++)
        sum += (a[i] - b[i]) * (a[i] - b[i]);
    return sqrt(sum);
}

// 1-NN prediction
void predict_1nn(int X_train[][FEATURES], char y_train[][10], int train_size,
                 int test_x[], char *predicted) {
    double best_dist = 999999;
    int best_index = 0;

    for (int i = 0; i < train_size; i++) {
        double d = distance(test_x, X_train[i]);
        if (d < best_dist) {
            best_dist = d;
            best_index = i;
        }
    }

    strcpy(predicted, y_train[best_index]);
}

// MAIN PROGRAM
int main() {
    int X_train[MAX][FEATURES], X_test[MAX][FEATURES];
    char y_train[MAX][10], y_test[MAX][10];
    char y_pred[MAX][10];

    // Load training and testing datasets
    int train_size = load_dataset("train-combined.data", X_train, y_train);
    int test_size  = load_dataset("test-combined.data",  X_test,  y_test);

    printf("Training rows: %d\n", train_size);
    printf("Testing rows: %d\n\n", test_size);

    // ---- Predict for all test rows ----
    int correct = 0;
    int TP = 0, TN = 0, FP = 0, FN = 0;

    for (int i = 0; i < test_size; i++) {
        predict_1nn(X_train, y_train, train_size, X_test[i], y_pred[i]);

        if (strcmp(y_pred[i], y_test[i]) == 0)
            correct++;

        // confusion matrix for WIN vs NOT-WIN
        int actual_win = strcmp(y_test[i], "win") == 0;
        int pred_win   = strcmp(y_pred[i], "win") == 0;

        if (actual_win && pred_win) TP++;
        else if (!actual_win && !pred_win) TN++;
        else if (!actual_win && pred_win) FP++;
        else if (actual_win && !pred_win) FN++;
    }

    // ---- Print Results ----
    printf("Confusion Matrix (WIN vs NOT-WIN):\n");
    printf("TP = %d\n", TP);
    printf("FN = %d\n", FN);
    printf("FP = %d\n", FP);
    printf("TN = %d\n\n", TN);

    double accuracy = (double)correct / test_size;
    double error = 1 - accuracy;

    printf("Accuracy: %.4f\n", accuracy);
    printf("Probability of Error: %.4f\n", error);

    return 0;
}