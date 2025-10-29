// main.c — simple console front-end (student style)
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "game.h"
#include "stats.h"

// draw the 3x3 board
static void printBoard(const char b[9]) {
    printf("\n %c | %c | %c\n", b[0], b[1], b[2]);
    printf("---+---+---\n");
    printf(" %c | %c | %c\n", b[3], b[4], b[5]);
    printf("---+---+---\n");
    printf(" %c | %c | %c\n\n", b[6], b[7], b[8]);
}

int main(void) {
    srand((unsigned)time(NULL)); // in case AI uses randomness somewhere

    Game g;
    int gameMode = 1; // 1 = PvP, 2 = PvAI
    int aiLvl = 2;    // 1=Easy, 2=Med, 3=Hard

    game_init(&g);

    printf("=== Tic Tac Toe ===\n");
    printf("1) Two Players\n");
    printf("2) Play vs AI\n");
    printf("Enter choice: ");

    if (scanf("%d", &gameMode) != 1) return 0;
    if (gameMode == 2) {
        printf("Select AI difficulty (1=Easy, 2=Med, 3=Hard): ");
        if (scanf("%d", &aiLvl) != 1) aiLvl = 2;
        if (aiLvl < 1 || aiLvl > 3) aiLvl = 2;
    } else {
        gameMode = 1;
    }

    // main loop: keep playing until there is a winner or draw
    while (1) {
        printBoard(g.b);

        if (gameMode == 2 && g.turn == 'O') {
            // AI plays as 'O'
            game_ai_move(&g, aiLvl);
        } else {
            int pos; // user enters 1..9
            printf("Player %c, enter position (1-9): ", g.turn);
            if (scanf("%d", &pos) != 1) return 0;

            // convert to index 0..8 and try
            if (!game_make_move(&g, pos - 1)) {
                puts("Invalid move. Try again.");
                continue; // ask again without switching turn
            }
        }

        // check if game is over after the move
        game_check_end(&g);
        if (g.winner != 0) {
            printBoard(g.b);

            // announce result
            if (g.winner == 1) {
                puts("X wins!");
            } else if (g.winner == 2) {
                if (gameMode == 1) puts("O wins!");
                else               puts("AI (O) wins!");
            } else {
                puts("It's a draw!");
            }

            // record stats per mode
            StatsMode smode = (gameMode == 1) ? STATS_PVP : STATS_PVAI;
            int resultCode;
            if (g.winner == 1) resultCode = 1;     // X
            else if (g.winner == 2) resultCode = 2; // O
            else resultCode = 0;                    // draw

            stats_record_result_mode(smode, resultCode);

            // show totals for this category
            int games = 0, xw = 0, ow = 0, dr = 0;
            stats_get_counts_mode(smode, &games, &xw, &ow, &dr);

            printf("\n=== %s totals ===\n", (smode == STATS_PVP) ? "PvP" : "PvAI");
            printf("Games : %d\n", games);
            printf("X wins: %d\n", xw);
            printf("O wins: %d\n", ow);
            printf("Draws : %d\n", dr);

            break; // end program after one game
        }
    }

    return 0;
}
