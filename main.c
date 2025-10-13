#include <stdio.h>

// Declare the function from another file
void playGame(int mode);

int main() {
    int mode;

    printf("Tic Tac Toe Game!\n");
    printf("Select mode:\n");
    printf("1. Two Player\n");
    printf("2. Play against AI\n");
    printf("Enter choice: ");
    scanf("%d", &mode);

    playGame(mode);  // Call the function from game.c

    return 0;
}
