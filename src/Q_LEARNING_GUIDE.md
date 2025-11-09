# Q-Learning Training Guide

## Overview

Q-Learning is a **reinforcement learning** algorithm that learns through **trial and error** by playing games against itself. This is fundamentally different from supervised learning (Naive Bayes and Linear Regression).

## Key Differences

| Aspect | Supervised Learning | Q-Learning |
|--------|-------------------|------------|
| **Training Method** | Learn from labeled examples | Learn from experience (self-play) |
| **Dataset Usage** | Direct training on board→outcome | Optional: Bootstrap Q-values |
| **Training Time** | Fast (one pass through data) | Slower (many episodes needed) |
| **Output** | Model weights/probabilities | Q-table (state-action values) |
| **Improvement** | Fixed after training | Can continue learning |

## How Q-Learning Works

### 1. **Q-Table**
- Stores Q(state, action) = expected reward
- Example: Q(board, move_5) = 0.8 means "taking action 5 from this board state leads to winning 80% of the time"

### 2. **Learning Process**
```
For each episode:
  1. Start with empty board
  2. Agent chooses action (epsilon-greedy)
  3. Observe reward (win/lose/draw)
  4. Update Q-values using:
     Q(s,a) ← Q(s,a) + α[r + γ·max(Q(s',a')) - Q(s,a)]
  5. Repeat until game ends
```

### 3. **Hyperparameters**
- **α (Alpha)**: Learning rate (0.1) - how much to update Q-values
- **γ (Gamma)**: Discount factor (0.9) - importance of future rewards
- **ε (Epsilon)**: Exploration rate (0.1) - probability of random move

## Using Your Minimax Dataset

Your dataset can be used for **Q-value initialization** (bootstrapping):

### Option 1: Non-Terminal Dataset (Recommended)
```bash
cd src-haris
.\train_qlearning.bat
# Choose option 1
```

**Why?** Non-terminal states teach strategic thinking:
- `x,o,b,b,x,o,b,b,b,win` → Initialize Q-values high for moves leading to win
- Agent starts with "educated guesses" and refines through self-play

### Option 2: Combined Dataset
```bash
.\train_qlearning.bat
# Choose option 2
```

**Why?** Includes both terminal and non-terminal:
- More comprehensive initialization
- Covers more game states

### Option 3: No Dataset (Pure Self-Play)
```bash
.\train_qlearning.bat
# Choose option 3
```

**Why?** Learn from scratch:
- Takes longer to converge
- Discovers strategies independently
- Good for comparison

## Training Process

### Step-by-Step Commands

```powershell
# Navigate to directory
cd c:\Users\muhdh\Documents\GitHub\tic-tac-toe-ml-training\src-haris

# Option A: Use batch script (easiest)
.\train_qlearning.bat

# Option B: Manual compilation and training
gcc q_learning.c -o q_learning.exe -lm

# Train with non-terminal dataset
.\q_learning.exe tic-tac-toe-minimax-non-terminal.data

# Train with combined dataset
.\q_learning.exe tic-tac-toe-minimax-complete.data

# Train from scratch
.\q_learning.exe
```

### Training Output

```
========================================
TRAINING Q-LEARNING AGENT
========================================
Episodes: 50000
Alpha (learning rate): 0.10
Gamma (discount): 0.90
Epsilon (exploration): 0.10
========================================

Episode 5000: Win: 72.5%, Draw: 18.2%, Loss: 9.3% | Q-entries: 2341
Episode 10000: Win: 78.1%, Draw: 15.4%, Loss: 6.5% | Q-entries: 4523
Episode 15000: Win: 82.3%, Draw: 13.1%, Loss: 4.6% | Q-entries: 6234
...
```

**What to look for:**
- ✅ Win rate increasing over time
- ✅ Loss rate decreasing
- ✅ Q-entries growing (learning more states)

## Performance Comparison

After training all three algorithms, you can compare:

### Win Rates (vs Random Opponent)

| Algorithm | Expected Win Rate | Training Time |
|-----------|------------------|---------------|
| **Naive Bayes** | 70-80% | ~1 second |
| **Linear Regression** | 75-85% | ~5 seconds |
| **Q-Learning** | 85-95% | ~30 seconds |
| **Minimax (optimal)** | 100% | N/A (no training) |

### Advantages of Q-Learning

1. **Learns strategy through experience**
   - Discovers patterns that labeled data might miss
   - Can learn from mistakes

2. **Continues improving**
   - Can train longer for better performance
   - Can adapt to different opponents

3. **No labeled data required**
   - Can learn purely from self-play
   - Your minimax dataset just speeds up learning

### When to Use Q-Learning

✅ **Use Q-Learning when:**
- You want the AI to discover strategies independently
- You have computational resources for training
- You want continuous improvement capability
- You're interested in reinforcement learning

❌ **Don't use Q-Learning when:**
- You need instant training (use supervised learning)
- You have limited computational resources
- You already have perfect labeled data

## Adjusting Training Parameters

### In `q_learning.c`, modify these constants:

```c
#define MAX_EPISODES 50000   // Increase for better learning
#define ALPHA 0.1            // Learning rate (0.01-0.3)
#define GAMMA 0.9            // Discount factor (0.8-0.99)
#define EPSILON 0.1          // Exploration rate (0.05-0.2)
```

**Recommendations:**
- **More episodes** (100,000+) → Better performance but slower
- **Lower alpha** (0.05) → More stable but slower learning
- **Higher gamma** (0.95) → Values long-term rewards more
- **Lower epsilon** (0.05) → Less exploration, more exploitation

## Output Files

After training, you'll get:

### Q-Learning Model
```
q_learning_model.txt
```

Format:
```
# Q-Learning Model
# Format: board_state,action,q_value,visits
x,o,b,b,x,o,b,b,b,6,0.823451,127
x,o,b,b,x,o,b,b,b,8,0.754231,94
...
```

- **board_state**: Current board configuration
- **action**: Move position (0-8)
- **q_value**: Expected reward for taking this action
- **visits**: How many times this state-action was encountered

## Integration with Your Project

### Using Q-Learning in Gameplay

Your Q-learning model can be integrated into the AI vs AI testing:

1. Load Q-table from file
2. For each game state, lookup best action:
   ```c
   int best_action = choose_best_action(qtable, current_board);
   ```
3. Make the move with highest Q-value

### Comparison Testing

Run all three models and compare:

```powershell
# Test Q-learning vs Random
.\q_learning.exe

# Test Q-learning vs Naive Bayes
# (Requires integration in ai_vs_ai_test.c)

# Test Q-learning vs Linear Regression
# (Requires integration in ai_vs_ai_test.c)
```

## Troubleshooting

### Low Win Rate After Training
- **Solution**: Increase MAX_EPISODES (try 100,000)
- **Solution**: Try bootstrapping with minimax dataset

### Training Too Slow
- **Solution**: Reduce MAX_EPISODES
- **Solution**: Increase EPSILON for faster exploration

### Agent Makes Random Moves
- **Solution**: Ensure testing uses epsilon=0 (no exploration)
- **Solution**: Check Q-values are being properly updated

## Summary

**To train Q-Learning:**

1. Generate minimax dataset (optional but recommended):
   ```bash
   .\dataset-gen.exe
   ```

2. Train Q-learning:
   ```bash
   .\train_qlearning.bat
   ```

3. Compare with supervised learning models

**Key Point**: Q-Learning doesn't train "on" the dataset like Naive Bayes/Linear Regression. It uses the dataset to **initialize** Q-values, then learns through **self-play**. This is fundamentally different but often produces better results!
