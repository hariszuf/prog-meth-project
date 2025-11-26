#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX 10000
#define FEATURES 9
#define LINE_LEN 200

// Encode x/o/b to numbers
int encode(char *token) {
    if (strcmp(token, "x") == 0) return 1;
    if (strcmp(token, "o") == 0) return -1;
    return 0; // "b"
}

// Load .data file
int load_dataset(char *filename, int X[][FEATURES], char y[][10]) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("ERROR: Cannot open %s\n", filename);
        exit(1);
    }

    char line[LINE_LEN];
    int row = 0;

    while (fgets(line, sizeof(line), fp)) {
        char *token = strtok(line, ",");
        int col = 0;

        // First 9 tokens = features
        while (token && col < FEATURES) {
            X[row][col] = encode(token);
            token = strtok(NULL, ",");
            col++;
        }

        // Final token = class label
        if (token)
            strcpy(y[row], token);

        row++;
    }

    fclose(fp);
    return row;
}

// Compute Euclidean distance
double distance(int a[], int b[]) {
    double sum = 0;
    for (int i = 0; i < FEATURES; i++)
        sum += (a[i] - b[i]) * (a[i] - b[i]);
    return sqrt(sum);
}

// 1 nearest neighbour
void predict_1nn(int X_train[][FEATURES], char y_train[][10], int train_size,
                 int test_x[], char *predicted) {
    double best_dist = 1e9;
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

// Run one experiment
void run_experiment(
    char *train_file, char *test_file,
    int X_train[][FEATURES], int X_test[][FEATURES],
    char y_train[][10], char y_test[][10], char y_pred[][10]
) {
    printf("\n==============================\n");
    printf("Training: %s\n", train_file);
    printf("Testing : %s\n", test_file);
    printf("==============================\n");

    int train_size = load_dataset(train_file, X_train, y_train);
    int test_size = load_dataset(test_file, X_test, y_test);

    int TP = 0, TN = 0, FP = 0, FN = 0;
    int correct = 0;

    for (int i = 0; i < test_size; i++) {
        predict_1nn(X_train, y_train, train_size, X_test[i], y_pred[i]);

        if (strcmp(y_pred[i], y_test[i]) == 0)
            correct++;

        int actual_win = strcmp(y_test[i], "win") == 0;
        int pred_win   = strcmp(y_pred[i], "win") == 0;

        if (actual_win && pred_win) TP++;
        else if (actual_win && !pred_win) FN++;
        else if (!actual_win && pred_win) FP++;
        else TN++;
    }

    double accuracy = (double)correct / test_size;
    double error = 1 - accuracy;

    printf("\nConfusion Matrix (WIN vs NOT-WIN):\n");
    printf("[ %4d   %4d ]   <- Actual WIN\n", TP, FN);
    printf("[ %4d   %4d ]   <- Actual NOT-WIN\n\n", FP, TN);

    printf("Accuracy: %.4f\n", accuracy);
    printf("Probability of Error: %.4f\n", error);
}

int main() {
    int X_train[MAX][FEATURES], X_test[MAX][FEATURES];
    char y_train[MAX][10], y_test[MAX][10], y_pred[MAX][10];

    // 4 experiment combinations
    run_experiment("train_combined.data",     "test_combined.data",
                   X_train, X_test, y_train, y_test, y_pred);

    run_experiment("train_combined.data",     "test_non_terminal.data",
                   X_train, X_test, y_train, y_test, y_pred);

    run_experiment("train_non_terminal.data",  "test_combined.data",
                   X_train, X_test, y_train, y_test, y_pred);

    run_experiment("train_non_terminal.data",  "test_non_terminal.data",
                   X_train, X_test, y_train, y_test, y_pred);

    return 0;
}