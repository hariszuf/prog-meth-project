// main.c — console front-end, uses engine + stats (PvP/PvAI)
#include <stdio.h>
#include <stdlib.h>  // <-- add this line
#include <time.h>
#include "game.h"
#include "stats.h"

static void printBoard(const char b[9]){
    printf("\n %c | %c | %c\n", b[0], b[1], b[2]);
    printf("---+---+---\n");
    printf(" %c | %c | %c\n", b[3], b[4], b[5]);
    printf("---+---+---\n");
    printf(" %c | %c | %c\n\n", b[6], b[7], b[8]);
}

int main(void){
    srand((unsigned)time(NULL));

    int mode=1, level=2;
    Game g; game_init(&g);

    printf("Tic Tac Toe Game!\n");
    printf("1. Two Player\n2. Play against AI\nEnter choice: ");
    if (scanf("%d",&mode)!=1) {
        return 0;
    }
    if (mode==2){
        printf("Select AI difficulty (1=Easy 2=Med 3=Hard): ");
        if (scanf("%d",&level)!=1 || level<1 || level>3) {
            level=2;
        }
    } else {
        mode=1;
    }

    while(1){
        printBoard(g.b);
        if (mode==2 && g.turn=='O'){
            game_ai_move(&g, level);
        } else {
            int pos;
            printf("Player %c, enter position (1-9): ", g.turn);
            if (scanf("%d",&pos)!=1) {
                return 0;
            }
            if (!game_make_move(&g, pos-1)){
                puts("Invalid move.");
                continue;
            }
        }
        game_check_end(&g);
        if (g.winner){
            printBoard(g.b);
            int winnerCode = (g.winner==1)?1:(g.winner==2)?2:0;
            if (g.winner==1) {
                puts("X wins!");
            } else if (g.winner==2) {
                puts("O wins!");
            } else {
                puts("Draw!");
            }
            StatsMode smode = (mode==1)?STATS_PVP:STATS_PVAI;
            stats_record_result_mode(smode, winnerCode);
            int games,xw,ow,dr; stats_get_counts_mode(smode,&games,&xw,&ow,&dr);
            printf("\n=== %s totals ===\n", (smode==STATS_PVP)?"PvP":"PvAI");
            printf("Games: %d\nX wins: %d\nO wins: %d\nDraws: %d\n", games,xw,ow,dr);
            break;
        }
    }
    return 0;
}

