// minimax.c — simple Tic-Tac-Toe AI with optional depth caps
#include <stdlib.h>          // for rand()

// Check if player p ('X' or 'O') has any winning line on board b
int winBy(char b[9], char p)
{
    // check all 8 lines (3 rows, 3 cols, 2 diagonals)
    if (b[0] == p && b[1] == p && b[2] == p)
    {
        return 1;
    }
    else
    {
        if (b[3] == p && b[4] == p && b[5] == p)
        {
            return 1;
        }
        else
        {
            if (b[6] == p && b[7] == p && b[8] == p)
            {
                return 1;
            }
            else
            {
                if (b[0] == p && b[3] == p && b[6] == p)
                {
                    return 1;
                }
                else
                {
                    if (b[1] == p && b[4] == p && b[7] == p)
                    {
                        return 1;
                    }
                    else
                    {
                        if (b[2] == p && b[5] == p && b[8] == p)
                        {
                            return 1;
                        }
                        else
                        {
                            if (b[0] == p && b[4] == p && b[8] == p)
                            {
                                return 1;
                            }
                            else
                            {
                                if (b[2] == p && b[4] == p && b[6] == p)
                                {
                                    return 1;
                                }
                                else
                                {
                                    return 0; // no winning line
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

// Helper: return 1 if there is at least one empty cell
static int has_space(const char b[9])
{
                   
    for (int i = 0; i < 9; i++)                 // scan all cells
    {
        if (b[i] != 'X' && b[i] != 'O')     // if we find a non-X/O
        {
            return 1;                       // there is space
        }
    }
    return 0;                               // no space left
}

// Helper: evaluate terminal board; positive is good for 'O'
static int eval(const char b[9])
{
    if (winBy((char*)b, 'O'))               // if O wins
    {
        return 10;                          // good score
    }
    else
    {
        if (winBy((char*)b, 'X'))           // if X wins
        {
            return -10;                     // bad score for O
        }
        else
        {
            return 0;                       // draw or not terminal
        }
    }
}

// Recursive minimax with optional depth cap (maxDepth==0 means full search)
static int minimax_cap(char b[9], int isMax, int depth, int maxDepth)
{
    int score;                              // current board evaluation
    int i;                                  // loop counter
    int best;                               // best score for this player
    int value;                              // score of a particular move
    char save;                              // to undo move

    score = eval(b);                        // check if someone already won

    if (score == 10)                        // O has a win on board
    {
        return score - depth;               // prefer quicker wins
    }
    else
    {
        if (score == -10)                   // X has a win on board
        {
            return score + depth;           // prefer slower losses
        }
        else
        {
            if (!has_space(b))              // draw (no space)
            {
                return 0;                   // neutral score
            }
            else
            {
                if (maxDepth > 0)           // use a depth limit?
                {
                    if (depth >= maxDepth)  // reached the cap
                    {
                        return 0;           // return neutral at cap
                    }
                }
            }
        }
    }

    if (isMax)                              // O's turn (maximize score)
    {
        best = -1000;                       // start with very low
        for (i = 0; i < 9; i++)             // try all cells
        {
            if (b[i] == 'X' || b[i] == 'O') // skip taken cells
            {
                // do nothing for taken cells
            }
            else
            {
                save = b[i];                // remember old value
                b[i] = 'O';                 // make O's move
                value = minimax_cap(b, 0, depth + 1, maxDepth); // now X's turn
                b[i] = save;                // undo the move

                if (value > best)           // found better score
                {
                    best = value;           // update best
                }
            }
        }
        return best;                        // best achievable for O
    }
    else                                    // X's turn (minimize score)
    {
        best = 1000;                        // start with very high
        for (i = 0; i < 9; i++)             // try all cells
        {
            if (b[i] == 'X' || b[i] == 'O') // skip taken cells
            {
                // do nothing for taken cells
            }
            else
            {
                save = b[i];                // remember old value
                b[i] = 'X';                 // make X's move
                value = minimax_cap(b, 1, depth + 1, maxDepth); // now O's turn
                b[i] = save;                // undo the move

                if (value < best)           // found lower score
                {
                    best = value;           // update best (minimize)
                }
            }
        }
        return best;                        // best achievable for X (as minimizer)
    }
}

// Choose a move for 'O' given level: 1=Easy, 2=Medium, 3=Hard
int findBestMoveLvl(char b[9], int level)
{
    int empty[9];                           // list of empty cell indices
    int n;                                  // count of empty cells
    int i;                                  // general loop
    int j;                                  // loop over empty cells
    int best;                               // best score encountered
    int second;                             // second-best score (for medium)
    int move;                               // chosen move index
    int sec;                                // second-best move index
    int value;                              // score for a candidate move
    char save;                              // to undo move

    n = 0;                                  // start with 0 empty cells
    for (i = 0; i < 9; i++)                 // scan all cells
    {
        if (b[i] != 'X' && b[i] != 'O')     // if cell is empty
        {
            empty[n] = i;                   // store its index
            n = n + 1;                      // increase count
        }
    }

    if (n == 0)                             // no legal moves
    {
        return -1;                          // signal failure
    }

    // Level 1: mostly random, sometimes shallow lookahead
    if (level == 1)
    {
        if ((rand() % 100) < 50)            // 50% chance: pick random empty
        {
            return empty[rand() % n];       // return random empty cell
        }
        else
        {
            best = -1000;                   // else use shallow search
            move = empty[0];                // initialize to something valid

            for (j = 0; j < n; j++)         // try each empty position
            {
                i = empty[j];               // candidate index
                save = b[i];                // remember old char
                b[i] = 'O';                 // try placing O
                value = minimax_cap(b, 0, 0, 1); // X responds, depth cap 1
                b[i] = save;                // undo

                if (value > best)           // found higher score
                {
                    best = value;           // update best score
                    move = i;               // update move index
                }
            }

            return move;                    // return best shallow move
        }
    }
    else
    {
        // Level 2: usually best, sometimes second, sometimes random
        if (level == 2)
        {
            best = -1000;                   // best score so far
            second = -1000;                 // second-best score
            move = empty[0];                // best move index
            sec = empty[0];                 // second move index

            for (j = 0; j < n; j++)         // check all candidates
            {
                i = empty[j];               // candidate index
                save = b[i];                // remember old char
                b[i] = 'O';                 // try placing O
                value = minimax_cap(b, 0, 0, 3); // search up to depth 3
                b[i] = save;                // undo

                if (value > best)           // if better than current best
                {
                    second = best;          // old best becomes second
                    sec = move;             // old best move index becomes second
                    best = value;           // update best score
                    move = i;               // update best move index
                }
                else
                {
                    if (value > second)     // maybe it is second-best
                    {
                        second = value;     // update second score
                        sec = i;            // update second index
                    }
                }
            }

            i = rand() % 100;               // choose behavior by probability
            if (i < 20 && n >= 2)           // 20% choose second-best if exists
            {
                return sec;
            }
            else
            {
                if (i < 30)                 // next 10% choose random
                {
                    return empty[rand() % n];
                }
                else
                {
                    return move;            // otherwise choose best
                }
            }
        }
        else
        {
            // Level 3: full minimax search (no depth cap)
            best = -1000;                   // start low
            move = empty[0];                // initialize to valid position

            for (j = 0; j < n; j++)         // try each empty cell
            {
                i = empty[j];               // candidate index
                save = b[i];                // remember old char
                b[i] = 'O';                 // try placing O
                value = minimax_cap(b, 0, 0, 0); // full search
                b[i] = save;                // undo

                if (value > best)           // better score found
                {
                    best = value;           // update best score
                    move = i;               // update move index
                }
            }

            return move;                    // chosen move for hard level
        }
    }
}
