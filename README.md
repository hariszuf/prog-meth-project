# Tic-Tac-Toe Machine Learning Project

Machine learning implementation with format-agnostic architecture.

## Project Overview

All three AI algorithms can be trained using either character or numerical matrix format and deployed in the same game without code changes.

## AI Models (Matrix-Trained)

| Model | Accuracy | Status |
|-------|----------|--------|
| Q-Learning | ~88% | Implemented |
| Linear Regression | 57.85% | Implemented |
| Naive Bayes | 52.19% | Implemented |

## Project Structure

```
prog-meth-project/
├── TTTGUI (Main Game Folder)/
│   ├── ttt_gui.exe
│   ├── game.c/h
│   └── *_ai.c/h
├── models/
│   ├── linear_regression_non_terminal/
│   ├── naive_bayes_non_terminal/
│   ├── q learning/
│   └── backups/
├── dataset/
│   └── new processed/
├── src/
│   ├── model training matrix/
│   ├── q-learning training/
│   └── data related/
├── evaluation/
└── experiment/
```

## Quick Start

### Play the Game
```bash
cd "TTTGUI (Main Game Folder)"
./ttt_gui.exe
```

### Train Models
```bash
cd experiment
./train_matrix_models_simple.bat
```

### Deploy Models
```bash
cd experiment
./implement_matrix_models.bat
```

## Training Details

### Dataset Format
- Features: 1.0 (X), -1.0 (O), 0.0 (empty)
- Outcomes: +1 (win), 0 (draw), -1 (lose)
- Samples: 4,382 combined, 3,616 non-terminal
- Split: 80/20 train/test

### Algorithms
- Linear Regression: Batch Gradient Descent, 1000 epochs, lr=0.01
- Naive Bayes: Probability-based with Laplace smoothing
- Q-Learning: Temporal Difference, 50,000 episodes

## Technical Architecture

### Format-Agnostic Design
The game code remains unchanged regardless of training format. Models trained on numerical matrix format work identically to character format models.

### Model Storage
- Linear Regression: 10 weights (bias + 9 features)
- Naive Bayes: Probability tables
- Q-Learning: State-action Q-values

## Performance Results

- Linear Regression: 57.85% accuracy, 61.53% precision, 79.48% recall
- Naive Bayes: 52.19% accuracy, 63.37% precision, 58.82% recall
- Q-Learning: ~88% accuracy

## Development Workflow

1. Process datasets: `dataset_processor_matrix.c`
2. Train models: `train_matrix_models_simple.bat`
3. Evaluate: Built-in metrics
4. Implement: `implement_matrix_models.bat`
5. Play: `ttt_gui.exe`

## Documentation

- Game Guide: `TTTGUI (Main Game Folder)/README.md`
- Evaluation Results: `evaluation/MATRIX_MODEL_EVALUATION_SUMMARY.txt`
- Q-Learning Guide: `src/Q_LEARNING_GUIDE.md`

## Technical Requirements

- Compiler: GCC (MinGW-w64)
- Graphics: Raylib
- Platform: Windows
- Language: C

## Model Format

All models use text format:
- Linear Regression: Plain text weights
- Naive Bayes: Probability tables
- Q-Learning: State-action Q-values

## Key Features

- Three AI algorithms (Q-Learning, Linear Regression, Naive Bayes)
- Matrix-trained models
- Format-agnostic architecture
- Automatic model backups
- Comprehensive evaluation tools
- Graphical UI with Raylib
- Ternary classification (win/draw/lose)
