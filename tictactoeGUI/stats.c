// stats.c — keeps PvP and PvAI separately
#include <stdio.h>
#include "stats.h"

#define STATS_FILE "tictactoe_stats.txt"

typedef struct { int games, x_wins, o_wins, draws; } StatsCat;
typedef struct { StatsCat pvp, pvai; } StatsAll;

static void load_all(StatsAll *all){
    FILE *f = fopen(STATS_FILE, "r");
    all->pvp  = (StatsCat){0,0,0,0};
    all->pvai = (StatsCat){0,0,0,0};

    if (!f) {
        return;
    } else {
        int n = fscanf(f, "%d %d %d %d %d %d %d %d",
            &all->pvp.games,  &all->pvp.x_wins,  &all->pvp.o_wins,  &all->pvp.draws,
            &all->pvai.games, &all->pvai.x_wins, &all->pvai.o_wins, &all->pvai.draws);

        if (n != 8) {
            rewind(f);
            StatsCat legacy = (StatsCat){0,0,0,0};
            if (fscanf(f, "%d %d %d %d",
                      &legacy.games, &legacy.x_wins, &legacy.o_wins, &legacy.draws) == 4) {
                all->pvai = legacy;
            } else {
                /* no-op */
            }
        } else {
            /* parsed 8 values successfully */
        }

        fclose(f);
    }
}

static void save_all(const StatsAll *all){
    FILE *f = fopen(STATS_FILE, "w");
    if (!f) {
        return;
    } else {
        fprintf(f, "%d %d %d %d %d %d %d %d\n",
            all->pvp.games,  all->pvp.x_wins,  all->pvp.o_wins,  all->pvp.draws,
            all->pvai.games, all->pvai.x_wins, all->pvai.o_wins, all->pvai.draws);
        fclose(f);
    }
}

void stats_record_result_mode(StatsMode mode, int winner){
    StatsAll all;
    load_all(&all);

    StatsCat *cat;
    if (mode == STATS_PVP) {
        cat = &all.pvp;
    } else {
        cat = &all.pvai;
    }

    cat->games++;

    if (winner == 1) {
        cat->x_wins++;
    } else {
        if (winner == 2) {
            cat->o_wins++;
        } else {
            cat->draws++;
        }
    }

    save_all(&all);
}

void stats_get_counts_mode(StatsMode mode, int *games, int *x_wins, int *o_wins, int *draws){
    StatsAll all;
    load_all(&all);

    StatsCat *cat;
    if (mode == STATS_PVP) {
        cat = &all.pvp;
    } else {
        cat = &all.pvai;
    }

    if (games) {
        *games = cat->games;
    } else {
        /* no-op */
    }

    if (x_wins) {
        *x_wins = cat->x_wins;
    } else {
        /* no-op */
    }

    if (o_wins) {
        *o_wins = cat->o_wins;
    } else {
        /* no-op */
    }

    if (draws) {
        *draws = cat->draws;
    } else {
        /* no-op */
    }
}
