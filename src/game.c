#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Declare AI functions from minimax.c
int winBy(char b[9], char p);
int findBestMoveLvl(char b[9], int level);

void printBoard(char b[9]) {
    printf("\n %c | %c | %c\n", b[0], b[1], b[2]);
    printf("---+---+---\n");
    printf(" %c | %c | %c\n", b[3], b[4], b[5]);
    printf("---+---+---\n");
    printf(" %c | %c | %c\n\n", b[6], b[7], b[8]);
}

void playGame(int mode) {
    char b[9] = {'1','2','3','4','5','6','7','8','9'};
    int turn = 0;
    int pos;
    char mark;
    int win = 0;
    int level = 2; // 1=easy, 2=medium, 3=hard

    srand((unsigned)time(NULL));
    printf("Player1=X , Player2=O\n");

    while (1) {
        printBoard(b);
        mark = (turn % 2 == 0) ? 'X' : 'O';

        if (mode == 2 && mark == 'O') {
            pos = findBestMoveLvl(b, level);
            printf("AI chooses position %d\n", pos);
        } else {
            printf("Player %d (%c), enter a position (1-9): ", (turn % 2) + 1, mark);
            if (scanf("%d", &pos) != 1) {
                int c; while ((c = getchar()) != '\n' && c != EOF) {}
                printf("Invalid input! Try again.\n");
                continue;
            }
            if (pos < 1 || pos > 9) {
                printf("Invalid number! Try again.\n");
                continue;
            }
            if (b[pos - 1] == 'X' || b[pos - 1] == 'O') {
                printf("That spot is already taken!\n");
                continue;
            }
        }

        b[pos - 1] = mark;

        if (winBy(b, 'X') || winBy(b, 'O')) {
            printBoard(b);
            printf("Player %d (%c) won!!!\n", (turn % 2) + 1, mark);
            break;
        }

        if (turn == 8) {
            printBoard(b);
            printf("Draw Game!\n");
            break;
        }

        turn++;
    }
}
