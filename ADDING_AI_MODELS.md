# Guide: Adding New AI Models to the Game

This guide explains the modular architecture that allows you to easily experiment with and swap different AI models.

## Current Architecture

The game uses a **modular AI system** where different AI models can be plugged in at different difficulty levels:

```
Level 1 (Easy)   → Naive Bayes AI
Level 2 (Medium) → Minimax (depth 3)
Level 3 (Hard)   → Full Minimax (unbeatable)
```

## How to Add a New AI Model

### Step 1: Train Your Model

Create a training program in `src/`:
```c
// src/your_model.c
// Train your model using dataset/processed/train.data
// Export to models/your_model/model.txt
```

### Step 2: Create AI Interface Files

Create header and implementation files following the pattern:

**`your_model_ai.h`**:
```c
#ifndef YOUR_MODEL_AI_H
#define YOUR_MODEL_AI_H

// Define your model structure
typedef struct {
    // Your model parameters
} YourModel;

// Load model from file
int your_model_load(const char *filename, YourModel *model);

// Find best move for 'O'
int your_model_find_best_move(const YourModel *model, char board[9]);

#endif
```

**`your_model_ai.c`**:
```c
#include "your_model_ai.h"
#include <stdio.h>
#include <string.h>

int your_model_load(const char *filename, YourModel *model) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return 0;
    
    // Parse your model file
    // Load parameters into model struct
    
    fclose(fp);
    return 1;
}

int your_model_find_best_move(const YourModel *model, char board[9]) {
    // Find all empty cells
    // Evaluate each possible move
    // Return best move index (0-8)
    
    return best_move;
}
```

### Step 3: Integrate into Game Logic

**Add to `game.h`**:
```c
// At top of file, add include comment
// Add function declarations
void game_load_your_model(const char *model_path);
```

**Add to `game.c`**:
```c
// At top, add include
#include "your_model_ai.h"

// Add global model variable
static YourModel your_model;
static int your_model_loaded = 0;

// Add loader function
void game_load_your_model(const char *model_path) {
    if (!your_model_loaded) {
        if (your_model_load(model_path, &your_model)) {
            your_model_loaded = 1;
        }
    }
}

// In game_ai_move(), add level check:
if (level == X && your_model_loaded) {
    mv = your_model_find_best_move(&your_model, g->b);
}
```

**Add to `gui_ai.c`**:
```c
// In main(), load model:
game_load_your_model("../models/your_model/model.txt");

// Update difficulty display:
if (level == X) {
    DrawText("Your Model Name", 380, 70, 20, BLACK);
}
```

**Update `compile.bat`**:
```batch
gcc -o ttt_gui.exe ^
    gui_ai.c ^
    game.c ^
    minimax.c ^
    naive_bayes_ai.c ^
    your_model_ai.c ^  REM Add your file here
    stats.c ^
    ...
```

### Step 4: Test

```bash
.\compile.bat
.\ttt_gui.exe
```

## Example: Naive Bayes AI

See the existing Naive Bayes implementation as a reference:

```
Files:
  src/naive_bayes.c              - Training program
  models/naive bayes/model.txt   - Trained model
  TTTGUI(wtf)/naive_bayes_ai.h   - Interface
  TTTGUI(wtf)/naive_bayes_ai.c   - Implementation

Integration:
  game.h   → game_load_nb_model() declaration
  game.c   → Loading and usage in game_ai_move()
  gui_ai.c → Load call and difficulty display
```

## Preserved Resources

The Linear Regression AI is preserved as a complete example:

```
✅ src/linear_regression.c              - Training program
✅ models/linear regression/model.txt   - Trained model (96% accuracy)
✅ models/linear regression/README.md   - Full documentation
✅ LINEAR_REGRESSION_SUMMARY.md         - Quick start guide
```

To re-enable Linear Regression AI:
1. Create `linear_regression_ai.h` and `linear_regression_ai.c` (refer to docs)
2. Follow integration steps above
3. Assign to desired difficulty level

## Model Requirements

Your AI model should:

1. **Input**: Board state as `char board[9]`
   - `'X'` = X player
   - `'O'` = O player  
   - `' '` = Empty cell

2. **Output**: Move index (0-8)
   - 0-2: Top row (left to right)
   - 3-5: Middle row
   - 6-8: Bottom row
   - Return -1 if no valid move

3. **Performance**: Fast enough for real-time play
   - Target: < 100ms per move
   - Naive Bayes: ~0.01ms
   - Minimax: ~0.1-10ms depending on depth

## Tips for Experimentation

1. **Keep backups**: Git commit before major changes
2. **Test incrementally**: Compile after each small change
3. **Use different levels**: Assign to unused difficulty level first
4. **Compare performance**: Track win/loss rates against each difficulty
5. **Document results**: Note accuracy, speed, gameplay feel

## Troubleshooting

**Model not loading:**
- Check file path in `game_load_your_model()`
- Verify model file exists and is readable
- Check return value of load function

**AI making invalid moves:**
- Verify move index is 0-8
- Check that cell is empty before returning
- Add fallback logic in `game_ai_move()`

**Compilation errors:**
- Add all new `.c` files to compile.bat
- Check include paths
- Verify function signatures match declarations

## Directory Structure

```
prog-meth-project/
├── src/                       # Training programs
│   ├── naive_bayes.c
│   ├── linear_regression.c
│   └── your_model.c          # Add here
│
├── models/                    # Trained models
│   ├── naive bayes/
│   │   └── model.txt
│   ├── linear regression/
│   │   ├── model.txt
│   │   └── README.md
│   └── your_model/           # Add here
│       └── model.txt
│
├── dataset/                   # Training data
│   └── processed/
│       ├── train.data
│       └── test.data
│
└── TTTGUI(wtf)/              # Game integration
    ├── game.h
    ├── game.c
    ├── gui_ai.c
    ├── naive_bayes_ai.h
    ├── naive_bayes_ai.c
    └── your_model_ai.c       # Add here
```

## Next Steps

1. ✅ Train your model using the dataset
2. ✅ Create interface files following the pattern
3. ✅ Integrate into game logic
4. ✅ Test and iterate
5. ✅ Compare with existing AIs
6. ✅ Document your findings

Happy experimenting! 🎮🤖
