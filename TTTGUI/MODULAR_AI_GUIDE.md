# Modular AI System Guide

## Overview

The game now uses a **fully modular AI system** that allows you to easily swap between all 6 trained models (and Minimax variants) at any difficulty level.

## Quick Start

### Default Configuration (Best Models)

```
Easy (Level 1):   Minimax (Easy) - Imperfect play with depth limit
Medium (Level 2): Q-Learning     - Best ML model from evaluation
Hard (Level 3):   Minimax (Hard) - Perfect unbeatable play
```

### Compile and Run

```bash
cd TTTGUI(wtf)
.\compile.bat
.\ttt_gui.exe
```

The game will automatically load all AI models at startup and display which model is being used.

## Architecture

### Components

1. **`model_config.h/c`** - Centralized AI configuration system
2. **`game.h/c`** - Enhanced game logic with modular AI routing
3. **AI Interface Files** (one pair per model type):
   - `naive_bayes_ai.h/c`
   - `linear_regression_ai.h/c`
   - `q_learning_ai.h/c`
   - `minimax.h/c`

### How It Works

```c
// Configuration defines which AI to use at each level
AIConfig config;
config.easy_model = AI_MODEL_MINIMAX_EASY;
config.medium_model = AI_MODEL_Q_LEARNING;
config.hard_model = AI_MODEL_MINIMAX_HARD;

// Game routes to the appropriate AI based on level
game_ai_move(&game, level); // Automatically uses the right model
```

## Changing AI Models

### Method 1: Use Preset Configurations (Easiest)

Edit `gui_ai.c` in the `main()` function after loading models:

```c
// Load all models
game_load_all_models();

// Apply a preset configuration
AIConfig config;
ai_config_preset_ml_showcase(&config);  // Use ML models at all levels
game_set_ai_config(&config);
```

**Available Presets:**

- `ai_config_preset_default()` - Original: NB/Minimax-Med/Minimax-Hard
- `ai_config_preset_best_models()` - **Current**: Minimax-Easy/Q-Learning/Minimax-Hard
- `ai_config_preset_ml_showcase()` - All ML: NB/Q-Learning/Linear Regression
- `ai_config_preset_minimax_only()` - Traditional: All Minimax variants

### Method 2: Custom Configuration

```c
// Create custom configuration
AIConfig config;
ai_config_init(&config);

// Set each level individually
ai_config_set_level(&config, 1, AI_MODEL_NAIVE_BAYES);
ai_config_set_level(&config, 2, AI_MODEL_LINEAR_REGRESSION);
ai_config_set_level(&config, 3, AI_MODEL_Q_LEARNING);

// Apply configuration
game_set_ai_config(&config);
```

### Method 3: Runtime Configuration (Advanced)

Modify `model_config.c` to add your own presets:

```c
void ai_config_preset_my_config(AIConfig *config) {
    config->easy_model = AI_MODEL_LINEAR_REGRESSION;
    config->medium_model = AI_MODEL_Q_LEARNING;
    config->hard_model = AI_MODEL_NAIVE_BAYES;
}
```

## Swapping Between Dataset Variants

Each ML model has two trained variants:
- **Non-Terminal**: Trained on strategic positions only
- **Combined**: Trained on all game states

### Default Loaded Models

```c
Naive Bayes:        models/naive_bayes_non_terminal/model_non_terminal.txt
Linear Regression:  models/linear_regression_non_terminal/model_non_terminal.txt
Q-Learning:         models/q learning/q_learning_combined.txt  ← Combined (best)
```

### Swap to Different Variant

Edit `game.c` in `game_load_all_models()`:

```c
// Change Q-Learning from combined to non-terminal
if (ql_load_model("../models/q learning/q_learning_non_terminal.txt", &ql_model)) {
    ql_model_loaded = 1;
}

// Change Naive Bayes from non-terminal to combined
if (nb_load_model("../models/naive_bayes_combined/model_combined.txt", &nb_model)) {
    nb_model_loaded = 1;
}
```

Or reload at runtime:

```c
// In your code, swap to combined Naive Bayes
game_load_model_file(AI_MODEL_NAIVE_BAYES, 
                    "../models/naive_bayes_combined/model_combined.txt");
```

## Available AI Models

### Model Types Enum

```c
AI_MODEL_NAIVE_BAYES           // Probabilistic classification
AI_MODEL_LINEAR_REGRESSION     // Numeric position evaluation
AI_MODEL_Q_LEARNING            // Reinforcement learning
AI_MODEL_MINIMAX_EASY          // Imperfect minimax (depth 3)
AI_MODEL_MINIMAX_HARD          // Perfect minimax (full depth)
```

### Model Characteristics

| Model | Speed | Accuracy | Learning Type | Best Use Case |
|-------|-------|----------|---------------|---------------|
| Naive Bayes | Very Fast | Good | Supervised | Easy/Medium |
| Linear Regression | Very Fast | Good | Supervised | Easy/Medium |
| Q-Learning | Fast | Excellent | Reinforcement | Medium/Hard |
| Minimax Easy | Medium | Good | Algorithmic | Easy |
| Minimax Hard | Slow | Perfect | Algorithmic | Hard |

## Demonstration Mode

For demonstrations, you can quickly switch configurations:

### Setup 1: Show All ML Models
```c
AIConfig config;
ai_config_preset_ml_showcase(&config);
game_set_ai_config(&config);
```

Play against:
- Level 1: Naive Bayes
- Level 2: Q-Learning
- Level 3: Linear Regression

### Setup 2: Traditional vs ML Comparison
```c
AIConfig config;
config.easy_model = AI_MODEL_MINIMAX_EASY;
config.medium_model = AI_MODEL_Q_LEARNING;
config.hard_model = AI_MODEL_MINIMAX_HARD;
game_set_ai_config(&config);
```

### Setup 3: Same Model, Different Datasets
Manually load different variants and test performance:

```c
// Test 1: Non-terminal Q-Learning
game_load_model_file(AI_MODEL_Q_LEARNING, 
    "../models/q learning/q_learning_non_terminal.txt");
// Play games, record stats

// Test 2: Combined Q-Learning
game_load_model_file(AI_MODEL_Q_LEARNING, 
    "../models/q learning/q_learning_combined.txt");
// Play games, compare stats
```

## In-Game Display

The game automatically displays which AI model is active:

```
Player 1: X
AI: O (Q-Learning)    ← Shows current model name
```

The name updates automatically based on difficulty level selected.

## Code Structure

### Adding Your Own AI Model

1. **Create interface files** (`your_model_ai.h/c`)
2. **Add to enum** in `model_config.h`:
   ```c
   typedef enum {
       // ... existing models
       AI_MODEL_YOUR_MODEL
   } AIModelType;
   ```

3. **Add to game.c**:
   ```c
   static YourModel your_model;
   static int your_model_loaded = 0;
   
   // In game_load_all_models():
   if (your_model_load("path/to/model.txt", &your_model)) {
       your_model_loaded = 1;
   }
   
   // In game_ai_move():
   case AI_MODEL_YOUR_MODEL:
       if (your_model_loaded) {
           mv = your_model_find_best_move(&your_model, g->b);
       }
       break;
   ```

4. **Update compile.bat** to include new files
5. **Add name mapping** in `model_config.c`

## Testing Different Configurations

### Systematic Testing Approach

```c
// Test each model at medium difficulty
AIModelType models[] = {
    AI_MODEL_NAIVE_BAYES,
    AI_MODEL_LINEAR_REGRESSION,
    AI_MODEL_Q_LEARNING
};

for (int i = 0; i < 3; i++) {
    AIConfig config;
    ai_config_set_level(&config, 2, models[i]);
    game_set_ai_config(&config);
    
    // Play 10 games, record win/loss rates
    // Compare performance
}
```

### Evaluation Results Reference

Check `src/model_evaluation_report.txt` to see how each model performs against perfect Minimax play:

```
Best performing model: Q-Learning (Combined)
Draw rate: 95%+
Loss rate: <5%
```

Use this data to inform your configuration choices.

## Best Practices

1. **Load models once at startup** - Don't reload unnecessarily
2. **Use Q-Learning (combined) for medium** - Best evaluated performance
3. **Use Minimax (hard) for hard** - Guarantees unbeatable play
4. **Use Minimax (easy) for easy** - Provides appropriate challenge
5. **Test configurations** - Play multiple games to verify behavior
6. **Check console output** - Models print loading status

## Troubleshooting

### Model not loading
```
✗ Failed to load Q-Learning model
```
**Solution**: Check file paths in `game.c`, ensure models exist

### Wrong AI behavior
**Solution**: Verify configuration with `game_get_ai_config()`, check console output

### Compilation errors
**Solution**: Ensure all `.c` files in `compile.bat`, check include paths

### Model crashes game
**Solution**: Check model file format matches loading code, verify memory allocation

## File Locations Summary

```
TTTGUI(wtf)/
├── model_config.h/c          ← Configuration system
├── game.h/c                  ← Enhanced game logic
├── naive_bayes_ai.h/c        ← NB interface
├── linear_regression_ai.h/c  ← LR interface (NEW)
├── q_learning_ai.h/c         ← QL interface (NEW)
├── minimax.h/c               ← Minimax interface
├── compile.bat               ← Updated build script
└── gui_ai.c                  ← GUI with model display

models/
├── naive_bayes_non_terminal/
├── naive_bayes_combined/
├── linear_regression_non_terminal/
├── linear_regression_combined/
└── q learning/
    ├── q_learning_non_terminal.txt
    └── q_learning_combined.txt  ← Best performing
```

## Next Steps

1. ✅ Compile with `.\compile.bat`
2. ✅ Run `.\ttt_gui.exe`
3. ✅ Test default configuration
4. ✅ Try different presets
5. ✅ Compare performance
6. ✅ Choose your favorite setup
7. ✅ Document your findings

Happy gaming with your modular AI system! 🎮🤖
