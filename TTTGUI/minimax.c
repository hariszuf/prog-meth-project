#include <stdlib.h>   // for rand()

// Check if player p ('X' or 'O') has any winning line on board b
int winBy(char b[9], char p)
{
    // 3 rows
    if (b[0] == p && b[1] == p && b[2] == p) return 1;
    if (b[3] == p && b[4] == p && b[5] == p) return 1;
    if (b[6] == p && b[7] == p && b[8] == p) return 1;

    // 3 columns
    if (b[0] == p && b[3] == p && b[6] == p) return 1;
    if (b[1] == p && b[4] == p && b[7] == p) return 1;
    if (b[2] == p && b[5] == p && b[8] == p) return 1;

    // 2 diagonals
    if (b[0] == p && b[4] == p && b[8] == p) return 1;
    if (b[2] == p && b[4] == p && b[6] == p) return 1;

    return 0;  // no winning line
}

// Check if board has at least one empty space (not 'X' or 'O')
static int has_space(const char b[9])
{
    for (int i = 0; i < 9; i++)
    {
        if (b[i] != 'X' && b[i] != 'O')
        {
            return 1;          // found at least one empty cell
        }
    }
    return 0;                  // board is full
}

// Evaluate board: +10 = O wins, -10 = X wins, 0 = none/draw
static int eval(const char b[9])
{
    if (winBy((char*)b, 'O'))
    {
        return 10;             // good for O
    }
    else if (winBy((char*)b, 'X'))
    {
        return -10;            // bad for O
    }
    else
    {
        return 0;              // draw or non-terminal (handled above in minimax)
    }
}

// Minimax with optional depth cap (maxDepth==0 means full search)
static int minimax_cap(char b[9], int isMax, int depth, int maxDepth)
{
    int score;
    int i;
    int best;
    int value;
    char save;

    // First, check terminal states (win / loss / draw)
    score = eval(b);

    if (score == 10)           // O has a win on board
    {
        return score - depth;  // earlier wins are better
    }
    else if (score == -10)     // X has a win on board
    {
        return score + depth;  // later losses are slightly less bad
    }
    else if (!has_space(b))    // no empty cells → draw
    {
        return 0;
    }

    // Apply depth limit if maxDepth > 0
    if (maxDepth > 0 && depth >= maxDepth)
    {
        // At depth cap we stop looking further.
        // Returning 0 means “treat it as unknown / neutral”.
        return 0;
    }

    // Maximizing player: O
    if (isMax)
    {
        best = -1000;          // very low initial score

        for (i = 0; i < 9; i++)
        {
            if (b[i] != 'X' && b[i] != 'O')   // only try empty cells
            {
                save = b[i];
                b[i] = 'O';                   // O plays here

                // Next turn is X (minimizing), depth+1
                value = minimax_cap(b, 0, depth + 1, maxDepth);

                b[i] = save;                  // undo move

                if (value > best)
                {
                    best = value;             // keep best score for O
                }
            }
        }
        return best;
    }
    // Minimizing player: X
    else
    {
        best = 1000;           // very high initial score

        for (i = 0; i < 9; i++)
        {
            if (b[i] != 'X' && b[i] != 'O')   // only try empty cells
            {
                save = b[i];
                b[i] = 'X';                   // X plays here

                // Next turn is O (maximizing), depth+1
                value = minimax_cap(b, 1, depth + 1, maxDepth);

                b[i] = save;                  // undo move

                if (value < best)
                {
                    best = value;             // keep lowest score for X
                }
            }
        }
        return best;
    }
}

// Choose a move for 'O' given level: 1=Easy, 2=Medium, 3=Hard
int findBestMoveLvl(char b[9], int level)
{
    int empty[9];     // indices of empty cells
    int n;            // number of empty cells
    int i, j;
    int best, second;
    int move, sec;
    int value;
    char save;

    // Collect all empty cell indices
    n = 0;
    for (i = 0; i < 9; i++)
    {
        if (b[i] != 'X' && b[i] != 'O')
        {
            empty[n] = i;
            n++;
        }
    }

    if (n == 0)
    {
        return -1;    // no legal moves
    }

    // -------- Level 1: Easy (50% random, 50% shallow minimax) --------
    if (level == 1)
    {
        if ((rand() % 100) < 50)
        {
            // Half the time: completely random move
            return empty[rand() % n];
        }
        else
        {
            
            best = -1000;
            move = empty[0];

            for (j = 0; j < n; j++)
            {
                i = empty[j];
                save = b[i];
                b[i] = 'O';

                // Depth cap 1
                value = minimax_cap(b, 0, 0, 1);

                b[i] = save;

                if (value > best)
                {
                    best = value;
                    move = i;
                }
            }

            return move;
        }
    }

    // -------- Level 2: Medium (mixture of best, second-best, random) --------
    if (level == 2)
    {
        best = -1000;
        second = -1000;
        move = empty[0];
        sec = empty[0];

        // Evaluate all empty cells with a deeper but still capped search (depth 3)
        for (j = 0; j < n; j++)
        {
            i = empty[j];
            save = b[i];
            b[i] = 'O';

            value = minimax_cap(b, 0, 0, 3);

            b[i] = save;

            if (value > best)
            {
                // Shift best to second-best
                second = best;
                sec = move;

                best = value;
                move = i;
            }
            else if (value > second)
            {
                second = value;
                sec = i;
            }
        }

        // Randomly decide whether to pick best, second-best, or a random move
        i = rand() % 100;
        if (i < 20 && n >= 2)
        {
            // 20% chance: second-best move 
            return sec;
        }
        else if (i < 30)
        {
            // Next 10%: random move
            return empty[rand() % n];
        }
        else
        {
            // 70%: best minimax move
            return move;
        }
    }

    // -------- Level 3: Hard (full minimax, no depth cap) --------
    // At this level, the AI plays perfectly.
    best = -1000;
    move = empty[0];

    for (j = 0; j < n; j++)
    {
        i = empty[j];
        save = b[i];
        b[i] = 'O';

        // maxDepth = 0  
        value = minimax_cap(b, 0, 0, 0);

        b[i] = save;

        if (value > best)
        {
            best = value;
            move = i;
        }
    }

    return move;
}
