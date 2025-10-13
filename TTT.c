#include <stdio.h>

void printBoard(char b[9]){
    printf("\n %c | %c | %c\n",b[0],b[1],b[2]);
    printf("---+---+---\n");
    printf(" %c | %c | %c\n",b[3],b[4],b[5]);
    printf("---+---+---\n");
    printf(" %c | %c | %c\n\n",b[6],b[7],b[8]);
}

int main(){
    char b[9] = {'1','2','3','4','5','6','7','8','9'};
    int turn = 0;
    int pos;
    char mark;
    int win = 0;

    printf("Tic Tac Toe Game!\n");
    printf("Player1=X , Player2=O\n");

    while(1){
        printBoard(b);

        if (turn % 2 == 0) {
            mark = 'X';
        } else {
            mark = 'O';
        }

        printf("Player %d (%c), enter a position (1-9): ", (turn % 2) + 1, mark);
        scanf("%d", &pos);

        /* FIX: use || (OR) and && (AND), not bitwise & or | */
        if (pos < 1 || pos > 9){
            printf("Invalid number! Try again.\n");
            continue;
        }
        if (b[pos - 1] == 'X' || b[pos - 1] == 'O'){
            printf("That spot is already taken!\n");
            continue;
        }

        b[pos - 1] = mark;

        /* winner check: any line true => win (use ||) */
        if ( (b[0]==b[1] && b[1]==b[2]) ||
             (b[3]==b[4] && b[4]==b[5]) ||
             (b[6]==b[7] && b[7]==b[8]) ||
             (b[0]==b[3] && b[3]==b[6]) ||
             (b[1]==b[4] && b[4]==b[7]) ||
             (b[2]==b[5] && b[5]==b[8]) ||
             (b[0]==b[4] && b[4]==b[8]) ||
             (b[2]==b[4] && b[4]==b[6]) ) {
            win = 1;
        }

        if (win == 1) {
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

    return 0;
}
