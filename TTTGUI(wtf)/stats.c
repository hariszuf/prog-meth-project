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
} StatsCat;

// Struct containing both categories
typedef struct {
    StatsCat pvp;   // Player vs Player statistics
    StatsCat pvai;  // Player vs AI statistics
} StatsAll;

/*---------------------------------------------
   Helper Function: zero_all
   Purpose: set all values in the StatsAll struct to zero
----------------------------------------------*/
static void zero_all(StatsAll *all)
{
    all->pvp.games = 0;
    all->pvp.x_wins = 0;
    all->pvp.o_wins = 0;
    all->pvp.draws = 0;

    all->pvai.games = 0;
    all->pvai.x_wins = 0;
    all->pvai.o_wins = 0;
    all->pvai.draws = 0;
}

/*---------------------------------------------
   Helper Function: load_all
   Purpose: read the statistics from file into memory
----------------------------------------------*/
static void load_all(StatsAll *all)
{
    FILE *f;   // file pointer
    int n;     // number of values successfully read

    // set all stats to zero first
    zero_all(all);

    // open file for reading
    f = fopen(STATS_FILE, "r");

    // if the file cannot be opened, just return (stats remain zero)
    if (f == NULL)
    {
        return;
    }
    else
    {
        // try to read 8 numbers: 4 for PvP + 4 for PvAI
        n = fscanf(f, "%d %d %d %d %d %d %d %d",
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
static void save_all(const StatsAll *all)
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
    StatsAll all;      // holds both PvP and PvAI stats
    StatsCat *cat;     // pointer to selected category

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
    cat->games = cat->games + 1;

    // update according to winner
    if (winner == 1)
    {
        // X wins
        cat->x_wins = cat->x_wins + 1;
    }
    else if (winner == 2)
    {
        // O wins
        cat->o_wins = cat->o_wins + 1;
    }
    else
    {
        // draw
        cat->draws = cat->draws + 1;
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
    StatsAll all;    // holds all statistics
    StatsCat *cat;   // pointer to the correct category

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
