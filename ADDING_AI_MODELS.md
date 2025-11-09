# Guide: Adding New AI Models to the Game

This guide explains the modular architecture that allows you to easily experiment with and swap different AI models.

## Current Architecture

The game uses a **fully modular AI system** where you can easily swap between all 6 trained models and Minimax variants:

### Default Configuration (Best Models)
```
Level 1 (Easy)   → Minimax (Easy) - Imperfect play
Level 2 (Medium) → Q-Learning     - Best ML model (95%+ vs Minimax)
Level 3 (Hard)   → Minimax (Hard) - Perfect unbeatable play
```

### Modular System Features
✅ Load all 6 models at startup  
✅ Switch models with preset configurations  
✅ Swap between dataset variants (non-terminal vs combined)  
✅ Display current AI model in-game  
✅ Easy to add new models  

See `TTTGUI(wtf)/MODULAR_AI_GUIDE.md` for detailed usage instructions.

## Available Trained Models

You currently have **6 trained models** ready to use:

### Naive Bayes Models
- **`models/naive_bayes_non_terminal/model_non_terminal.txt`**
  - Trained on strategic positions only (non-terminal states)
  - Focuses on mid-game decision making
  
- **`models/naive_bayes_combined/model_combined.txt`**
  - Trained on all game states (terminal + non-terminal)
  - Comprehensive coverage of all positions

### Linear Regression Models
- **`models/linear_regression_non_terminal/model_non_terminal.txt`**
  - Numeric evaluation of strategic positions
  - Fast predictions with linear weights
  
- **`models/linear_regression_combined/model_combined.txt`**
  - Trained on all game states
  - Balanced approach for all positions

### Q-Learning Models
- **`models/q learning/q_learning_non_terminal.txt`**
  - Reinforcement learning on strategic positions
  - Learned optimal policy through trial and error
  
- **`models/q learning/q_learning_combined.txt`**
  - Complete state-action value mapping
  - Extensive Q-table with ~18,000+ entries

## Model Comparison

To determine which model performs best, use the evaluation tool:

```bash
cd src
.\evaluate_models.bat
```

This will test all 6 models against perfect Minimax play (100 games each) and generate a comprehensive comparison report showing:
- Win/Draw/Loss rates
- Illegal move counts
- Average response time
- Overall performance ranking

See `src/MODEL_EVALUATION_GUIDE.md` for detailed information on interpreting results.

## How to Add a New AI Model

The modular system makes it easy to add new models. Follow this streamlined process:

### Step 1: Train Your Model

Create a training program in `src/model training/`:
```c
// src/model training/your_model.c
// Train your model using dataset files
// Export to models/your_model/model.txt
```

See existing trainers: `naive_bayes.c`, `linear_regression.c`, `q_learning.c`

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

**Add to `model_config.h`**:
```c
typedef enum {
    AI_MODEL_NAIVE_BAYES,
    AI_MODEL_LINEAR_REGRESSION,
    AI_MODEL_Q_LEARNING,
    AI_MODEL_MINIMAX_EASY,
    AI_MODEL_MINIMAX_HARD,
    AI_MODEL_YOUR_MODEL        // Add your model here
} AIModelType;
```

**Add to `model_config.c`** (for display names):
```c
const char* ai_config_get_model_name(AIModelType model) {
    switch (model) {
        // ... existing cases
        case AI_MODEL_YOUR_MODEL:
            return "Your Model Name";
        default:
            return "Unknown";
    }
}
```

**Add to `game.c`**:
```c
// At top, add include
#include "your_model_ai.h"

// Add global model variable
static YourModel your_model;
static int your_model_loaded = 0;

// In game_load_all_models(), add loading:
if (your_model_load("../models/your_model/model.txt", &your_model)) {
    your_model_loaded = 1;
    printf("✓ Your Model loaded\n");
}

// In game_ai_move(), add case:
case AI_MODEL_YOUR_MODEL:
    if (your_model_loaded) {
        mv = your_model_find_best_move(&your_model, g->b);
    }
    break;

// In game_load_model_file(), add case for reloading:
case AI_MODEL_YOUR_MODEL:
    if (your_model_load(model_path, &your_model)) {
        your_model_loaded = 1;
        printf("✓ Reloaded Your Model from: %s\n", model_path);
    }
    break;
```

**Update `compile.bat`**:
```batch
gcc -o ttt_gui.exe ^
    gui_ai.c ^
    game.c ^
    minimax.c ^
    naive_bayes_ai.c ^
    linear_regression_ai.c ^
    q_learning_ai.c ^
    model_config.c ^
    your_model_ai.c ^  REM Add your file here
    stats.c ^
    ...
```

### Step 4: Configure and Test

**Set your model in a difficulty level** (in `gui_ai.c` after `game_load_all_models()`):
```c
AIConfig config;
ai_config_init(&config);
ai_config_set_level(&config, 2, AI_MODEL_YOUR_MODEL);  // Set as medium difficulty
game_set_ai_config(&config);
```

**Or create a preset** in `model_config.c`:
```c
void ai_config_preset_with_your_model(AIConfig *config) {
    config->easy_model = AI_MODEL_MINIMAX_EASY;
    config->medium_model = AI_MODEL_YOUR_MODEL;
    config->hard_model = AI_MODEL_MINIMAX_HARD;
}
```

Then use it:
```c
AIConfig config;
ai_config_preset_with_your_model(&config);
game_set_ai_config(&config);
```

## Example: Naive Bayes AI

All three ML models follow the same interface pattern. Check any for reference:

```
Naive Bayes Example:
  Training:       src/model training/naive_bayes.c
  Models:         models/naive_bayes_non_terminal/
                  models/naive_bayes_combined/
  Interface:      TTTGUI(wtf)/naive_bayes_ai.h
  Implementation: TTTGUI(wtf)/naive_bayes_ai.c

Linear Regression Example:
  Training:       src/model training/linear_regression.c
  Models:         models/linear_regression_non_terminal/
                  models/linear_regression_combined/
  Interface:      TTTGUI(wtf)/linear_regression_ai.h
  Implementation: TTTGUI(wtf)/linear_regression_ai.c

Q-Learning Example:
  Training:       src/model training/q_learning.c
  Models:         models/q learning/
  Interface:      TTTGUI(wtf)/q_learning_ai.h
  Implementation: TTTGUI(wtf)/q_learning_ai.c

Integration:
  Configuration:  TTTGUI(wtf)/model_config.h/c
  Game Logic:     TTTGUI(wtf)/game.h/c
  GUI:            TTTGUI(wtf)/gui_ai.c
```

## All Available AI Models

You have three types of AI algorithms, each with two dataset variants:

### 1. Naive Bayes (Probabilistic)
**Training**: `src/model training/naive_bayes.c`
**Models**: 
- `models/naive_bayes_non_terminal/model_non_terminal.txt`
- `models/naive_bayes_combined/model_combined.txt`

**Characteristics**:
- Probabilistic classification approach
- Fast prediction (~0.01ms per move)
- Good for pattern recognition
- Currently integrated in the game

### 2. Linear Regression (Numeric Evaluation)
**Training**: `src/model training/linear_regression.c`
**Models**:
- `models/linear_regression_non_terminal/model_non_terminal.txt`
- `models/linear_regression_combined/model_combined.txt`

**Characteristics**:
- Learns weights for board positions
- Very fast predictions
- Continuous score output
- Ready to integrate

### 3. Q-Learning (Reinforcement Learning)
**Training**: `src/model training/q_learning.c`
**Models**:
- `models/q learning/q_learning_non_terminal.txt`
- `models/q learning/q_learning_combined.txt`

**Characteristics**:
- State-action value learning
- Optimal policy discovery
- Large lookup tables (~18K entries)
- Learns through gameplay experience
- Ready to integrate

## Training Your Own Models

Use the comprehensive training script to train all models at once:

```bash
cd src
.\train_models_compare.bat
```

This will:
1. Process datasets (non-terminal and combined)
2. Train all 6 models
3. Save models to their respective directories
4. Generate training reports
5. Show accuracy metrics for each model

See `src/TRAINING_GUIDE.md` for detailed training instructions.

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

1. **Evaluate first**: Run `.\evaluate_models.bat` to see which model performs best
2. **Use presets**: Quick config changes with `ai_config_preset_*()` functions
3. **Keep backups**: Git commit before major changes
4. **Test incrementally**: Compile after each small change
5. **Use different levels**: Test same model at different difficulties
6. **Compare performance**: Track win/loss rates via in-game stats
7. **Document results**: Note accuracy, speed, gameplay feel
8. **Mix and match**: Try different dataset variants (non-terminal vs combined)
9. **Check console**: Model loading status printed at startup
10. **Display shows model**: AI label shows current model name (e.g., "AI: O (Q-Learning)")

## Quick Configuration Guide

### Want to test a specific model?

```c
// In gui_ai.c after game_load_all_models():
AIConfig config;
ai_config_init(&config);
ai_config_set_level(&config, 2, AI_MODEL_NAIVE_BAYES);  // Test NB at medium
game_set_ai_config(&config);
```

### Want to compare dataset variants?

```c
// Test non-terminal Q-Learning
game_load_model_file(AI_MODEL_Q_LEARNING, 
    "../models/q learning/q_learning_non_terminal.txt");
// Play games, record performance

// Test combined Q-Learning  
game_load_model_file(AI_MODEL_Q_LEARNING, 
    "../models/q learning/q_learning_combined.txt");
// Play games, compare
```

### Want all ML models?

```c
AIConfig config;
ai_config_preset_ml_showcase(&config);  // NB/QL/LR
game_set_ai_config(&config);
```

See `TTTGUI(wtf)/MODULAR_AI_GUIDE.md` for complete configuration options.

## Choosing the Right Model

**For Speed**:
- Linear Regression (fastest, <0.01ms)
- Naive Bayes (very fast, ~0.01ms)

**For Accuracy**:
- Check evaluation report from `evaluate_models.bat`
- Models with highest draw rate against Minimax are most optimal

**For Learning Approach**:
- Supervised: Naive Bayes, Linear Regression
- Reinforcement: Q-Learning

**Dataset Variants**:
- **Non-Terminal**: Better for strategic mid-game play
- **Combined**: More comprehensive, includes endgame positions

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
├── src/                                   # Training & evaluation programs
│   ├── model training/
│   │   ├── naive_bayes.c                 # Naive Bayes trainer
│   │   ├── linear_regression.c           # Linear Regression trainer
│   │   └── q_learning.c                  # Q-Learning trainer
│   ├── data related/
│   │   ├── dataset_processor.c           # Train/test split tool
│   │   └── dataset-gen.c                 # Dataset generator
│   ├── evaluate_against_minimax.c        # Model evaluation program
│   ├── evaluate_models.bat               # Evaluation script
│   ├── train_models_compare.bat          # Training script (all models)
│   ├── TRAINING_GUIDE.md                 # Training documentation
│   └── MODEL_EVALUATION_GUIDE.md         # Evaluation documentation
│
├── models/                                # Trained models (6 total)
│   ├── naive_bayes_non_terminal/
│   │   └── model_non_terminal.txt
│   ├── naive_bayes_combined/
│   │   └── model_combined.txt
│   ├── linear_regression_non_terminal/
│   │   ├── model_non_terminal.txt
│   │   └── model_non_terminal.bin
│   ├── linear_regression_combined/
│   │   ├── model_combined.txt
│   │   └── model_combined.bin
│   └── q learning/
│       ├── q_learning_non_terminal.txt
│       └── q_learning_combined.txt
│
├── dataset/                               # Training data
│   ├── tic-tac-toe.names                # Dataset metadata
│   ├── tic-tac-toe-minimax-non-terminal.data
│   ├── tic-tac-toe-minimax-complete.data
│   ├── new processed/                    # Split datasets
│   │   ├── train_non_terminal.data
│   │   ├── test_non_terminal.data
│   │   ├── train_combined.data
│   │   └── test_combined.data
│   └── new results/                      # Training reports
│       ├── report_non_terminal.txt
│       ├── report_combined.txt
│       └── dataset_report.txt
│
└── TTTGUI(wtf)/                          # Game integration
    ├── game.h
    ├── game.c
    ├── gui_ai.c
    ├── minimax.h
    ├── minimax.c
    ├── naive_bayes_ai.h                  # Currently integrated
    ├── naive_bayes_ai.c                  # Currently integrated
    └── your_model_ai.c                   # Add new models here
```

## Next Steps

### Quick Start (Using Existing Models)
1. ✅ Evaluate all 6 models: `cd src && .\evaluate_models.bat`
2. ✅ Review the comparison report: `model_evaluation_report.txt`
3. ✅ Choose the best performing model
4. ✅ Integrate it into the game (follow integration steps above)
5. ✅ Test in actual gameplay

### Training New Models
1. ✅ Generate dataset (if needed): `cd src/data related && .\dataset-gen.exe`
2. ✅ Train all models: `cd src && .\train_models_compare.bat`
3. ✅ Review training reports in `dataset/new results/`
4. ✅ Evaluate against Minimax: `.\evaluate_models.bat`
5. ✅ Integrate best model into game

### Adding Custom AI
1. ✅ Create your training program in `src/model training/`
2. ✅ Train your model using the datasets
3. ✅ Create interface files (`your_model_ai.h` and `.c`)
4. ✅ Follow integration steps in this guide
5. ✅ Evaluate against Minimax
6. ✅ Compare with existing models
7. ✅ Document your findings

### Resources
- **Training Guide**: `src/TRAINING_GUIDE.md`
- **Evaluation Guide**: `src/MODEL_EVALUATION_GUIDE.md`
- **Q-Learning Guide**: `src/Q_LEARNING_GUIDE.md`
- **Model Comparison**: Run `.\evaluate_models.bat` in `src/`

Happy experimenting! 🎮🤖
