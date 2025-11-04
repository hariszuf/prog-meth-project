// stats.h — public functions to record and read stats
#ifndef STATS_H
#define STATS_H

// Two categories: Player vs Player, Player vs AI
typedef enum {
    STATS_PVP = 0,              // PvP mode
    STATS_PVAI = 1              // PvAI mode
} StatsMode;

// Record the result of one finished game in the given mode
// winner: 1 = X won, 2 = O won, 0 = draw
void stats_record_result_mode(StatsMode mode, int winner);

// Read the counts back (any pointer may be NULL if caller does not need it)
void stats_get_counts_mode(StatsMode mode,int *games,int *x_wins,int *o_wins,int *draws);

void stats_reset_pvp(void);
#endif // STATS_H