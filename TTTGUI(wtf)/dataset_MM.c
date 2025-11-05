// dataset_gen.c — write 9 cells + result (win/lose/draw) from X's perspective
#include <stdio.h>   // for file I/O functions like fopen, fprintf, fclose

// This function is from your project (in minimax.c)
// It checks if a given player ('X' or 'O') has won the game
int winBy(char b[9], char p); 

// -------------------- Helper functions --------------------

// Function to check if the board is completely filled (no empty spaces)
static int is_full(const char b[9]) {
    // loop through all 9 cells
    for (int i = 0; i < 9; i++) 
        // if there is any cell that is not X or O, board not full
        if (b[i] != 'X' && b[i] != 'O') 
            return 0;  // not full yet
    return 1;          // all cells filled
}

// Function to check if both players have won (which is an invalid state)
static int both_won(const char b[9]) {
    // if both X and O are winners, return true (1)
    return winBy((char*)b, 'X') && winBy((char*)b, 'O');
}

// Function to count Xs and Os and make sure the board state is reachable
static int counts_ok(const char b[9], int *cx, int *co) {
    int x = 0, o = 0; // counters for X and O

    // loop through the 9 board cells
    for (int i = 0; i < 9; i++) {
        if (b[i] == 'X') x++;        // count X
        else if (b[i] == 'O') o++;   // count O
        else if (b[i] != ' ') return 0; // invalid char
    }

    // store counts into pointers if they exist
    if (cx) *cx = x; 
    if (co) *co = o;

    // valid if X and O counts are equal or X has 1 more than O
    return (x == o) || (x == o + 1);
}

// -------------------- Core solver --------------------

// Recursive function to evaluate a board from X's perspective
// Returns +1 if X can force a win, -1 if O can force a win, or 0 for draw
static int solve_from_X(char b[9]) {
    // Base case: if X already wins, return +1
    if (winBy(b,'X')) return +1;
    // If O wins, return -1
    if (winBy(b,'O')) return -1;
    // If board is full and no winner, it’s a draw (0)
    if (is_full(b))   return  0;

    // Count Xs and Os to know whose turn it is
    int cx, co; 
    counts_ok(b, &cx, &co);

    // If counts are equal, it's X's turn; otherwise O's turn
    int x_turn = (cx == co); 

    // Initialize the "best" score outside possible range
    int best = x_turn ? -2 : +2; 

    // Loop over all 9 board positions
    for (int i = 0; i < 9; i++) {
        // Only act if cell is empty
        if (b[i] == ' ') {
            // Place a move for the current player
            b[i] = x_turn ? 'X' : 'O';

            // Recursively evaluate the resulting board
            int val = solve_from_X(b);

            // Undo the move (backtrack)
            b[i] = ' ';

            // If it's X's turn, X wants the max result
            if (x_turn) {                
                if (val > best) best = val;  // keep higher score
                if (best == +1) return +1;   // if X can win, stop searching
            } 
            // If it's O's turn, O wants to minimize X's score
            else {                      
                if (val < best) best = val;  // keep lower score
                if (best == -1) return -1;   // if O can win, stop searching
            }
        }
    }

    // Return final evaluation (if no move found, treat as draw)
    return best == -2 || best == +2 ? 0 : best; 
}

// -------------------- File writing --------------------

// Write one line of data like: x,x,o,b,o,b,b,o,x,win
static void write_row(FILE *f, const char b[9], int outcome) {
    // Loop through all 9 cells
    for (int i = 0; i < 9; i++) {
        // Convert board characters: 'X'→x, 'O'→o, ' '→b (blank)
        char c = b[i];
        char t = (c == 'X') ? 'x' : (c == 'O') ? 'o' : 'b';

        // Write to file, separated by commas
        fputc(t, f);
        fputc(',', f);
    }

    // Determine outcome label
    const char *lab = (outcome > 0) ? "win" : (outcome < 0) ? "lose" : "draw";

    // Write the label and newline
    fprintf(f, "%s\n", lab);
}

// -------------------- Main loop to go through all possible boards --------------------

static void enumerate_all(FILE *f) {
    // Possible cell values (empty, X, O)
    const char vals[3] = { ' ', 'X', 'O' };
    char b[9]; // board array

    // 9 nested loops for all 3^9 possible combinations
    for (int a0=0; a0<3; a0++){ b[0]=vals[a0];
    for (int a1=0; a1<3; a1++){ b[1]=vals[a1];
    for (int a2=0; a2<3; a2++){ b[2]=vals[a2];
    for (int a3=0; a3<3; a3++){ b[3]=vals[a3];
    for (int a4=0; a4<3; a4++){ b[4]=vals[a4];
    for (int a5=0; a5<3; a5++){ b[5]=vals[a5];
    for (int a6=0; a6<3; a6++){ b[6]=vals[a6];
    for (int a7=0; a7<3; a7++){ b[7]=vals[a7];
    for (int a8=0; a8<3; a8++){ b[8]=vals[a8];

        // Count X and O to ensure valid board state
        int cx, co;
        if (!counts_ok(b, &cx, &co)) continue; // skip invalid
        if (both_won(b)) continue;             // skip impossible

        // Evaluate board (+1/-1/0)
        int outcome = solve_from_X(b);

        // Write one row to file
        write_row(f, b, outcome);

    }}}}}}}}}
} // close all 9 loops


// -------------------- Program entry point --------------------

int main(void) {
    // Create and open file to store the dataset
    FILE *f = fopen("test.data", "w");
    if (!f) return 1;  // stop if file cannot be opened

    // Generate and write all data
    enumerate_all(f);

    // Close the file
    fclose(f);

    // Exit program successfully
    return 0;
}
