// stats.c — keeps separate statistics for PvP and PvAI
#include <stdio.h>      // for file operations: fopen, fscanf, fprintf, fclose
#include "stats.h"      // include our own header for function prototypes and enums

// File name to store all statistics
#define STATS_FILE "tictactoe_stats.txt"

// Each category (PvP or PvAI) keeps total games, wins for X, wins for O, and draws
typedef struct {
    int games;      // total number of games played
    int x_wins;     // total number of X wins
    int o_wins;     // total number of O wins
    int draws;      // total number of draws
} Stats;

// Struct containing both categories
typedef struct {
    Stats pvp;   // Player vs Player statistics
    Stats pvai;  // Player vs AI statistics
} AllStats;

/*---------------------------------------------
   Helper Function: load_all
   Purpose: read the statistics from file into memory
----------------------------------------------*/
static void load_all(AllStats *all)
{
    FILE *f;   // file pointer

    // open file for reading
    f = fopen(STATS_FILE, "r");

    // if the file cannot be opened, just return (stats remain zero)
    if (f == NULL)
    {
        *all = (AllStats){0};
        return;
    }
    else
    {
        // try to read 8 numbers: 4 for PvP + 4 for PvAI
        fscanf(f, "%d %d %d %d %d %d %d %d",
               &all->pvp.games,  &all->pvp.x_wins,  &all->pvp.o_wins,  &all->pvp.draws,
               &all->pvai.games, &all->pvai.x_wins, &all->pvai.o_wins, &all->pvai.draws);

        // close file after reading
        fclose(f);
    }
}

/*---------------------------------------------
   Helper Function: save_all
   Purpose: write all statistics to file
----------------------------------------------*/
static void save_all(AllStats *all)
{
    FILE *f;

    // open file for writing (overwrites old data)
    f = fopen(STATS_FILE, "w");

    // if the file cannot be opened, stop
    if (f == NULL)
    {
        return;
    }
    else
    {
        // write all 8 numbers to file
        fprintf(f, "%d %d %d %d %d %d %d %d\n",
                all->pvp.games,  all->pvp.x_wins,  all->pvp.o_wins,  all->pvp.draws,
                all->pvai.games, all->pvai.x_wins, all->pvai.o_wins, all->pvai.draws);

        // close file
        fclose(f);
    }
}

/*---------------------------------------------
   Function: stats_record_result_mode
   Purpose: update statistics after each game
----------------------------------------------*/
void stats_record_result_mode(StatsMode mode, int winner)
{
    AllStats all;      // holds both PvP and PvAI stats
    Stats *cat;     // pointer to selected category

    // load all stats from file
    load_all(&all);

    // decide which category to update based on mode
    if (mode == STATS_PVP)
    {
        cat = &all.pvp;     // update Player vs Player
    }
    else if (mode == STATS_PVAI)
    {
        cat = &all.pvai;    // update Player vs AI
    }
    else
    {
        return; // invalid mode
    }

    // increase total games played
    cat->games++;

    // update according to winner
    if (winner == 1)
    {
        // X wins
        cat->x_wins++;
    }
    else if (winner == 2)
    {
        // O wins
        cat->o_wins++;
    }
    else
    {
        // draw
        cat->draws++;
    }

    // save updated stats back to file
    save_all(&all);
}

/*---------------------------------------------
   Function: stats_get_counts_mode
   Purpose: return the stats of a given mode
----------------------------------------------*/
void stats_get_counts_mode(StatsMode mode, int *games, int *x_wins, int *o_wins, int *draws)
{
    AllStats all;    // holds all statistics
    Stats *cat;   // pointer to the correct category

    // load current statistics from file
    load_all(&all);

    // choose the correct category based on mode
    if (mode == STATS_PVP)
    {
        cat = &all.pvp;
    }
    else if (mode == STATS_PVAI)
    {
        cat = &all.pvai;
    }
    else
    {
        return; // invalid mode
    }

    // return values to caller (only if pointer is not NULL)
    if (games != NULL)
    {
        *games = cat->games;
    }

    if (x_wins != NULL)
    {
        *x_wins = cat->x_wins;
    }

    if (o_wins != NULL)
    {
        *o_wins = cat->o_wins;
    }

    if (draws != NULL)
    {
        *draws = cat->draws;
    }
}

void stats_reset_pvp(void)
{
    FILE *f = fopen(STATS_FILE, "r");
    int pvp_g, pvp_x, pvp_o, pvp_d;
    int ai_g, ai_x, ai_o, ai_d;

    // If file exists, read current stats
    if (f)
    {
        fscanf(f, "%d %d %d %d %d %d %d %d",
               &pvp_g, &pvp_x, &pvp_o, &pvp_d,
               &ai_g, &ai_x, &ai_o, &ai_d);
        fclose(f);
    }
    else
    {
        // If no file, initialize to 0
        pvp_g = pvp_x = pvp_o = pvp_d = 0;
        ai_g = ai_x = ai_o = ai_d = 0;
    }

    // Reset PvP only
    pvp_g = pvp_x = pvp_o = pvp_d = 0;

    // Write updated stats back
    f = fopen(STATS_FILE, "w");
    if (f)
    {
        fprintf(f, "%d %d %d %d %d %d %d %d\n",
                pvp_g, pvp_x, pvp_o, pvp_d,
                ai_g, ai_x, ai_o, ai_d);
        fclose(f);
    }
}
