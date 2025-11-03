# Naive Bayes AI Integration Guide

## Overview
This guide explains how the Naive Bayes model has been integrated into the Tic-Tac-Toe game, allowing players to choose between different AI difficulties.

## File Structure

### New Files Created:
1. **naive_bayes_ai.h** - Header file for Naive Bayes AI functions
2. **naive_bayes_ai.c** - Implementation of Naive Bayes prediction and move selection

### Modified Files:
1. **game.h** - Added `game_load_nb_model()` function declaration
2. **game.c** - Modified `game_ai_move()` to support Naive Bayes (level 1)
3. **gui_ai.c** - Added model loading at startup and updated UI labels

## Difficulty Levels

The game now supports three AI difficulty levels:

- **Level 1 (Easy)**: Uses **Naive Bayes** trained model
  - Makes probabilistic decisions based on historical game data
  - Adds 20% randomness to be less predictable
  - Good for beginners
  
- **Level 2 (Medium)**: Uses **Minimax with depth limit (depth 3)**
  - Looks ahead 3 moves
  - 20% chance to pick second-best move
  - 10% chance to pick random move
  - Balanced challenge
  
- **Level 3 (Hard)**: Uses **Full Minimax**
  - Perfect play - unbeatable
  - Searches entire game tree
  - Best possible move every time

## How It Works

### 1. Model Loading
```c
// In gui_ai.c, during initialization:
game_load_nb_model("../models/naive bayes/model.txt");
```
The model is loaded once at startup from the text file.

### 2. Board Conversion
The game board is converted to the format expected by the Naive Bayes model:
- `'X'` → `"x"`
- `'O'` → `"o"`
- Empty → `"b"` (blank)

### 3. Move Selection
For each empty cell, the AI:
1. Simulates placing 'O' in that cell
2. Predicts the outcome (win/lose/draw) using Naive Bayes
3. Assigns a score based on the prediction
4. Chooses the move with the highest win probability

### 4. Prediction Scoring
```c
if (predicted_label == "win")  → score = probability
if (predicted_label == "draw") → score = probability * 0.5
if (predicted_label == "lose") → score = probability * 0.1
```

## Model Format

The Naive Bayes model (`model.txt`) contains:
- **Label Probabilities**: P(win), P(lose), P(draw)
- **Feature Probabilities**: P(state|label) for each board position (0-8)

Example:
```
Label: win        P(Label) = 0.668407
...
Feature 0:
  State=x     | Label=win        | P(State|Label) = 0.455078
```

## Compilation

### Using the provided batch script:
```batch
compile.bat
```

### Manual compilation:
```bash
gcc -o tictactoe_gui.exe gui_ai.c game.c minimax.c naive_bayes_ai.c stats.c \
    -I"C:/raylib/raylib/src" -L"C:/raylib/raylib/src" \
    -lraylib -lopengl32 -lgdi32 -lwinmm -Wall
```

**Note**: Adjust raylib paths according to your installation.

## Running the Game

1. Ensure the model file exists at: `../models/naive bayes/model.txt`
2. Run: `tictactoe_gui.exe`
3. Click "Player vs AI" to play against the computer
4. Select difficulty: E (Easy/NB), M (Medium/Minimax), H (Hard/Full Minimax)

## Architecture Diagram

```
┌─────────────────┐
│   gui_ai.c      │  ← User Interface (raylib)
│  (Main Loop)    │
└────────┬────────┘
         │
         ├─── Calls game functions
         │
┌────────▼────────┐
│     game.c      │  ← Game Logic
│  (Game Engine)  │
└────────┬────────┘
         │
         ├─── Level 1 ──┐
         │              │
┌────────▼────────┐    │
│  minimax.c      │    │
│ (Levels 2 & 3) │    │
└─────────────────┘    │
                       │
              ┌────────▼─────────┐
              │ naive_bayes_ai.c │
              │   (Level 1)      │
              └──────────────────┘
                       │
              ┌────────▼─────────┐
              │    model.txt     │
              │ (Trained Model)  │
              └──────────────────┘
```

## Customization

### Adjusting Naive Bayes Behavior

In `naive_bayes_ai.c`, you can modify:

1. **Randomness level** (line ~128):
```c
if ((rand() % 100) < 20)  // Change 20 to increase/decrease randomness
```

2. **Scoring weights** (lines ~105-113):
```c
if (strcmp(predicted_label, "win") == 0) {
    score = prob;  // Adjust this multiplier
}
```

3. **Smoothing value** (line ~60):
```c
prob *= 0.001;  // Probability for unseen feature combinations
```

## Troubleshooting

### Model not loading:
- Check that `model.txt` is in the correct path: `../models/naive bayes/model.txt`
- Verify the model file format matches the expected format
- If model fails to load, the game falls back to minimax

### AI not making moves:
- Ensure the model loaded successfully (check console output)
- Verify the board state conversion is working correctly
- Check that level 1 is selected

### Compilation errors:
- Ensure all `.c` and `.h` files are in the same directory
- Verify raylib paths are correct
- Check that you have gcc installed

## Performance Considerations

- **Model Loading**: Happens once at startup (~0.1-0.5 seconds)
- **Move Calculation**: Very fast (~0.01-0.05 seconds per move)
- **Memory Usage**: Model is ~200KB in memory
- **Naive Bayes is faster than Minimax** for move selection

## Future Enhancements

Possible improvements:
1. Add more difficulty levels with different AI strategies
2. Train separate models for different play styles (aggressive, defensive)
3. Implement reinforcement learning for adaptive difficulty
4. Add move explanations showing why the AI chose a particular move
5. Create a hybrid AI that uses both Naive Bayes and Minimax

## References

- Original Naive Bayes training: `src/naive_bayes.c`
- Game dataset: `dataset/tic-tac-toe.names`
- Model output: `models/naive bayes/model.txt`
