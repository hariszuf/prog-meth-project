## Build (GUI with raylib)

### Windows (MinGW-w64 example)
```bash
gcc gui_ai.c minimax.c -o ttt_gui ^
  -I<raylib>\include ^
  -L<raylib>\lib\mingw64 ^
  -lraylib -lopengl32 -lgdi32 -lwinmm
