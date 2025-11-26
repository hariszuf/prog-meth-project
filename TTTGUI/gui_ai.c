#include "raylib.h"
#include <string.h>
#include <time.h>
#include <stdio.h>
#include "game.h"
#include "stats.h"

//Define pop up window sizr and size of tic tac toe grid
#define W_WIDTH 620
#define W_HEIGHT 720
#define CELL_SIZE 150
#define GRID_OFF_X 80
#define GRID_OFF_Y 140

// Check where is the cursor and what happens when cursor is clicked
static bool DrawButton(Rectangle r, const char* label, Color bg, Color fg)
{
    Vector2 m = GetMousePosition();
    bool hot = CheckCollisionPointRec(m, r);
    Color paint = hot ? ColorBrightness(bg, 0.15f) : bg;

    DrawRectangleRec(r, paint);
    DrawRectangleLinesEx(r, 2, BLACK);
    DrawText(label, (int)(r.x + 10), (int)(r.y + 8), 20, fg);

    return (hot && IsMouseButtonPressed(MOUSE_LEFT_BUTTON));
}

// Draw Grid for tic tac toe
static void DrawBoardGrid()
{
    int thickness = 4;
    for (int i = 1; i < 3; i++)
    {
        DrawRectangle(GRID_OFF_X, GRID_OFF_Y + i * CELL_SIZE - thickness / 2, 3 * CELL_SIZE, thickness, BLACK);
        DrawRectangle(GRID_OFF_X + i * CELL_SIZE - thickness / 2, GRID_OFF_Y, thickness, 3 * CELL_SIZE, BLACK);
    }
    DrawRectangleLinesEx((Rectangle){GRID_OFF_X, GRID_OFF_Y, 3 * CELL_SIZE, 3 * CELL_SIZE}, thickness, BLACK);
}

static void DrawGamePieces(const char* board)
{
    for (int i = 0; i < 9; i++)
    {
        if (board[i] == ' ') continue;

        int r = i / 3;
        int c = i % 3;
        int cx = GRID_OFF_X + c * CELL_SIZE + CELL_SIZE / 2;
        int cy = GRID_OFF_Y + r * CELL_SIZE + CELL_SIZE / 2;

        if (board[i] == 'X')
        {
            int thick = 5;
            for (int t = -thick / 2; t <= thick / 2; t++)
            {
                DrawLine(cx - 30, cy - 30 + t, cx + 30, cy + 30 + t, RED);
                DrawLine(cx - 30, cy + 30 + t, cx + 30, cy - 30 + t, RED);
                DrawLine(cx - 30 + t, cy - 30, cx + 30 + t, cy + 30, RED);
                DrawLine(cx - 30 + t, cy + 30, cx + 30 + t, cy - 30, RED);
            }
        }
        else if (board[i] == 'O')
        {
            for (int t = 0; t < 4; t++)
                DrawCircleLines(cx, cy, 40 - t, BLUE);
        }
    }
}

int main(void)
{
    printf("WORKING DIR = %s\n", GetWorkingDirectory());
    InitWindow(W_WIDTH, W_HEIGHT, "Tic Tac Toe (GUI)");
    SetTargetFPS(60);
    SetRandomSeed((unsigned)time(NULL));
    InitAudioDevice();

    Sound winSound = LoadSound("audio/win.mp3");
    Sound loseSound = LoadSound("audio/lose.mp3");

    game_load_all_models();
    
    Game g;
    game_init(&g);

    int mode = 0; 
    int level = 1;
    int recorded = 0;
    int ai_move_no = 0;

    while (!WindowShouldClose())
    {
        bool clickConsumed = false;
        int x = 20, y = 20, gap = 30, btnH = 40;

        int wPVP = MeasureText("Player vs Player", 20) + 20;
        int wAI = MeasureText("Player vs AI", 20) + 20;
        Rectangle bPVP = {(float)x, (float)y, (float)wPVP, (float)btnH};
        x += wPVP + gap;
        Rectangle bAI = {(float)x, (float)y, (float)wAI, (float)btnH};
        x += wAI + gap;

        Rectangle bE = {0}, bM = {0}, bH = {0};
        if (mode == 1)
        {
            int wE = MeasureText("E", 20) + 20;
            int wM = MeasureText("M", 20) + 20;
            int wH = MeasureText("H", 20) + 20;
            bE = (Rectangle){(float)x, (float)y, (float)wE, (float)btnH}; x += wE + gap;
            bM = (Rectangle){(float)x, (float)y, (float)wM, (float)btnH}; x += wM + gap;
            bH = (Rectangle){(float)x, (float)y, (float)wH, (float)btnH}; x += wH + gap;
        }

        // Buttons for UI
        if (DrawButton(bPVP, "Player vs Player", (mode == 0 ? BLUE : LIGHTGRAY), BLACK))
        {
            mode = 0;
            game_reset(&g);  // Reset game when switching to PvP mode
            recorded = 0;
            ai_move_no = 0;
            clickConsumed = true;
        }

        if (DrawButton(bAI, "Player vs AI", (mode == 1 ? RED : LIGHTGRAY), BLACK))
        {
            mode = 1;
            game_reset(&g);  // Reset game when switching to PvAI mode
            recorded = 0;
            ai_move_no = 0;
            clickConsumed = true;
        }


        if (mode == 1)
        {
            if (DrawButton(bE, "E", (level == 1 ? GREEN : LIGHTGRAY), BLACK)) { 
                level = 1; 
                game_reset(&g); 
                clickConsumed = true; 
            }

            if (DrawButton(bM, "M", (level == 2 ? ORANGE : LIGHTGRAY), BLACK)) { 
                level = 2; 
                game_reset(&g);  
                clickConsumed = true; 
            }

            if (DrawButton(bH, "H", (level == 3 ? RED : LIGHTGRAY), BLACK)) { 
                level = 3; 
                game_reset(&g);  
                clickConsumed = true; 
            }
        }

        if (IsKeyPressed(KEY_R))
        {
            game_reset(&g); recorded = 0; ai_move_no = 0;
        }

        // Input Handling
        if (!clickConsumed && g.winner == 0 && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            Vector2 mpos = GetMousePosition();
            Rectangle grid = {(float)GRID_OFF_X, (float)GRID_OFF_Y, 3.0f * CELL_SIZE, 3.0f * CELL_SIZE};

            if (CheckCollisionPointRec(mpos, grid))
            {
                int c = (int)((mpos.x - GRID_OFF_X) / (float)CELL_SIZE);
                int r = (int)((mpos.y - GRID_OFF_Y) / (float)CELL_SIZE);
                int idx = r * 3 + c;

                if (mode == 0 || (mode == 1 && g.turn == 'X'))
                {
                    if (game_make_move(&g, idx)) game_check_end(&g);
                }
            }
        }

        // AI logic flow
        if (mode == 1 && g.winner == 0 && g.turn == 'O')
        {
            double t0 = GetTime();
            game_ai_move(&g, level);
            double ms = (GetTime() - t0) * 1000.0;
            
            int depth_used = (level == 1 ? 1 : (level == 2 ? 3 : 0));
            stats_log_ai_move(mode, level, ai_move_no, ms, depth_used);
            game_check_end(&g);
        }

        // Stats & Sound
        if (g.winner != 0 && !recorded)
        {
            int code = (g.winner == 1) ? 1 : (g.winner == 2) ? 2 : 0;
            stats_record_result_mode((mode == 0 ? STATS_PVP : STATS_PVAI), (mode == 0 ? 0 : level), code);
            recorded = 1;

            if (g.winner == 1) PlaySound(winSound);
            else if (g.winner == 2 && mode == 0) PlaySound(winSound);
            else if (g.winner == 2 && mode == 1) PlaySound(loseSound);
        }

        // Set background to white
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Top UI
        DrawText("Mode:", 90, 70, 20, BLACK);
        DrawText(mode == 0 ? "Player vs Player" : "Player vs AI", 150, 70, 20, BLACK);

        if (mode == 1)
        {
            DrawText("Difficulty:", 400, 70, 20, BLACK);
            Color dColor = (level == 1) ? GREEN : (level == 2) ? ORANGE : RED;
            const char* dText = (level == 1) ? " Easy" : (level == 2) ? " Medium" : " Hard";
            DrawText(dText, 500, 70, 20, dColor);
            
            char ai_label[100];
            snprintf(ai_label, sizeof(ai_label), "AI: O (%s)", game_get_ai_name(level));
            DrawText(ai_label, 350, 100, 20, BLACK);
        }

        DrawText("Player 1: X", 90, 100, 20, RED);
        if (mode == 0) DrawText("Player 2: O", 400, 100, 20, BLUE);

        // Board
        DrawBoardGrid();
        DrawGamePieces(g.b);

        // Status Text
        char status[128];
        Color statusColor = BLACK;

        if (g.winner == 0)
        {
            if (g.turn == 'X') { strcpy(status, "Turn: Player 1 (X)"); statusColor = RED; }
            else {
                strcpy(status, mode == 0 ? "Turn: Player 2 (O)" : "AI (O) is playing...");
                statusColor = BLUE;
            }
        }
        else
        {
            if (g.winner == 1) { strcpy(status, "Winner: Player 1 (X)"); statusColor = RED; }
            else if (g.winner == 2) {
                strcpy(status, mode == 0 ? "Winner: Player 2 (O)" : "Winner: AI (O)");
                statusColor = BLUE;
            }
            else { strcpy(status, "Result: Draw"); statusColor = DARKGRAY; }
        }

        int textW = MeasureText(status, 24);
        DrawText(status, (W_WIDTH - textW) / 2, GRID_OFF_Y + 3 * CELL_SIZE + 20, 24, statusColor);
        DrawText("Click cells to play. Press R to reset. ESC to quit.", 28, GRID_OFF_Y + 3 * CELL_SIZE + 56, 22, DARKGRAY);

        // Scoreboard
        int games, xw, ow, dr;
        stats_get_counts_mode(mode == 0 ? STATS_PVP : STATS_PVAI, mode == 0 ? 0 : level, &games, &xw, &ow, &dr);
        
        char scoreLine[256];
        snprintf(scoreLine, sizeof(scoreLine), "%s | Games:%d  X Win:%d  O Win:%d  Draw:%d",
                 mode == 0 ? "PvP" : "PvAI", games, xw, ow, dr);
        
        int scoreW = MeasureText(scoreLine, 20);
        DrawText(scoreLine, (W_WIDTH - scoreW) / 2, GRID_OFF_Y + 3 * CELL_SIZE + 90, 20, DARKBLUE);

        // Reset PvP Stats Button
        if (mode == 0)
        {
            Rectangle bReset = { (float)(28 + scoreW + 100), (float)(GRID_OFF_Y + 3 * CELL_SIZE + 85), 
                                 (float)(MeasureText("Reset", 18) + 20), 28 };
            
            bool hover = CheckCollisionPointRec(GetMousePosition(), bReset);
            DrawRectangleRec(bReset, hover ? RED : LIGHTGRAY);
            DrawRectangleLinesEx(bReset, 1, BLACK);
            
            int rTextW = MeasureText("Reset", 18);
            DrawText("Reset", (int)(bReset.x + (bReset.width - rTextW) / 2), (int)(bReset.y + (bReset.height - 18) / 2), 18, hover ? WHITE : BLACK);

            if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                stats_reset_pvp(); recorded = 0;
            }
        }

        EndDrawing();
    }
    stats_reset_pvp();
    UnloadSound(winSound);
    UnloadSound(loseSound);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}