// gui_ai.c — GUI Tic-Tac-Toe (PvP/PvAI) using raylib
#include "raylib.h"     // raylib graphics library
#include <string.h>     // for strcpy
#include <time.h>       // for time() to seed random
#include <stdio.h>      // for snprintf
#include "game.h"       // game file
#include "stats.h"      // stats file



//Button function: draws button and returns true if clicked
static bool Btn(Rectangle r, const char* label, Color bg, Color fg)
{
    Vector2 m = GetMousePosition();
    bool hot = CheckCollisionPointRec(m, r);

    Color paint = hot ? ColorBrightness(bg, 0.15f) : bg;

    DrawRectangleRec(r, paint);
    DrawRectangleLinesEx(r, 2, BLACK);
    DrawText(label, (int)(r.x + 10), (int)(r.y + 8), 20, fg);

    if (hot && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        return true;
    }
    return false; 
}

int main(void)
{
    const int W = 600;      // window width
    const int H = 720;      // window height
    const int CELL = 150;   // size of each grid cell
    const int OFFX = 80;    // grid x offset from left
    const int OFFY = 140;   // grid y offset from top

    InitWindow(W, H, "Tic Tac Toe (GUI)");  // create window
    SetTargetFPS(60);                       
    SetRandomSeed((unsigned)time(NULL));    // seed random for AI

    InitAudioDevice(); // Initialize audio

    Sound winSound = LoadSound("audio/win.mp3");
    Sound loseSound = LoadSound("audio/lose.mp3");

    // Load all AI models at startup
    printf("========================================\n");
    printf("Loading AI Models...\n");
    printf("========================================\n");
    game_load_all_models();
    printf("========================================\n\n");

    Game g;           // game state
    game_init(&g);    // initialize game

    int mode = 0;     // set default game mode: 0=PvP, 1=PvAI
    int level = 1;    // set default AI difficulty: 1=Easy, 2=Medium, 3=Hard
    int recorded = 0; // flag to prevent recording stats twice
    int ai_move_no = 0;
    while (!WindowShouldClose())  // main game loop
    {
        bool clickConsumed = false; // track if click was used by UI

        // UI button layout variables
        int font = 20;  // font size
        int padX = 20;  // button padding
        int btnH = 40;  // button height
        int gap = 30;   // gap between buttons
        int x = 10;     // current x position for placing buttons
        int y = 20;     // y position for buttons

        // Calculate button widths based on text
        int wPVP = MeasureText("Player vs Player", font) + padX;
        int wAI = MeasureText("Player vs AI", font) + padX;
        int wE = MeasureText("E", font) + padX;
        int wM = MeasureText("M", font) + padX;
        int wH = MeasureText("H", font) + padX;

        // Define PvP and AI buttons
        Rectangle bPVP = {(float)x, (float)y, (float)wPVP, (float)btnH}; // PvP button
        x += wPVP + gap;  // move x position for next button
        Rectangle bAI = {(float)x, (float)y, (float)wAI, (float)btnH};   // PvAI button
        x += wAI + gap;   // move x position after AI button

        // Keep position after mode buttons (used for Reset PvP Stats)
        //int xAfterModeButtons = x;

        // Difficulty buttons (only shown if in AI mode)
        Rectangle bE, bM, bH;
        if (mode == 1)
        {
            bE = (Rectangle){ (float)x, (float)y, (float)wE, (float)btnH };
            x += wE + gap;
            bM = (Rectangle){ (float)x, (float)y, (float)wM, (float)btnH };
            x += wM + gap;
            bH = (Rectangle){ (float)x, (float)y, (float)wH, (float)btnH };
            x += wH + gap;
        }

        // Define Reset PvP Stats button — always created, only visible in PvP
        // Define Reset PvP Stats button (smaller and compact)
        //int wReset = MeasureText("Reset", font) + 10;      // shorter label and narrower width
        //Rectangle bResetPvP = { (float)(xAfterModeButtons + 5), (float)(y + 50), (float)wReset, (float)(btnH - 10) };

        // Handle PvP mode button click
        if (Btn(bPVP, "Player vs Player", (mode == 0 ? BLUE : LIGHTGRAY), BLACK))
        {
            mode = 0;
            game_reset(&g);
            recorded = 0;
            ai_move_no = 0; // reset AI move counter
            clickConsumed = true;
        }

        // Handle PvAI mode button click
        if (Btn(bAI, "Player vs AI", (mode == 1 ? RED : LIGHTGRAY), BLACK))
        {
            mode = 1;
            game_reset(&g);
            recorded = 0;
            ai_move_no = 0; // reset AI move counter
            clickConsumed = true;
        }

        // Show difficulty buttons only in PvAI mode
        if (mode == 1)
        {
            // Easy difficulty button
            if (Btn(bE, "E", (level == 1 ? GREEN : LIGHTGRAY), BLACK))
            {
                level = 1;          // set difficulty to Easy
                clickConsumed = true; // mark click as used
            }
            // Medium difficulty button
            if (Btn(bM, "M", (level == 2 ? ORANGE : LIGHTGRAY), BLACK))
            {
                level = 2;          // set difficulty to Medium
                clickConsumed = true; // mark click as used
            }
            // Hard difficulty button
            if (Btn(bH, "H", (level == 3 ? RED : LIGHTGRAY), BLACK))
            {
                level = 3;          // set difficulty to Hard
                clickConsumed = true; // mark click as used
            }
        }

      

        // Handle R key press to reset game
        if (IsKeyPressed(KEY_R))
        {
            game_reset(&g);
            recorded = 0;
            ai_move_no = 0; // reset AI move counter
        }

        // Handle board clicks (only if UI didn't consume click, game not over, and mouse clicked)
        if (!clickConsumed && g.winner == 0 && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            Vector2 mpos = GetMousePosition();

            // Grid rectangle to validate clicks first
            Rectangle grid = { (float)OFFX, (float)OFFY, 3.0f * CELL, 3.0f * CELL };

            if (CheckCollisionPointRec(mpos, grid))
            {
                // Safe now: (mpos - OFFX/OFFY) are non-negative
                int c = (int)((mpos.x - OFFX) / (float)CELL);
                int r = (int)((mpos.y - OFFY) / (float)CELL);
                int i = r * 3 + c;

                // In PvP, both can click. In PvAI, only X (human) can click
                if (mode == 0 || (mode == 1 && g.turn == 'X'))
                {
                    if (game_make_move(&g, i))
                    {
                        game_check_end(&g);
                    }
                }
            }
            // else: click was outside the grid → ignore
        }

        // AI turn: if PvAI mode, game not over, and it's O's turn
        if (mode == 1 && g.winner == 0 && g.turn == 'O')
        {
            double t0 = GetTime();                 // start timer (seconds)
            game_ai_move(&g, level);               // AI makes a move
            double t1 = GetTime();                 // end timer
            double ms = (t1 - t0) * 1000.0;        // duration in milliseconds

            ai_move_no++;                          // increment AI move counter
            stats_log_ai_move(mode, level, ai_move_no, ms);  // append to file

            game_check_end(&g);                    // check if game ended
        }

        // Record statistics once per game when game ends
        if (g.winner != 0 && !recorded)
        {
            int code;  // code to pass to stats
            
            if (g.winner == 1)  // if X wins
            {
                code = 1;  // X wins code
            }
            else if (g.winner == 2)  // if O wins
            {
                code = 2;  // O wins code
            }
            else  // if draw
            {
                code = 0;  // draw code
            }

        if (mode == STATS_PVP)
        {
            // No AI level for PvP, use 0
            stats_record_result_mode(STATS_PVP, 0, code);
        }
        else if (mode == STATS_PVAI)
        {
            // Use actual AI difficulty level (1=Easy, 2=Medium, 3=Hard)
            stats_record_result_mode(STATS_PVAI, level, code);
        }
            
            recorded = 1;  // mark as recorded
            if (mode == 0)  // PvP mode
            {
                if (g.winner == 1 || g.winner == 2)
                    PlaySound(winSound);
            }
            else if (mode == 1)  // PvAI mode
            {
                if (g.winner == 1)
                    PlaySound(winSound);   // Player wins
                else if (g.winner == 2)
                    PlaySound(loseSound);  // AI wins
            }


        }

        // Start drawing this frame
        BeginDrawing();
        ClearBackground(RAYWHITE);  // set background to white

        // Display current mode
        DrawText("Mode:", 90, 70, 20, BLACK);  // label
        if (mode == 0)  // if PvP mode
        {
            DrawText("Player vs Player", 150, 70, 20, BLACK);  // PvP text
        }
        else  // if PvAI mode
        {
            DrawText("Player vs AI", 150, 70, 20, BLACK);  // PvAI text
        }

        // Display difficulty (only in PvAI mode)
        if (mode == 1)
        {
            DrawText("Difficulty:", 400, 70, 20, BLACK);  // label
            
            if (level == 1)  // Easy Mode
            {
                DrawText("Easy", 500, 70, 20, GREEN);  // draw Easy
            }
            else if (level == 2)  // Medium mode
            {
                DrawText("Medium", 500, 70, 20, ORANGE);  // draw Medium
            }
            else  // Hard mode
            {
                DrawText("Hard", 500, 70, 20, RED);  // draw Hard
            }
        }

        // Draw the 3x3 grid
    // Draw the 3x3 grid (thicker lines)
    int gridThickness = 4;  // control grid thickness

    for (int i = 1; i < 3; i++)
    {
        // Draw horizontal lines
        DrawRectangle(OFFX, OFFY + i * CELL - gridThickness / 2, 3 * CELL, gridThickness, BLACK);

        // Draw vertical lines
        DrawRectangle(OFFX + i * CELL - gridThickness / 2, OFFY, gridThickness, 3 * CELL, BLACK);
    }

    // Outer border
    DrawRectangleLinesEx((Rectangle){OFFX, OFFY, 3 * CELL, 3 * CELL}, gridThickness, BLACK);


        // Draw all X's and O's on the board
        for (int i = 0; i < 9; i++)  // loop through all 9 cells
        {
            int r = i / 3;                       // calculate row
            int c = i % 3;                       // calculate column
            int cx = OFFX + c * CELL + CELL / 2; // center x of cell
            int cy = OFFY + r * CELL + CELL / 2; // center y of cell

        if (g.b[i] == 'X')
        {
            int thick = 5;  // thickness of X lines
            for (int t = -thick / 2; t <= thick / 2; t++)
            {
                // draw both diagonals shifted in both directions
                DrawLine(cx - 30, cy - 30 + t, cx + 30, cy + 30 + t, RED);
                DrawLine(cx - 30, cy + 30 + t, cx + 30, cy - 30 + t, RED);
                DrawLine(cx - 30 + t, cy - 30, cx + 30 + t, cy + 30, RED);
                DrawLine(cx - 30 + t, cy + 30, cx + 30 + t, cy - 30, RED);
            }
        }

        else if (g.b[i] == 'O')  // if cell has O
        {
            int thick = 4; // thickness of O circle
            for (int t = 0; t < thick; t++)
            {
                DrawCircleLines(cx, cy, 40 - t, BLUE); // draw multiple rings to thicken
            }
        }

        }

        // Build status message text
        char status[128];  // buffer for status text
        if (g.winner == 0)  // game still in progress
        {
            if (g.turn == 'X')  // X's turn
            {
                strcpy(status, "Turn: Player 1 (X)");  // human X turn message
            }
            else  // O's turn
            {
                if (mode == 0)  // if PvP mode
                {
                    strcpy(status, "Turn: Player 2 (O)");  // Player 2 turn message
                }
                else  // if PvAI mode
                {
                    strcpy(status, "AI (O) is playing...");  // AI turn message
                }
            }
        }
        else  // game finished
        {
            if (g.winner == 1)  // X won
            {
                strcpy(status, "Winner: Player 1 (X)");  // X wins message
            }
            else if (g.winner == 2)  // O won
            {
                if (mode == 0)  // if PvP mode
                {
                    strcpy(status, "Winner: Player 2 (O)");  // Player 2 wins message
                }
                else  // if PvAI mode
                {
                    strcpy(status, "Winner: AI (O)");  // AI wins message
                }
            }
            else  // draw
            {
                strcpy(status, "Result: Draw");  // draw message
            }
        }

        // Draw player labels
        DrawText("Player 1: X", 90, 100, 20, RED);  // X label
        if (mode == 0)  // if PvP mode
        {
            DrawText("Player 2: O", 400, 100, 20, BLUE);  // Player 2 label
        }
        else  // if PvAI mode
        {
            // Show AI model name being used
            char ai_label[100];
            snprintf(ai_label, sizeof(ai_label), "AI: O (%s)",game_get_ai_name(level));
            DrawText(ai_label, 350, 100, 20, BLACK);  // AI label with model name
        }

        // Draw status message
        int fontSize = 24;

        // Choose text color based on the message
        Color statusColor = BLACK; // default

        if (g.winner == 0)  // game still running
        {
            if (g.turn == 'X')
                statusColor = RED;      // Player 1’s turn → red
            else if (g.turn == 'O')
                statusColor = BLUE;     // Player 2 or AI’s turn → blue
        }
        else  // game finished
        {
            if (g.winner == 1)
                statusColor = RED;      // X won → red
            else if (g.winner == 2)
                statusColor = BLUE;     // O or AI won → blue
            else
                statusColor = DARKGRAY; // draw
        }

// Center the text horizontally
int textWidth = MeasureText(status, fontSize);
int centerX = (W - textWidth) / 2;

// Draw the colored text
DrawText(status, centerX, OFFY + 3 * CELL + 20, fontSize, statusColor);

        DrawText("Click cells to play. Press R to reset. ESC to quit.",
                28, OFFY + 3 * CELL + 56, 22, DARKGRAY);  // help text

        // Draw scoreboard
        int games, xw, ow, dr;  // variables for stats
        
        // Display PvP stats
        if (mode == 0)  // PvP mode
        {
            stats_get_counts_mode(STATS_PVP, 0, &games, &xw, &ow, &dr);  // get PvP stats
        }
        else if (mode == 1)  // PvAI mode
        {
            stats_get_counts_mode(STATS_PVAI, level, &games, &xw, &ow, &dr);  // get PvAI stats
        }

        char line[256];  // buffer for scoreboard text
        if (mode == 0)  // if PvP mode
        {
            snprintf(line, sizeof(line), "PvP | Games:%d  X Win:%d  O Win:%d  Draw:%d",
                    games, xw, ow, dr);  // format PvP scoreboard
        }
        else  // if PvAI mode
        {
            snprintf(line, sizeof(line), "PvAI | Games:%d  X Win:%d  O Win:%d  Draw:%d",
                    games, xw, ow, dr);  // format PvAI scoreboard
        }
        
        int scoreFont = 20;
        int scoreY = OFFY + 3 * CELL + 90;  // same height as before

        // Measure text width to center horizontally
        int scoreWidth = MeasureText(line, scoreFont);
        int scoreX = (W - scoreWidth) / 2;  // center horizontally in window

        // Draw centered scoreboard
        DrawText(line, scoreX, scoreY, scoreFont, DARKBLUE);

        // Draw and handle Reset PvP Stats button beside the scoreboard
        // Draw and handle Reset PvP Stats button beside the scoreboard
        if (mode == 0)  // only show in PvP mode
        {
            int resetFont = 18;  // font size for text
            int resetW = MeasureText("Reset", resetFont) + 20;  // width of button
            int resetH = 28;     // height of button

            // Position the button beside the scoreboard (next to "Draw:0")
            int resetX = 28 + MeasureText(line, 20) + 100;  // 70px gap after scoreboard text
            int resetY = OFFY + 3 * CELL + 85;             // vertically align with scoreboard

            Rectangle bResetPvP = { (float)resetX, (float)resetY, (float)resetW, (float)resetH };

            Vector2 mpos = GetMousePosition();                     // get mouse position
            bool hover = CheckCollisionPointRec(mpos, bResetPvP);  // check hover

            // Draw button background and border
            DrawRectangleRec(bResetPvP, hover ? RED : LIGHTGRAY);
            DrawRectangleLinesEx(bResetPvP, 1, BLACK);

            // Center text horizontally and vertically inside the button
            int textW = MeasureText("Reset", resetFont);
            int textX = (int)(bResetPvP.x + (bResetPvP.width - textW) / 2);
            int textY = (int)(bResetPvP.y + (bResetPvP.height - resetFont) / 2);

            DrawText("Reset", textX, textY, resetFont, hover ? WHITE : BLACK);

            // Handle click event
            if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                stats_reset_pvp();  // clear PvP stats
                recorded = 0;       // reset local flag
            }
        }



        EndDrawing();  // finish drawing this frame
    }
    UnloadSound(winSound);
    UnloadSound(loseSound);
    CloseAudioDevice();

    CloseWindow();
    return 0;
}
