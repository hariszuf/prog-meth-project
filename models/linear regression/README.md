# Linear Regression AI Integration

## Overview
This document explains the Linear Regression AI model that has been integrated into the Tic-Tac-Toe game as the **Medium difficulty level**.

## Training the Model

### Dataset
- Training set: 766 instances
- Test set: 192 instances
- Format: 9 board positions (x, o, b) + label (win, lose, draw)

### Model Architecture
**Linear Regression** with 10 features:
- Feature 0: Bias term (constant 1.0)
- Features 1-9: Board positions encoded as:
  - `x` = +1.0 (X player's piece)
  - `o` = -1.0 (O player's piece)
  - `b` = 0.0 (blank/empty)

### Training Configuration
```c
Epochs: 1000
Learning Rate: 0.01
Optimizer: Stochastic Gradient Descent (SGD)
Loss Function: Mean Squared Error (MSE)
```

### Performance
```
Test Accuracy: 96.35% (185/192 correct)
- Win predictions: 100.00% (114/114)
- Lose predictions: 100.00% (71/71)
- Draw predictions: 0.00% (0/7)
```

**Note**: The model struggles with draw predictions (0%) because draws are rare in the dataset (only 7 instances). However, it's highly accurate at predicting wins and losses.

## How It Works

### 1. Training Process
The training program (`src/linear_regression.c`) implements:
- Feature extraction from board states
- Stochastic gradient descent optimization
- Weight updates: `w[i] += learning_rate * error * feature[i]`
- Model saving to both text and binary formats

### 2. Prediction
The model predicts a continuous value:
- **>0.5**: Win likely for X (bad for O)
- **<-0.5**: Lose likely for X (good for O)
- **Between -0.5 and 0.5**: Draw or uncertain

### 3. Move Selection
When selecting a move for O (the AI):
1. Try each empty cell
2. Simulate placing O there
3. Predict the outcome
4. Choose the move with the **lowest** prediction (since we want X to lose)
5. Add 10% randomness to make it less predictable

## Integration into Game

### New Difficulty Levels
The game now supports **4 difficulty levels**:

| Level | Button | AI Algorithm | Description |
|-------|--------|--------------|-------------|
| 1 | E | Naive Bayes | Easy - Probabilistic decisions |
| 2 | M | **Linear Regression** | **Medium - Gradient-based predictions** |
| 3 | H | Minimax (depth 3) | Hard - Looks ahead 3 moves |
| 4 | X | Full Minimax | Expert - Perfect play (unbeatable) |

### Files Created
```
src/
  └── linear_regression.c          # Training program

models/linear regression/
  ├── model.txt                    # Human-readable weights
  └── model.bin                    # Binary format (faster loading)

TTTGUI(wtf)/
  ├── linear_regression_ai.h       # Header file
  ├── linear_regression_ai.c       # AI implementation
  ├── game.c                       # Updated with LR support
  ├── game.h                       # Updated with LR functions
  ├── gui_ai.c                     # Updated with 4 levels
  ├── compile.bat                  # Updated compilation
  └── command.txt                  # Updated commands
```

## Compilation

### Train the Model
```bash
cd src
gcc linear_regression.c -o linear_regression.exe -lm -Wall
./linear_regression.exe
```

### Compile the Game
```bash
cd TTTGUI(wtf)
./compile.bat
```

Or manually:
```bash
gcc gui_ai.c game.c minimax.c naive_bayes_ai.c linear_regression_ai.c stats.c \
    -o ttt_gui.exe \
    -I"C:\raylib\raylib\src" -L"C:\raylib\raylib\src" \
    -lraylib -lopengl32 -lgdi32 -lwinmm -luser32 -lshell32 -lws2_32 -Wall
```

## Model Weights
The trained model learned these weights:

```
Bias:        -1.0053
Top-Left:     1.9899
Top-Middle:   1.9729
Top-Right:    1.9694
Middle-Left:  1.9822
Center:       1.9734
Middle-Right: 1.9803
Bottom-Left:  1.9876
Bottom-Mid:   1.9821
Bottom-Right: 1.9758
```

### Weight Interpretation
- **Positive weights** (~2.0): Each X piece increases the prediction toward "win for X"
- **Negative bias** (-1.0): Baseline shifts toward O winning
- **Similar values**: All positions are roughly equally important

The model essentially learns: `score = -1 + 2*(count of X's) - 2*(count of O's)`

## Comparison with Other AIs

### Naive Bayes vs Linear Regression vs Minimax

| Feature | Naive Bayes | Linear Regression | Minimax |
|---------|-------------|-------------------|---------|
| **Approach** | Probabilistic | Gradient-based | Game tree search |
| **Training** | Yes | Yes | No (rule-based) |
| **Speed** | Fast | Fast | Slower (at high depths) |
| **Accuracy** | ~88% | ~96% | 100% (perfect) |
| **Draws** | Good | Poor | Perfect |
| **Predictability** | Medium | Medium | High (at max depth) |
| **Difficulty** | Easy | Medium | Hard/Expert |

### Why Linear Regression for Medium?
1. **Better than Naive Bayes**: Higher accuracy (96% vs 88%)
2. **Faster than Minimax**: Constant time O(1) vs exponential O(9!)
3. **More human-like**: Makes occasional mistakes, not perfect
4. **Balanced challenge**: Harder than NB, easier than perfect Minimax

## Usage

1. **Start the game**: `./ttt_gui.exe`
2. **Select "Player vs AI"**
3. **Click "M" for Medium difficulty** (Linear Regression)
4. **Play and enjoy!**

The AI will make smart moves based on the trained weights, with occasional randomness to keep it interesting.

## Retraining

To retrain with different hyperparameters, edit `src/linear_regression.c`:

```c
int epochs = 1000;           // Number of training iterations
double learning_rate = 0.01; // Step size for weight updates
```

Then recompile and run:
```bash
gcc linear_regression.c -o linear_regression.exe -lm -Wall
./linear_regression.exe
```

The new model will be saved to `models/linear regression/model.txt`.

## Technical Details

### Algorithm: Gradient Descent
For each training example:
1. **Forward pass**: Compute prediction
   ```c
   prediction = w[0] + w[1]*x[1] + ... + w[9]*x[9]
   ```

2. **Calculate error**:
   ```c
   error = actual_label - prediction
   ```

3. **Update weights**:
   ```c
   w[i] = w[i] + learning_rate * error * x[i]
   ```

### Loss Function
Mean Squared Error (MSE):
```
MSE = (1/N) * Σ(actual - predicted)²
```

Converged to: **0.012211** after 100 epochs

### Encoding Scheme
Board → Features:
```
[X][O][ ]     [1.0, 1.0, -1.0, 0.0, ...]
[ ][X][O]  →  (bias=1.0, then board values)
[O][ ][X]
```

Prediction → Class:
- `pred > 0.5` → "win" (for X)
- `pred < -0.5` → "lose" (for X, i.e., win for O)
- `-0.5 <= pred <= 0.5` → "draw"

## Future Improvements

1. **Better draw handling**: Add more draw examples or use class weights
2. **Feature engineering**: Add features like "two in a row", "center control"
3. **Polynomial features**: Include x[i]*x[j] interaction terms
4. **Regularization**: Add L1/L2 penalty to prevent overfitting
5. **Different algorithms**: Try logistic regression, SVM, or neural networks

## References

- Dataset: UCI Machine Learning Repository (Tic-Tac-Toe Endgame)
- Algorithm: Gradient Descent with MSE loss
- Integration: Similar to Naive Bayes model structure
