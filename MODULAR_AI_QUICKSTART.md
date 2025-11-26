# Modular AI System Quick Reference

## Current Setup

### Default Configuration
```
Easy (Level 1):   Minimax (Easy)
Medium (Level 2): Q-Learning
Hard (Level 3):   Minimax (Hard)
```

## Usage

```bash
cd TTTGUI
.\compile.bat
.\ttt_gui.exe
```

## Swapping Models

Edit `gui_ai.c` after `game_load_all_models()`:

### Preset Configurations

```c
// Best Models (default)
AIConfig config;
ai_config_preset_best_models(&config);
game_set_ai_config(&config);

// ML Showcase
AIConfig config;
ai_config_preset_ml_showcase(&config);
game_set_ai_config(&config);

// Minimax Only
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

## Available Models

| Model | Type | Speed |
|-------|------|-------|
| Naive Bayes | ML | Very Fast |
| Linear Regression | ML | Very Fast |
| Q-Learning | ML | Fast |
| Minimax (Easy) | Algorithmic | Medium |
| Minimax (Hard) | Algorithmic | Slow |
