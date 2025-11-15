// stats.c — keeps separate statistics for PvP and PvAI
#include <stdio.h>      // for file operations: fopen, fscanf, fprintf, fclose
#include "stats.h"      // include our own header for function prototypes and enums
#include <time.h>
#include <windows.h>
#include <psapi.h>
// File name to store all statistics
#define STATS_FILE "tictactoe_stats.txt"
#define AI_TIME_FILE "ai_timing.txt"



// Return the current process memory usage in kilobytes
static double get_memory_kb(void)
{
    PROCESS_MEMORY_COUNTERS pmc; // struct to hold memory info

    // Retrieve memory information for the current process
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        // Convert bytes (WorkingSetSize) to kilobytes
        return (double)pmc.WorkingSetSize / 1024.0;   // in KB
    }
    return 0.0;
}




// Record AI performance data to log file
void stats_log_ai_move(int mode, int level, int move_no, double ms)
{
    FILE *f = fopen(AI_TIME_FILE, "a");

    if (!f) {
        return; 
    }

    time_t now = time(NULL);

    struct tm *lt = localtime(&now);

    char ts[32];

    strftime(ts, sizeof ts, "%Y-%m-%d %H:%M:%S", lt);

    const char *levelmode;

    if (level == 1) {
        levelmode = "Easy";
    }
    else if (level == 2) {
        levelmode = "Medium";
    }
    else if (level == 3) {
        levelmode = "Hard";
    }
    else {
        levelmode = "Unknown";
    }

    double mem_kb = get_memory_kb();

    fprintf(f, "%s, mode=%s, level=%s, move=%d, ms=%.3f, mem=%.2fKB\n",
            ts,
            (mode == 0 ? "PVP" : "PVAI"),
            levelmode,
            move_no,
            ms,
            mem_kb);

    fclose(f);
}


// Each category keeps total games, wins for X, wins for O, and draws
typedef struct {
    int games;      // total games played
    int x_wins;     // total X wins
    int o_wins;     // total O wins
    int draws;      // total draws
} Stats;

// PvAI now has 3 difficulty levels
typedef struct {
    Stats easy;
    Stats medium;
    Stats hard;
} PvAIStats;

// Struct containing both categories
typedef struct {
    Stats pvp;       // Player vs Player statistics
    PvAIStats pvai;  // Player vs AI statistics (split by difficulty)
} AllStats;

static void save_all(AllStats *all)
{
    FILE *f = fopen(STATS_FILE, "w");
    if (!f){
        return;
    } 

    // Write all numbers in order
    fprintf(f,
        "%d %d %d %d "      // PvP
        "%d %d %d %d "      // Easy
        "%d %d %d %d "      // Medium
        "%d %d %d %d\n",    // Hard
        all->pvp.games, all->pvp.x_wins, all->pvp.o_wins, all->pvp.draws,
        all->pvai.easy.games, all->pvai.easy.x_wins, all->pvai.easy.o_wins, all->pvai.easy.draws,
        all->pvai.medium.games, all->pvai.medium.x_wins, all->pvai.medium.o_wins, all->pvai.medium.draws,
        all->pvai.hard.games, all->pvai.hard.x_wins, all->pvai.hard.o_wins, all->pvai.hard.draws);

    fclose(f);
}

static void load_all(AllStats *all)
{
    FILE *f = fopen(STATS_FILE, "r");
    if (!f) {
        *all = (AllStats){0};
        return;
    }

    fscanf(f,
        "%d %d %d %d "
        "%d %d %d %d "
        "%d %d %d %d "
        "%d %d %d %d",
        &all->pvp.games, &all->pvp.x_wins, &all->pvp.o_wins, &all->pvp.draws,
        &all->pvai.easy.games, &all->pvai.easy.x_wins, &all->pvai.easy.o_wins, &all->pvai.easy.draws,
        &all->pvai.medium.games, &all->pvai.medium.x_wins, &all->pvai.medium.o_wins, &all->pvai.medium.draws,
        &all->pvai.hard.games, &all->pvai.hard.x_wins, &all->pvai.hard.o_wins, &all->pvai.hard.draws);

    fclose(f);
}

// update of stats after a game ends
void stats_record_result_mode(StatsMode mode, int level, int winner)
{
    AllStats all;
    load_all(&all);
    Stats *cat = NULL;

    if (mode == STATS_PVP) {
        cat = &all.pvp;
    } 
    else if (mode == STATS_PVAI) {
        if (level == 1) {
            cat = &all.pvai.easy;
        }
        else if (level == 2) {
            cat = &all.pvai.medium;
        }
        else if (level == 3) {
            cat = &all.pvai.hard;
        }
        else {
            return; // invalid level
        }
    }
    else {
        return; // invalid mode
    }

    cat->games++;

    if (winner == 1) {
        cat->x_wins++;
    }
    else if (winner == 2) {
        cat->o_wins++;
    }
    else {
        cat->draws++;
    }

    save_all(&all);
}

// return stats of the given mode and level
void stats_get_counts_mode(StatsMode mode, int level, int *games, int *x_wins, int *o_wins, int *draws) {
    AllStats all;     
    load_all(&all);   
    Stats *cat = NULL;

    if (mode == STATS_PVP) {    
        cat = &all.pvp;         
    }
    else if (mode == STATS_PVAI) {  
        if (level == 1) {         
            cat = &all.pvai.easy;
        }
        else if (level == 2) {      
            cat = &all.pvai.medium;
        }
        else if (level == 3) {      
            cat = &all.pvai.hard;
        }
        else {                      
            cat = NULL; // invalid level
        }
    }
    else {
        cat = NULL; // invalid mode
    }

    if (cat != NULL) {
        if (games != NULL)  { *games  = cat->games; }
        if (x_wins != NULL) { *x_wins = cat->x_wins; }
        if (o_wins != NULL) { *o_wins = cat->o_wins; }
        if (draws != NULL)  { *draws  = cat->draws; }
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
