// minimax.c — simple AI for Tic-Tac-Toe (student style)
#include <stdlib.h> // rand()

// check if player p has a 3-in-a-row
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

// any empty cell left?
static int has_empty(const char b[9]) {
    for (int i = 0; i < 9; i++)
        if (b[i] != 'X' && b[i] != 'O') return 1;
    return 0;
}

// simple terminal eval from O's point: O win=+10, X win=-10, else 0
static int eval(const char b[9]) {
    if (winBy((char*)b, 'O')) return 10;
    if (winBy((char*)b, 'X')) return -10;
    return 0;
}

// minimax with optional depth limit (maxDepth=0 means no limit)
// isMax=1 => O to move (maximize), isMax=0 => X to move (minimize)
static int minimax_limited(char b[9], int isMax, int depth, int maxDepth) {
    int s = eval(b);
    if (s == 10)  return s - depth; // faster win better
    if (s == -10) return s + depth; // slower loss better
    if (!has_empty(b)) return 0;    // draw
    if (maxDepth > 0 && depth >= maxDepth) return 0;

    if (isMax) { // O moves
        int best = -1000;
        for (int i = 0; i < 9; i++) if (b[i] != 'X' && b[i] != 'O') {
            char keep = b[i]; b[i] = 'O';
            int val = minimax_limited(b, 0, depth + 1, maxDepth);
            b[i] = keep;
            if (val > best) best = val;
        }
        return best;
    } else { // X moves
        int best = 1000;
        for (int i = 0; i < 9; i++) if (b[i] != 'X' && b[i] != 'O') {
            char keep = b[i]; b[i] = 'X';
            int val = minimax_limited(b, 1, depth + 1, maxDepth);
            b[i] = keep;
            if (val < best) best = val;
        }
        return best;
    }
}

// level: 1=Easy, 2=Medium, 3=Hard
// returns best move index for O (0..8), or -1 if none
int findBestMoveLvl(char b[9], int level) {
    int freeIdx[9], n = 0;
    for (int i = 0; i < 9; i++) if (b[i] != 'X' && b[i] != 'O') freeIdx[n++] = i;
    if (n == 0) return -1;

    if (level == 1) { // Easy: 50% random, else 1-ply lookahead
        if (rand() % 100 < 50) return freeIdx[rand() % n];
        int best = -1000, move = freeIdx[0];
        for (int k = 0; k < n; k++) {
            int i = freeIdx[k];
            char keep = b[i]; b[i] = 'O';
            int val = minimax_limited(b, 0, 0, 1);
            b[i] = keep;
            if (val > best) { best = val; move = i; }
        }
        return move;
    }

    if (level == 2) { // Medium: depth 3 + a bit of randomness
        int best = -1000, second = -1000;
        int bestMove = freeIdx[0], secondMove = freeIdx[0];
        for (int k = 0; k < n; k++) {
            int i = freeIdx[k];
            char keep = b[i]; b[i] = 'O';
            int val = minimax_limited(b, 0, 0, 3);
            b[i] = keep;
            if (val > best) { second = best; secondMove = bestMove; best = val; bestMove = i; }
            else if (val > second) { second = val; secondMove = i; }
        }
        int r = rand() % 100;
        if (r < 20 && n >= 2) return secondMove; // sometimes pick 2nd best
        if (r < 30) return freeIdx[rand() % n];  // sometimes pick random
        return bestMove;
    }

    // Hard: full search
    int best = -1000, move = freeIdx[0];
    for (int k = 0; k < n; k++) {
        int i = freeIdx[k];
        char keep = b[i]; b[i] = 'O';
        int val = minimax_limited(b, 0, 0, 0); // no depth cap
        b[i] = keep;
        if (val > best) { best = val; move = i; }
    }
    return move;
}
