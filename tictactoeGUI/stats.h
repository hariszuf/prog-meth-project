// stats.h — separate categories (PvP, PvC)
#ifndef STATS_H
#define STATS_H

typedef enum { STATS_PVP = 0, STATS_PVC = 1 } StatsMode;

// winner codes: 1 = X, 2 = O, 0 = draw
void stats_record_result_mode(StatsMode mode, int winner);
void stats_get_counts_mode(StatsMode mode, int *games, int *x_wins, int *o_wins, int *draws);

#endif
