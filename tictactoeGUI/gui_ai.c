// gui_ai.c — GUI Tic Tac Toe (PvP/PvAI) using raylib
#include "raylib.h"
#include <string.h>
#include <time.h>
#include <stdio.h>
#include "game.h"
#include "stats.h"

static bool Btn(Rectangle r, const char* label, Color bg, Color fg) {
    Vector2 m = GetMousePosition();
    bool hot = CheckCollisionPointRec(m, r);
    DrawRectangleRec(r, hot ? ColorBrightness(bg, 0.15f) : bg);
    DrawRectangleLinesEx(r, 2, BLACK);
    DrawText(label, (int)(r.x + 10), (int)(r.y + 8), 20, fg);
    return (hot && IsMouseButtonPressed(MOUSE_LEFT_BUTTON));
}

int main(void) {
    const int W=720, H=760, CELL=120, OFFX=50, OFFY=140;

    InitWindow(W, H, "Tic Tac Toe (GUI)");
    SetTargetFPS(60);
    SetRandomSeed((unsigned)time(NULL));

    Game g;
    game_init(&g);
    int mode = 1;         // 0=PvP, 1=PvAI
    int level = 2;        // AI difficulty
    int recorded = 0;     // stats-writing guard

    while (!WindowShouldClose()) {
        bool clickConsumed = false;

        // Buttons
        int font = 20, padX = 20, btnH = 40, gap = 10, x = 20, y = 20;

        int wPVP = MeasureText("Player vs Player", font) + padX;
        int wAI  = MeasureText("Player vs AI", font) + padX;
        int wE   = MeasureText("E", font) + padX;
        int wM   = MeasureText("M", font) + padX;
        int wH   = MeasureText("H", font) + padX;

        Rectangle bPVP = (Rectangle){ x, y, (float)wPVP, (float)btnH }; x += wPVP + gap;
        Rectangle bAI  = (Rectangle){ x, y, (float)wAI,  (float)btnH }; x += wAI  + gap;
        Rectangle bE   = (Rectangle){ x, y, (float)wE,   (float)btnH }; x += wE   + gap;
        Rectangle bM   = (Rectangle){ x, y, (float)wM,   (float)btnH }; x += wM   + gap;
        Rectangle bH   = (Rectangle){ x, y, (float)wH,   (float)btnH };



        if (Btn(bPVP, "Player vs Player", mode == 0 ? GREEN : LIGHTGRAY, BLACK)) {
            mode = 0;
            game_reset(&g);
            recorded = 0;
            clickConsumed = true;
        }

        if (Btn(bAI, "Player vs AI", mode == 1 ? GREEN : LIGHTGRAY, BLACK)) {
            mode = 1;
            game_reset(&g);
            recorded = 0;
            clickConsumed = true;
        }

        if (mode == 1) {
            if (Btn(bE, "E", level == 1 ? ORANGE : LIGHTGRAY, BLACK)) {
                level = 1;
                clickConsumed = true;
            }
            if (Btn(bM, "M", level == 2 ? ORANGE : LIGHTGRAY, BLACK)) {
                level = 2;
                clickConsumed = true;
            }
            if (Btn(bH, "H", level == 3 ? ORANGE : LIGHTGRAY, BLACK)) {
                level = 3;
                clickConsumed = true;
            }
        }

        if (IsKeyPressed(KEY_R)) {
            game_reset(&g);
            recorded = 0;
        }

        // Human click (skip if a button consumed it)
        if (!clickConsumed && g.winner == 0 && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 m = GetMousePosition();
            int c = (int)((m.x - OFFX) / CELL), r = (int)((m.y - OFFY) / CELL);
            if (c >= 0 && c < 3 && r >= 0 && r < 3) {
                int i = r * 3 + c;
                if (mode == 0 || (mode == 1 && g.turn == 'X')) {
                    if (game_make_move(&g, i)) {
                        game_check_end(&g);
                    }
                }
            }
        }

        // AI turn
        if (mode == 1 && g.winner == 0 && g.turn == 'O') {
            game_ai_move(&g, level);
            game_check_end(&g);
        }

        // Record stats once after finish
        if (g.winner != 0 && !recorded) {
            int code = (g.winner == 1) ? 1 : (g.winner == 2) ? 2 : 0;
            stats_record_result_mode((mode == 0) ? STATS_PVP : STATS_PVAI, code);
            recorded = 1;
        }

        // ---- Draw ----
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("Mode:", 20, 70, 20, DARKGRAY);
        DrawText(mode == 0 ? "Player vs Player" : "Player vs AI", 90, 70, 20, BLACK);
        if (mode == 1) {
            DrawText("Difficulty:", 270, 70, 20, DARKGRAY);
            DrawText(level == 1 ? "Easy" : (level == 2 ? "Medium" : "Hard"), 380, 70, 20, BLACK);
        }

        // Grid
        for (int i = 1; i < 3; i++) {
            DrawLine(OFFX, OFFY + i * CELL, OFFX + 3 * CELL, OFFY + i * CELL, BLACK);
            DrawLine(OFFX + i * CELL, OFFY, OFFX + i * CELL, OFFY + 3 * CELL, BLACK);
        }
        DrawRectangleLines(OFFX, OFFY, 3 * CELL, 3 * CELL, BLACK);

        // Marks
        for (int i = 0; i < 9; i++) {
            int r = i / 3, c = i % 3, x = OFFX + c * CELL + CELL / 2, y = OFFY + r * CELL + CELL / 2;
            if (g.b[i] == 'X') {
                DrawLine(x - 30, y - 30, x + 30, y + 30, RED);
                DrawLine(x - 30, y + 30, x + 30, y - 30, RED);
            } else if (g.b[i] == 'O') {
                DrawCircleLines(x, y, 38, BLUE);
            }
        }

        // Status
        const char* status = (g.winner == 0)
            ? ((g.turn == 'X') ? "Turn: Player 1 (X)" : (mode == 0 ? "Turn: Player 2 (O)" : "AI (O) is playing…"))
            : (g.winner == 1 ? "Winner: Player 1 (X)" : g.winner == 2 ? (mode == 0 ? "Winner: Player 2 (O)" : "Winner: AI (O)") : "Result: Draw");

        DrawText("Player 1: X", 20, 100, 20, DARKGRAY);
        DrawText(mode == 0 ? "Player 2: O" : "AI: O", 170, 100, 20, DARKGRAY);
        DrawText(status, 20, OFFY + 3 * CELL + 20, 24, BLACK);
        DrawText("Click cells to play. Press R to reset. ESC to quit.", 20, OFFY + 3 * CELL + 56, 18, GRAY);

        // Category scoreboard
        int games, xw, ow, dr;
        stats_get_counts_mode((mode == 0) ? STATS_PVP : STATS_PVAI, &games, &xw, &ow, &dr);
        char line[256];  // make buffer larger
        snprintf(line, sizeof(line),
            "%s | Games:%d  Win:%d  Lose:%d  Draw:%d",
            (mode==0?"PvP":"PvAI"), games, xw, ow, dr);
        DrawText(line, 20, OFFY + 3*CELL + 90, 20, DARKBLUE);


        EndDrawing();
    }
    CloseWindow();
    return 0;
}
