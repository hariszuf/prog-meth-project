# Adding New AI Models

Guide for adding new AI models to the game.

## Current Models

Six trained models available:

### Naive Bayes
- `models/naive_bayes_non_terminal/model_non_terminal.txt` - Strategic positions
- `models/naive_bayes_combined/model_combined.txt` - All positions

### Linear Regression
- `models/linear_regression_non_terminal/model_non_terminal.txt` - Strategic positions
- `models/linear_regression_combined/model_combined.txt` - All positions

### Q-Learning
- `models/q learning/q_learning_non_terminal.txt` - Strategic positions
- `models/q learning/q_learning_combined.txt` - All positions

## Adding a New Model

### 1. Train Your Model

Create training program in `src/model training/`:
```c
// Export to models/your_model/model.txt
```

### 2. Create AI Interface

**your_model_ai.h**:
```c
#ifndef YOUR_MODEL_AI_H
#define YOUR_MODEL_AI_H

typedef struct {
    // Model parameters
} YourModel;

int your_model_load(const char *filename, YourModel *model);
int your_model_find_best_move(const YourModel *model, char board[9]);

#endif
```

**your_model_ai.c**:
```c
#include "your_model_ai.h"

int your_model_load(const char *filename, YourModel *model) {
    // Load model from file
    return 1;
}

int your_model_find_best_move(const YourModel *model, char board[9]) {
    // Return best move (0-8)
    return 0;
}
```

### 3. Integrate into Game

Add to `model_config.h`:
```c
typedef enum {
    AI_MODEL_NAIVE_BAYES,
    AI_MODEL_LINEAR_REGRESSION,
    AI_MODEL_Q_LEARNING,
    AI_MODEL_YOUR_MODEL,  // Add this
    AI_MODEL_MINIMAX_EASY,
    AI_MODEL_MINIMAX_HARD
} AIModelType;
```

Add to `game.h`:
```c
#include "your_model_ai.h"

typedef struct {
    NaiveBayesModel nb_model;
    LinearRegressionModel lr_model;
    QLearningModel ql_model;
    YourModel your_model;  // Add this
    // ...
} GameState;
```

Load in `gui_ai.c`:
```c
void game_load_all_models(void) {
    // ...existing loads...
    
    if (!your_model_load("../models/your_model/model.txt", &game.your_model)) {
        printf("Warning: Could not load your model\n");
    }
}
```

Route in `game.c`:
```c
int game_get_ai_move(void) {
    switch (game.ai_config.medium_model) {
        case AI_MODEL_YOUR_MODEL:
            return your_model_find_best_move(&game.your_model, game.board);
        // ...
    }
}
```

### 4. Compile and Test

```bash
cd TTTGUI
.\compile.bat
.\ttt_gui.exe
```
