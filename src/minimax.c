// minimax.c
#include <stdio.h>
#include <stdlib.h>

int winBy(char b[9], char p) {
    return
        (b[0]==p && b[1]==p && b[2]==p) ||
        (b[3]==p && b[4]==p && b[5]==p) ||
        (b[6]==p && b[7]==p && b[8]==p) ||
        (b[0]==p && b[3]==p && b[6]==p) ||
        (b[1]==p && b[4]==p && b[7]==p) ||
        (b[2]==p && b[5]==p && b[8]==p) ||
        (b[0]==p && b[4]==p && b[8]==p) ||
        (b[2]==p && b[4]==p && b[6]==p);
}

static int boardHasSpace(char b[9]) {
    for (int i = 0; i < 9; i++)
        if (b[i] != 'X' && b[i] != 'O') return 1;
    return 0;
}

static int scoreBoard(char b[9]) {
    if (winBy(b, 'O')) return 10;
    if (winBy(b, 'X')) return -10;
    return 0;
}

// minimax with optional depth cap (maxDepth<=0 means unlimited)
static int minimaxCap(char b[9], int isMax, int depth, int maxDepth) {
    int s = scoreBoard(b);
    if (s == 10)  return s - depth;
    if (s == -10) return s + depth;
    if (!boardHasSpace(b)) return 0;
    if (maxDepth > 0 && depth >= maxDepth) return 0; // truncate search

    if (isMax) {
        int best = -1000;
        for (int i = 0; i < 9; i++) {
            if (b[i] != 'X' && b[i] != 'O') {
                char save = b[i];
                b[i] = 'O';
                int val = minimaxCap(b, 0, depth+1, maxDepth);
                b[i] = save;
                if (val > best) best = val;
            }
        }
        return best;
    } else {
        int best = 1000;
        for (int i = 0; i < 9; i++) {
            if (b[i] != 'X' && b[i] != 'O') {
                char save = b[i];
                b[i] = 'X';
                int val = minimaxCap(b, 1, depth+1, maxDepth);
                b[i] = save;
                if (val < best) best = val;
            }
        }
        return best;
    }
}

// level: 1=easy (very fallible), 2=medium (some mistakes), 3=hard (optimal)
int findBestMoveLvl(char b[9], int level) {
    int empties[9], ecount = 0;
    for (int i = 0; i < 9; i++)
        if (b[i] != 'X' && b[i] != 'O') empties[ecount++] = i;

    if (ecount == 0) return 1; // shouldn't happen

    // EASY: 50% random; else shallow lookahead (depth=1)
    if (level == 1) {
        if (rand() % 100 < 50) {
            int k = rand() % ecount;
            return empties[k] + 1;
        }
        int best = -1000, mv = empties[0];
        for (int j = 0; j < ecount; j++) {
            int i = empties[j];
            char s = b[i];
            b[i] = 'O';
            int val = minimaxCap(b, 0, 0, 1);
            b[i] = s;
            if (val > best) { best = val; mv = i; }
        }
        return mv + 1;
    }

    // MEDIUM: depth cap + occasional "mistake"
    if (level == 2) {
        // score moves with depth cap = 3
        int scores[9]; // align with empties[]
        int best = -1000, second = -1000, bi = empties[0], si = empties[0];
        for (int j = 0; j < ecount; j++) {
            int i = empties[j];
            char s = b[i];
            b[i] = 'O';
            int val = minimaxCap(b, 0, 0, 3);
            b[i] = s;
            scores[j] = val;
            if (val > best) { second = best; si = bi; best = val; bi = i; }
            else if (val > second) { second = val; si = i; }
        }
        int roll = rand() % 100;
        if (roll < 20 && ecount >= 2) {          // 20% pick second best
            return si + 1;
        } else if (roll < 30) {                  // next 10% random
            int k = rand() % ecount;
            return empties[k] + 1;
        } else {                                 // 70% best
            return bi + 1;
        }
    }

    // HARD: full minimax (optimal)
    {
        int best = -1000, mv = empties[0];
        for (int j = 0; j < ecount; j++) {
            int i = empties[j];
            char s = b[i];
            b[i] = 'O';
            int val = minimaxCap(b, 0, 0, 0); // unlimited
            b[i] = s;
            if (val > best) { best = val; mv = i; }
        }
        return mv + 1;
    }
}

// Backward compatibility if you still call findBestMove(b)
int findBestMove(char b[9]) {
    return findBestMoveLvl(b, 3);
}
