#include <stdio.h>

// Declare the function from another file
void playGame(int mode, int level);

int main() {
    int mode;
    int level = 2; // default medium

    printf("Tic Tac Toe Game!\n");
    printf("Select mode:\n");
    printf("1. Two Player\n");
    printf("2. Play against AI\n");
    printf("Enter choice: ");
    scanf("%d", &mode);

    if (mode == 2) {
        printf("\nSelect AI difficulty:\n");
        printf("1. Easy\n");
        printf("2. Medium\n");
        printf("3. Hard\n");
        printf("Enter choice: ");
        scanf("%d", &level);
        if (level < 1 || level > 3) {
            printf("Invalid level! Defaulting to Medium.\n");
            level = 2;
        }
    }

    playGame(mode, level);  // pass both mode and level to game.c

    return 0;
}
