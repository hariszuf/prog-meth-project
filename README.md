# Tic-Tac-Toe Machine Learning Project

A comprehensive machine learning project demonstrating **format-agnostic architecture** for training and deploying AI models in a Tic-Tac-Toe game.

## 🎯 Project Overview

This project successfully demonstrates that **training format (character vs matrix) does not affect game implementation**. All three AI algorithms can be trained using either format and seamlessly deployed in the same game without code changes.

### Key Achievement
✅ **Format-Agnostic Architecture Proven**: Models trained on numerical matrix format (1.0, -1.0, 0.0) work identically to character format (x, o, b) models in the game.

## 🤖 AI Models (All Matrix-Trained)

| Model | Accuracy | Characteristics | Status |
|-------|----------|-----------------|--------|
| **Q-Learning** | ~88% | Best performer, strategic play | ✅ Implemented |
| **Linear Regression** | 57.85% | Aggressive, high recall (79.48%) | ✅ Implemented |
| **Naive Bayes** | 52.19% | Balanced, conservative | ✅ Implemented |

## 📁 Project Structure

```
prog-meth-project/
├── TTTGUI (Main Game Folder)/   # Main game with Raylib GUI
│   ├── ttt_gui.exe              # Compiled game (matrix-trained AIs)
│   ├── game.c/h                 # Core game logic
│   ├── *_ai.c/h                 # AI implementations (unchanged)
│   └── README.md                # Game documentation
│
├── models/                       # Trained models
│   ├── matrix_models_nonterminal/  # Matrix-trained models
│   │   ├── linear_regression/
│   │   ├── naive_bayes/
│   │   └── q_learning/
│   ├── linear_regression_non_terminal/  # Active in game
│   ├── naive_bayes_non_terminal/        # Active in game
│   ├── q learning/                      # Active in game
│   └── backups/                         # Model backups
│
├── dataset/                     # Training datasets
│   ├── new processed/          # Matrix format datasets
│   │   ├── train_combined_matrix.data (4,382 samples)
│   │   ├── test_combined_matrix.data (1,096 samples)
│   │   ├── train_non_terminal_matrix.data (3,616 samples)
│   │   └── test_non_terminal_matrix.data (904 samples)
│   └── new results/            # Dataset reports
│
├── src/                        # Training code
│   ├── model training matrix/  # Matrix format trainers
│   │   ├── linear_regression_matrix.c (BGD implementation)
│   │   └── naive_bayes_matrix.c
│   ├── q-learning training/    # Q-Learning trainer
│   │   └── unified_q_trainer.c (format-agnostic)
│   └── data related/           # Dataset processing
│       └── dataset_processor_matrix.c (ternary classification)
│
├── evaluation/                 # Model evaluation
│   ├── MATRIX_MODEL_EVALUATION_SUMMARY.txt  # Complete results
│   ├── confusion-matrix.c      # Confusion matrix generator
│   └── error-matrix.c          # 9x9 error analysis
│
└── experiment/                 # Training & deployment scripts
    ├── train_matrix_models_simple.bat      # Train LR & NB
    └── implement_matrix_models.bat         # Deploy to game
```

## 🚀 Quick Start

### 1. Play the Game
```bash
cd "TTTGUI (Main Game Folder)"
./ttt_gui.exe
```

The game is **ready to play** with all three matrix-trained AI models.

### 2. Train New Models (Optional)
```bash
cd experiment
./train_matrix_models_simple.bat
```

### 3. Implement Models (Optional)
```bash
cd experiment
./implement_matrix_models.bat
# Choose option [3] for both models
```

## 📊 Training Details

### Dataset Format: Matrix (Ternary Classification)
- **Features**: 1.0 (X), -1.0 (O), 0.0 (empty)
- **Outcomes**: +1 (win), 0 (draw), -1 (lose)
- **Samples**: 4,382 combined, 3,616 non-terminal
- **Split**: 80/20 train/test

### Algorithms
- **Linear Regression**: Batch Gradient Descent, 1000 epochs, lr=0.01
- **Naive Bayes**: Probability-based with Laplace smoothing
- **Q-Learning**: Temporal Difference, 50,000 episodes, ε-greedy

## 🔬 Technical Architecture

### Format-Agnostic Design
```
Training Format (Input)         Model Format (Internal)      Game Format (Output)
─────────────────────────────  ─────────────────────────  ───────────────────────
Matrix: 1.0, -1.0, 0.0    -->  Weights/Probabilities  -->  Character: x, o, b
   OR                          Q-values                     (Game uses this)
Character: x, o, b        -->  (Same internal format) -->
```

**Key Insight**: The game code is **completely unchanged** regardless of training format. Only the training pipeline differs.

### Model Storage
- **Linear Regression**: 10 weights (bias + 9 features)
- **Naive Bayes**: Probability tables per feature state
- **Q-Learning**: State-action Q-value mappings

## 📈 Performance Comparison

### Test Results (Combined Dataset)

**Linear Regression:**
```
Accuracy: 57.85%
Precision: 61.53%
Recall: 79.48%
→ Aggressive, over-predicts wins
```

**Naive Bayes:**
```
Accuracy: 52.19%
Precision: 63.37%
Recall: 58.82%
→ Balanced, conservative
```

**Q-Learning:**
```
Accuracy: ~88%
→ Best overall performer
```

## 🛠️ Development Workflow

### Complete Pipeline
1. **Process datasets**: `dataset_processor_matrix.c` → matrix format
2. **Train models**: `train_matrix_models_simple.bat` → trained models
3. **Evaluate**: Built-in metrics during training
4. **Implement**: `implement_matrix_models.bat` → game deployment
5. **Play**: `ttt_gui.exe` with matrix-trained AIs

### Rollback
```bash
# Backups stored in models/backups/
# Format: *_backup_YYYYMMDD.txt
# Copy back to original locations if needed
```

## 📚 Documentation

- **Game Guide**: `TTTGUI (Main Game Folder)/README.md`
- **Evaluation Results**: `evaluation/MATRIX_MODEL_EVALUATION_SUMMARY.txt`
- **Model Evaluation Guide**: `evaluation/MODEL_EVALUATION_GUIDE.md`
- **Q-Learning Guide**: `src/Q_LEARNING_GUIDE.md`

## 🎓 Academic Contributions

1. **Format-Agnostic Architecture**: Proven that training format is decoupled from deployment
2. **Ternary Classification**: Includes draws (not binary win/lose)
3. **Batch Gradient Descent**: Proper BGD implementation in Linear Regression
4. **Comprehensive Evaluation**: Confusion matrices, error matrices, performance metrics

## 🔧 Technical Requirements

- **Compiler**: GCC (MinGW-w64)
- **Graphics**: Raylib (for GUI)
- **Platform**: Windows
- **Language**: C

## 📦 Model Files

All models use **text format** for transparency:
- Linear Regression: Plain text weights
- Naive Bayes: Probability tables
- Q-Learning: State-action pairs with Q-values

No binary files needed - all models are human-readable.

## ✨ Key Features

- ✅ Three different AI algorithms
- ✅ Matrix-trained models (numerical format)
- ✅ Format-agnostic game code
- ✅ Automatic model backup system
- ✅ Comprehensive evaluation tools
- ✅ Easy retraining and deployment
- ✅ Graphical UI with Raylib
- ✅ Ternary classification (win/draw/lose)

## 🏆 Results Summary

**Successfully implemented a complete ML pipeline** demonstrating:
- Training on matrix format (1.0, -1.0, 0.0)
- Deploying to character-based game (x, o, b)
- Zero game code changes required
- All three algorithms working identically

This proves the **format-agnostic architecture** concept: the training representation is independent of the deployment representation.

## 📝 License

Academic project - free to use and modify.

## 👥 Contributors

- Machine Learning implementations
- Format-agnostic architecture design
- Comprehensive evaluation framework

---

**Status**: ✅ Complete and ready to play
**Last Updated**: November 26, 2025
**Game Version**: ttt_gui.exe (2.77 MB)
