# Modular AI System - Quick Reference

## ✅ What's New

Your tic-tac-toe game now has a **fully modular AI system** that makes it easy to swap between all 6 trained models and demonstrate different AI approaches.

## 🎯 Current Setup

### Default Configuration (Best Models)
```
Easy (Level 1):   Minimax (Easy) - Imperfect minimax with depth limit
Medium (Level 2): Q-Learning     - Best ML model (95%+ draw rate vs Minimax)
Hard (Level 3):   Minimax (Hard) - Perfect unbeatable play
```

### Files Created
```
TTTGUI(wtf)/
├── model_config.h/c          ✅ Configuration system
├── linear_regression_ai.h/c  ✅ LR interface (NEW)
├── q_learning_ai.h/c         ✅ QL interface (NEW)
├── game.h/c                  ✅ Enhanced with modular routing
├── gui_ai.c                  ✅ Shows current AI model name
├── compile.bat               ✅ Updated build script
└── MODULAR_AI_GUIDE.md       ✅ Complete usage guide
```

## 🚀 Quick Start

```bash
cd TTTGUI(wtf)
.\compile.bat
.\ttt_gui.exe
```

The game will:
1. Load all 6 AI models at startup
2. Display which model is active (e.g., "AI: O (Q-Learning)")
3. Route to the correct AI based on difficulty level

## 🔄 Swapping Models

### Use Preset Configurations

Edit `gui_ai.c` after `game_load_all_models()`:

```c
// Option 1: Best Models (default)
AIConfig config;
ai_config_preset_best_models(&config);
game_set_ai_config(&config);

// Option 2: ML Showcase (all ML models)
AIConfig config;
ai_config_preset_ml_showcase(&config);
game_set_ai_config(&config);

// Option 3: Traditional (all Minimax)
AIConfig config;
ai_config_preset_minimax_only(&config);
game_set_ai_config(&config);
```

### Custom Configuration

```c
AIConfig config;
ai_config_init(&config);
ai_config_set_level(&config, 1, AI_MODEL_NAIVE_BAYES);
ai_config_set_level(&config, 2, AI_MODEL_LINEAR_REGRESSION);
ai_config_set_level(&config, 3, AI_MODEL_Q_LEARNING);
game_set_ai_config(&config);
```

## 📦 Available Models

| Model | Type | Speed | Best Level | Dataset Variants |
|-------|------|-------|------------|------------------|
| Naive Bayes | ML | Very Fast | Easy/Medium | Non-Terminal, Combined |
| Linear Regression | ML | Very Fast | Easy/Medium | Non-Terminal, Combined |
| Q-Learning | ML | Fast | Medium/Hard | Non-Terminal, Combined |
| Minimax (Easy) | Algorithmic | Medium | Easy | N/A (depth-limited) |
| Minimax (Hard) | Algorithmic | Slow | Hard | N/A (perfect play) |

## 🎮 For Demonstrations

### Show All ML Models
```c
ai_config_preset_ml_showcase(&config);
// Easy: Naive Bayes
// Medium: Q-Learning  
// Hard: Linear Regression
```

### Compare ML vs Traditional
```c
config.easy_model = AI_MODEL_MINIMAX_EASY;
config.medium_model = AI_MODEL_Q_LEARNING;
config.hard_model = AI_MODEL_MINIMAX_HARD;
```

### Test Dataset Variants
```c
// Load non-terminal version
game_load_model_file(AI_MODEL_Q_LEARNING, 
    "../models/q learning/q_learning_non_terminal.txt");

// Later, load combined version
game_load_model_file(AI_MODEL_Q_LEARNING, 
    "../models/q learning/q_learning_combined.txt");
```

## 📊 Model Performance (vs Perfect Minimax)

Based on evaluation in `src/model_evaluation_report.txt`:

```
Q-Learning (Combined):      95%+ draws (BEST)
Linear Regression:          90%+ draws
Naive Bayes:                90%+ draws
```

Use this data to choose models for each difficulty level.

## 🛠️ Adding New Models

1. Create `your_model_ai.h/c` in `TTTGUI(wtf)/`
2. Add `AI_MODEL_YOUR_MODEL` to `model_config.h`
3. Add loading in `game_load_all_models()` in `game.c`
4. Add case in `game_ai_move()` in `game.c`
5. Update `compile.bat` to include new files
6. Compile and test

See `ADDING_AI_MODELS.md` for detailed instructions.

## 📁 Key Files to Edit

### To Change Configuration
- `gui_ai.c` - Set active configuration after loading models
- `model_config.c` - Create custom presets

### To Add New Model
- `TTTGUI(wtf)/your_model_ai.h/c` - Interface implementation
- `model_config.h` - Add to enum
- `game.c` - Add loading and routing logic
- `compile.bat` - Include in build

### To Swap Dataset Variants
- `game.c` in `game_load_all_models()` - Change file paths
- Or use `game_load_model_file()` at runtime

## 🎯 Recommended Configurations

### For Best Performance
```c
ai_config_preset_best_models(&config);
Easy:   Minimax (Easy)
Medium: Q-Learning (Combined)
Hard:   Minimax (Hard)
```

### For ML Demonstration
```c
ai_config_preset_ml_showcase(&config);
Easy:   Naive Bayes
Medium: Q-Learning
Hard:   Linear Regression
```

### For Consistent Traditional AI
```c
ai_config_preset_minimax_only(&config);
Easy:   Minimax (Easy)
Medium: Minimax (Easy)
Hard:   Minimax (Hard)
```

## 📖 Documentation

- **Complete Guide**: `TTTGUI(wtf)/MODULAR_AI_GUIDE.md`
- **Adding Models**: `ADDING_AI_MODELS.md`
- **Training**: `src/TRAINING_GUIDE.md`
- **Evaluation**: `src/MODEL_EVALUATION_GUIDE.md`
- **Q-Learning**: `src/Q_LEARNING_GUIDE.md`

## ✨ Features

✅ Load all 6 models at once  
✅ Switch configurations with one function call  
✅ Swap dataset variants at runtime  
✅ Display current AI model in-game  
✅ Easy to add new models  
✅ Preset configurations for quick demos  
✅ Clean separation of concerns  
✅ Type-safe enum-based routing  

## 🎬 Next Steps

1. ✅ Compile: `.\compile.bat`
2. ✅ Test default setup: Play at all difficulty levels
3. ✅ Try presets: Edit `gui_ai.c` and recompile
4. ✅ Compare models: Track win/loss rates
5. ✅ Find your favorite: Use best-performing configuration
6. ✅ Demonstrate: Show different AI approaches

---

**Your game is now fully modular and ready for demonstrations!** 🎮🤖
