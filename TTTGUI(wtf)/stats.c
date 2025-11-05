// stats.c — keeps separate statistics for PvP and PvAI
#include <stdio.h>      // for file operations: fopen, fscanf, fprintf, fclose
#include "stats.h"      // include our own header for function prototypes and enums
#include <time.h>
#include <windows.h>
#include <psapi.h>
// File name to store all statistics
#define STATS_FILE "tictactoe_stats.txt"
#define AI_TIME_FILE "ai_timing.txt"


// Function: get_memory_kb
// Purpose:  Return the current process memory usage (Working Set) in kilobytes
static double get_memory_kb(void)
{
    PROCESS_MEMORY_COUNTERS pmc;                      // struct to hold memory info

    // Retrieve memory information about the current process
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        // Convert bytes (WorkingSetSize) to kilobytes for easier reading
        return (double)pmc.WorkingSetSize / 1024.0;   
    }

    // If memory info could not be obtained, return 0
    return 0.0;
}



// Function: stats_log_ai_move
// Purpose:  Record AI performance data (timing and memory) to a log file
void stats_log_ai_move(int mode, int level, int move_no, double ms)
{
    // Open the performance log file in append mode
    FILE *f = fopen(AI_TIME_FILE, "a");

    // If file cannot be opened, exit function early
    if (!f) return;

    // Get the current time (in seconds since 1970)
    time_t now = time(NULL);

    // Convert time to local time structure
    struct tm *lt = localtime(&now);

    // Character array to store formatted timestamp
    char ts[32];

    // Format time into a readable string like "2025-11-05 18:25:45"
    strftime(ts, sizeof ts, "%Y-%m-%d %H:%M:%S", lt);

    // Declare a string pointer to hold difficulty level text
    const char *levelmode;

    // Determine difficulty name based on level value
    if (level == 1)
        levelmode = "Easy";
    else if (level == 2)
        levelmode = "Medium";
    else if (level == 3)
        levelmode = "Hard";
    else
        levelmode = "Unknown";  // if an unexpected value appears

    // Get current process memory usage in KB
    double mem_kb = get_memory_kb();

    // Example output line (for reference):
    // 2025-11-04 09:32:18, mode=PVAI, level=Hard, move=7, ms=4.317, mem=28312.44KB

    // Write a line of data into the log file
    fprintf(f, "%s, mode=%s, level=%s, move=%d, ms=%.3f, mem=%.2fKB\n",
            ts,                               // timestamp (date and time)
            (mode == 0 ? "PVP" : "PVAI"),     // determine if Player vs Player or Player vs AI
            levelmode,                        // difficulty string (Easy, Medium, Hard)
            move_no,                          // which move number this is
            ms,                               // time taken for AI move in milliseconds
            mem_kb);                          // memory used at the moment (in KB)

    // Close the file to save changes
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
    if (!f) return;

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

/*---------------------------------------------
   Function: stats_record_result_mode
   Purpose: update statistics after each game
----------------------------------------------*/
void stats_record_result_mode(StatsMode mode, int level, int winner)
{
    AllStats all;
    load_all(&all);
    Stats *cat = NULL;

    if (mode == STATS_PVP) {
        cat = &all.pvp;
    } 
    else if (mode == STATS_PVAI) {
        // choose by difficulty
        if (level == 1)
            cat = &all.pvai.easy;
        else if (level == 2)
            cat = &all.pvai.medium;
        else if (level == 3)
            cat = &all.pvai.hard;
        else
            return; // invalid level
    }
    else return; // invalid mode

    cat->games++;
    if (winner == 1)      cat->x_wins++;
    else if (winner == 2) cat->o_wins++;
    else                  cat->draws++;

    save_all(&all);
}

/*---------------------------------------------
   Function: stats_get_counts_mode
   Purpose: return the stats of a given mode
----------------------------------------------*/
void stats_get_counts_mode(StatsMode mode, int level, int *games, int *x_wins, int *o_wins, int *draws) {
    AllStats all;     // Struct to hold both PvP and PvAI stats
    load_all(&all);   // Load stats from the file
    Stats *cat = NULL; // Pointer to selected stats category

    if (mode == STATS_PVP) {      // If it's PvP mode
        cat = &all.pvp;           // Select PvP stats
    }
    else if (mode == STATS_PVAI) {  // If it's PvAI mode
        if (level == 1)           // Easy level
            cat = &all.pvai.easy;
        else if (level == 2)      // Medium level
            cat = &all.pvai.medium;
        else if (level == 3)      // Hard level
            cat = &all.pvai.hard;
    }

    if (cat != NULL) {
        // Populate the stats from the selected category
        if (games != NULL)  *games  = cat->games;
        if (x_wins != NULL) *x_wins = cat->x_wins;
        if (o_wins != NULL) *o_wins = cat->o_wins;
        if (draws != NULL)  *draws  = cat->draws;
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
