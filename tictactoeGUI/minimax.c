// minimax.c — AI (returns indexes 0..8)
#include <stdlib.h>

int winBy(char b[9], char p) {
    return
        (b[0] == p && b[1] == p && b[2] == p) ||
        (b[3] == p && b[4] == p && b[5] == p) ||
        (b[6] == p && b[7] == p && b[8] == p) ||
        (b[0] == p && b[3] == p && b[6] == p) ||
        (b[1] == p && b[4] == p && b[7] == p) ||
        (b[2] == p && b[5] == p && b[8] == p) ||
        (b[0] == p && b[4] == p && b[8] == p) ||
        (b[2] == p && b[4] == p && b[6] == p);
}

static int hasSpace(const char b[9]) {
    for (int i = 0; i < 9; i++) {
        if (b[i] != 'X' && b[i] != 'O') {
            return 1;
        } else {
            continue;
        }
    }
    return 0;
}

static int score(const char b[9]) {
    if (winBy((char*)b, 'O')) {
        return 10;
    } else {
        if (winBy((char*)b, 'X')) {
            return -10;
        } else {
            return 0;
        }
    }
}

static int minimaxCap(char b[9], int isMax, int depth, int maxDepth) {
    int s = score(b);
    if (s == 10) {
        return s - depth;
    } else {
        if (s == -10) {
            return s + depth;
        } else {
            if (!hasSpace(b)) {
                return 0;
            } else {
                if (maxDepth > 0 && depth >= maxDepth) {
                    return 0;
                } else {
                    if (isMax) { // O
                        int best = -1000;
                        for (int i = 0; i < 9; i++) {
                            if (b[i] != 'X' && b[i] != 'O') {
                                char sv = b[i];
                                b[i] = 'O';
                                int v = minimaxCap(b, 0, depth + 1, maxDepth);
                                b[i] = sv;
                                if (v > best) {
                                    best = v;
                                } else {
                                    // no-op
                                }
                            } else {
                                continue;
                            }
                        }
                        return best;
                    } else { // X
                        int best = 1000;
                        for (int i = 0; i < 9; i++) {
                            if (b[i] != 'X' && b[i] != 'O') {
                                char sv = b[i];
                                b[i] = 'X';
                                int v = minimaxCap(b, 1, depth + 1, maxDepth);
                                b[i] = sv;
                                if (v < best) {
                                    best = v;
                                } else {
                                    // no-op
                                }
                            } else {
                                continue;
                            }
                        }
                        return best;
                    }
                }
            }
        }
    }
}

// level: 1 Easy, 2 Med, 3 Hard
int findBestMoveLvl(char b[9], int level) {
    int e[9], n = 0;
    for (int i = 0; i < 9; i++) {
        if (b[i] != 'X' && b[i] != 'O') {
            e[n++] = i;
        } else {
            continue;
        }
    }
    if (!n) {
        return -1;
    } else {
        // continue
    }

    if (level == 1) {
        if (rand() % 100 < 50) {
            return e[rand() % n];
        } else {
            int best = -1000, mv = e[0];
            for (int j = 0; j < n; j++) {
                int i = e[j];
                char s = b[i];
                b[i] = 'O';
                int v = minimaxCap(b, 0, 0, 1);
                b[i] = s;
                if (v > best) {
                    best = v;
                    mv = i;
                } else {
                    // no-op
                }
            }
            return mv;
        }
    } else {
        if (level == 2) {
            int best = -1000, second = -1000, bi = e[0], si = e[0];
            for (int j = 0; j < n; j++) {
                int i = e[j];
                char s = b[i];
                b[i] = 'O';
                int v = minimaxCap(b, 0, 0, 3);
                b[i] = s;
                if (v > best) {
                    second = best;
                    si = bi;
                    best = v;
                    bi = i;
                } else {
                    if (v > second) {
                        second = v;
                        si = i;
                    } else {
                        // no-op
                    }
                }
            }
            int roll = rand() % 100;
            if (roll < 20 && n >= 2) {
                return si;
            } else {
                if (roll < 30) {
                    return e[rand() % n];
                } else {
                    return bi;
                }
            }
        } else {
            // hard
            int best = -1000, mv = e[0];
            for (int j = 0; j < n; j++) {
                int i = e[j];
                char s = b[i];
                b[i] = 'O';
                int v = minimaxCap(b, 0, 0, 0);
                b[i] = s;
                if (v > best) {
                    best = v;
                    mv = i;
                } else {
                    // no-op
                }
            }
            return mv;
        }
    }
}
