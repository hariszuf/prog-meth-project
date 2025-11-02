// gui_ai.c — GUI Tic-Tac-Toe (PvP/PvAI) using raylib
#include "raylib.h"     // raylib graphics and input functions
#include <string.h>     // C string functions like strlen (not heavily used here)
#include <time.h>       // time() for random seed
#include <stdio.h>      // snprintf() to format scoreboard text
#include "game.h"       // our game engine header (Game struct and functions)
#include "stats.h"      // stats functions for PvP/PvAI

/*------------------------------------------------------------
  Helper Button function
  Draws a rectangle button with a label and returns true if clicked
-------------------------------------------------------------*/
static bool Btn(Rectangle r, const char* label, Color bg, Color fg)
{
    Vector2 m;                            // store mouse (x,y)
    bool hot;                             // whether mouse is over the button
    bool pressed;                         // whether mouse button is pressed now
    Color paint;                          // background color to draw (normal/hover)

    // Get current mouse position from raylib
    m = GetMousePosition();

    // Check if mouse is inside the rectangle r
    hot = CheckCollisionPointRec(m, r);

    // If mouse is over the button, brighten color a bit
    if (hot)
    {
        paint = ColorBrightness(bg, 0.15f);
    }
    else
    {
        paint = bg;
    }

    // Draw filled rectangle for button background
    DrawRectangleRec(r, paint);

    // Draw a black border around the button (thickness 2 pixels)
    DrawRectangleLinesEx(r, 2, BLACK);

    // Draw the label text with some padding
    DrawText(label, (int)(r.x + 10), (int)(r.y + 8), 20, fg);

    // Check if left mouse button was pressed this frame
    pressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

    // If mouse is on button AND mouse was pressed, return true (clicked)
    if (hot)
    {
        if (pressed)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

/*------------------------------------------------------------
  Main program entry
-------------------------------------------------------------*/
int main(void)
{
    // Window and board layout constants
    const int W   = 720;     // window width
    const int H   = 760;     // window height
    const int CELL = 120;    // one grid cell size in pixels
    const int OFFX = 50;     // left offset for grid
    const int OFFY = 140;    // top offset for grid

    // Create a window with given size and title
    InitWindow(W, H, "Tic Tac Toe (GUI)");

    // Limit drawing / update to 60 frames per second
    SetTargetFPS(60);

    // Seed raylib random (used in AI levels 1 and 2)
    SetRandomSeed((unsigned)time(NULL));

    // Create a Game struct and initialize it (board 1..9, X starts)
    Game g;
    game_init(&g);

    // Mode: 0 = PvP (two humans), 1 = PvAI (human X vs AI O)
    int mode = 1;

    // AI difficulty level: 1=Easy, 2=Medium, 3=Hard
    int level = 2;

    // Guard to ensure we record stats only once after a game ends
    int recorded = 0;

    // Main loop: runs until the user closes the window (ESC or OS close)
    while (!WindowShouldClose())
    {
        bool clickConsumed;   // track if a click was used by UI buttons

        // Start each frame with "no button used the click yet"
        clickConsumed = false;

        // Top bar UI layout values
        int font  = 20;
        int padX  = 20;
        int btnH  = 40;
        int gap   = 10;
        int x     = 20;
        int y     = 20;

        // Measure text width for buttons so we size them neatly
        int wPVP = MeasureText("Player vs Player", font) + padX;
        int wAI  = MeasureText("Player vs AI",     font) + padX;
        int wE   = MeasureText("E", font) + padX;
        int wM   = MeasureText("M", font) + padX;
        int wH   = MeasureText("H", font) + padX;

        // Define rectangles for each button (x grows as we place each button)
        Rectangle bPVP = (Rectangle){ (float)x, (float)y, (float)wPVP, (float)btnH };
        x = x + wPVP + gap;

        Rectangle bAI  = (Rectangle){ (float)x, (float)y, (float)wAI,  (float)btnH };
        x = x + wAI + gap;

        Rectangle bE   = (Rectangle){ (float)x, (float)y, (float)wE,   (float)btnH };
        x = x + wE + gap;

        Rectangle bM   = (Rectangle){ (float)x, (float)y, (float)wM,   (float)btnH };
        x = x + wM + gap;

        Rectangle bH   = (Rectangle){ (float)x, (float)y, (float)wH,   (float)btnH };

        // ----- Handle UI Buttons -----

        // Player vs Player mode button
        if (Btn(bPVP, "Player vs Player", (mode == 0 ? GREEN : LIGHTGRAY), BLACK))
        {
            mode = 0;                 // set mode to PvP
            game_reset(&g);           // reset the current game
            recorded = 0;             // clear the stats guard
            clickConsumed = true;     // we used this click on a button
        }

        // Player vs AI mode button
        if (Btn(bAI, "Player vs AI", (mode == 1 ? GREEN : LIGHTGRAY), BLACK))
        {
            mode = 1;                 // set mode to PvAI
            game_reset(&g);           // reset the current game
            recorded = 0;             // clear the stats guard
            clickConsumed = true;     // we used this click on a button
        }

        // Difficulty buttons shown only when in PvAI mode
        if (mode == 1)
        {
            // Easy button
            if (Btn(bE, "E", (level == 1 ? ORANGE : LIGHTGRAY), BLACK))
            {
                level = 1;            // set level to Easy
                clickConsumed = true; // consume this click
            }

            // Medium button
            if (Btn(bM, "M", (level == 2 ? ORANGE : LIGHTGRAY), BLACK))
            {
                level = 2;            // set level to Medium
                clickConsumed = true; // consume this click
            }

            // Hard button
            if (Btn(bH, "H", (level == 3 ? ORANGE : LIGHTGRAY), BLACK))
            {
                level = 3;            // set level to Hard
                clickConsumed = true; // consume this click
            }
        }

        // Pressing R resets the game (handy during testing)
        if (IsKeyPressed(KEY_R))
        {
            game_reset(&g);           // reset the board and turn
            recorded = 0;             // allow stats to be written again after finish
        }

        // ----- Human board click handling -----
        // Only process if no UI button used the click,
        // game is not finished, and left mouse was pressed
        if (!clickConsumed)
        {
            if (g.winner == 0)
            {
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    Vector2 mpos;      // mouse position
                    int c;             // column index on grid (0..2)
                    int r;             // row index on grid (0..2)
                    int i;             // board index (0..8)

                    // Get mouse position
                    mpos = GetMousePosition();

                    // Convert mouse XY into grid coordinates
                    c = (int)((mpos.x - OFFX) / CELL);
                    r = (int)((mpos.y - OFFY) / CELL);

                    // Check if click is inside the 3x3 grid
                    if (c >= 0 && c < 3)
                    {
                        if (r >= 0 && r < 3)
                        {
                            // Convert (r,c) to single index i
                            i = r * 3 + c;

                            // In PvP mode, anyone can click when it's their turn
                            // In PvAI mode, only allow human 'X' to click
                            if (mode == 0)
                            {
                                if (game_make_move(&g, i))
                                {
                                    game_check_end(&g);   // update winner if any
                                }
                            }
                            else
                            {
                                if (mode == 1)
                                {
                                    if (g.turn == 'X')
                                    {
                                        if (game_make_move(&g, i))
                                        {
                                            game_check_end(&g);   // update winner if any
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        // ----- AI turn (only in PvAI mode) -----
        if (mode == 1)
        {
            if (g.winner == 0)
            {
                if (g.turn == 'O')
                {
                    game_ai_move(&g, level);  // let AI place 'O'
                    game_check_end(&g);       // check for win/draw
                }
            }
        }

        // ----- Record stats once after the game finishes -----
        if (g.winner != 0)
        {
            if (!recorded)
            {
                int code;  // 1 for X win, 2 for O win, 0 for draw

                // Translate g.winner to stats code
                if (g.winner == 1)
                {
                    code = 1; // X wins
                }
                else
                {
                    if (g.winner == 2)
                    {
                        code = 2; // O wins
                    }
                    else
                    {
                        code = 0; // draw
                    }
                }

                // Choose stats category based on mode and record it
                if (mode == 0)
                {
                    stats_record_result_mode(STATS_PVP, code);
                }
                else
                {
                    stats_record_result_mode(STATS_PVAI, code);
                }

                // Prevent recording twice for the same finished game
                recorded = 1;
            }
        }

        // ----- Drawing section -----
        BeginDrawing();                         // start drawing frame
        ClearBackground(RAYWHITE);              // clear screen to white

        // Show mode text
        DrawText("Mode:", 20, 70, 20, DARKGRAY);
        if (mode == 0)
        {
            DrawText("Player vs Player", 90, 70, 20, BLACK);
        }
        else
        {
            DrawText("Player vs AI", 90, 70, 20, BLACK);
        }

        // If PvAI mode, show difficulty text
        if (mode == 1)
        {
            DrawText("Difficulty:", 270, 70, 20, DARKGRAY);

            if (level == 1)
            {
                DrawText("Easy", 380, 70, 20, BLACK);
            }
            else
            {
                if (level == 2)
                {
                    DrawText("Medium", 380, 70, 20, BLACK);
                }
                else
                {
                    DrawText("Hard", 380, 70, 20, BLACK);
                }
            }
        }

        // Draw the 3x3 grid lines
        {
            int i; // loop counter for lines
            for (i = 1; i < 3; i++)
            {
                // horizontal lines
                DrawLine(OFFX, OFFY + i * CELL, OFFX + 3 * CELL, OFFY + i * CELL, BLACK);
                // vertical lines
                DrawLine(OFFX + i * CELL, OFFY, OFFX + i * CELL, OFFY + 3 * CELL, BLACK);
            }

            // outer border rectangle
            DrawRectangleLines(OFFX, OFFY, 3 * CELL, 3 * CELL, BLACK);
        }

        // Draw all marks on the board
        {
            int i; // index 0..8
            for (i = 0; i < 9; i++)
            {
                int r; // row
                int c; // column
                int cx;// center x of the cell
                int cy;// center y of the cell

                r  = i / 3;
                c  = i % 3;
                cx = OFFX + c * CELL + CELL / 2;
                cy = OFFY + r * CELL + CELL / 2;

                if (g.b[i] == 'X')
                {
                    // draw an 'X' with two red lines
                    DrawLine(cx - 30, cy - 30, cx + 30, cy + 30, RED);
                    DrawLine(cx - 30, cy + 30, cx + 30, cy - 30, RED);
                }
                else
                {
                    if (g.b[i] == 'O')
                    {
                        // draw an 'O' as a blue circle outline
                        DrawCircleLines(cx, cy, 38, BLUE);
                    }
                }
            }
        }

        // Build a status message (no ternary operators)
        {
            char status[128];   // text to show under the board

            // If game not finished
            if (g.winner == 0)
            {
                if (g.turn == 'X')
                {
                    // Always human X's turn label
                    strcpy(status, "Turn: Player 1 (X)");
                }
                else
                {
                    // g.turn == 'O'
                    if (mode == 0)
                    {
                        // PvP: it's player 2 turn
                        strcpy(status, "Turn: Player 2 (O)");
                    }
                    else
                    {
                        // PvAI: show AI is moving
                        strcpy(status, "AI (O) is playing...");
                    }
                }
            }
            else
            {
                // Game finished
                if (g.winner == 1)
                {
                    // X won
                    strcpy(status, "Winner: Player 1 (X)");
                }
                else
                {
                    if (g.winner == 2)
                    {
                        // O won
                        if (mode == 0)
                        {
                            strcpy(status, "Winner: Player 2 (O)");
                        }
                        else
                        {
                            strcpy(status, "Winner: AI (O)");
                        }
                    }
                    else
                    {
                        // draw
                        strcpy(status, "Result: Draw");
                    }
                }
            }

            // Draw fixed player labels above the status
            DrawText("Player 1: X", 20, 100, 20, DARKGRAY);

            if (mode == 0)
            {
                DrawText("Player 2: O", 170, 100, 20, DARKGRAY);
            }
            else
            {
                DrawText("AI: O", 170, 100, 20, DARKGRAY);
            }

            // Draw the computed status under the board
            DrawText(status, 20, OFFY + 3 * CELL + 20, 24, BLACK);

            // Helper hint text
            DrawText("Click cells to play. Press R to reset. ESC to quit.",
                     20, OFFY + 3 * CELL + 56, 18, GRAY);
        }

        // Draw category scoreboard (PvP or PvAI)
        {
            int games;  // total games
            int xw;     // X wins
            int ow;     // O wins
            int dr;     // draws
            char line[256];   // buffer for one scoreboard line

            // Read stats based on current mode
            if (mode == 0)
            {
                stats_get_counts_mode(STATS_PVP, &games, &xw, &ow, &dr);
            }
            else
            {
                stats_get_counts_mode(STATS_PVAI, &games, &xw, &ow, &dr);
            }

            // Format "PvP | Games:.. Win:.. Lose:.. Draw:.."
            // Note: "Win" shows X wins and "Lose" shows O wins from X's perspective
            snprintf(line, sizeof(line),
                     "%s | Games:%d  X Win:%d  O Win:%d  Draw:%d",
                     (mode == 0 ? "PvP" : "PvAI"), games, xw, ow, dr);

            // Draw the scoreboard text
            DrawText(line, 20, OFFY + 3 * CELL + 90, 20, DARKBLUE);
        }

        EndDrawing();                           // finish drawing this frame
    }

    // Close the window and free internal resources
    CloseWindow();

    // Return 0 from main (success)
    return 0;
}
