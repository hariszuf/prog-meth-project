# Q-Learning Continuous Training System

This folder contains a complete system for **continuous Q-Learning training** with **frozen deployment models**.

## 🎯 Key Concept

**Training Mode (Development)**
- Models learn continuously through self-play
- Q-values update based on game outcomes
- Checkpoints saved periodically
- Can be stopped/resumed anytime

**Deployment Mode (Production)**
- Models are frozen (read-only)
- No learning during gameplay
- Consistent behavior for users
- Fast inference without updates

---

## 📁 Files Overview

### Training System
- **`q_learning_continuous.c`** - Pure self-play training (from scratch)
- **`q_learning_continuous_dataset.c`** - Dataset-initialized + continuous learning ⭐
- **`start_training.bat`** - Start pure self-play training
- **`start_training_dataset.bat`** - Start dataset-initialized training (RECOMMENDED)
- **`check_progress.bat`** - View model statistics

### Deployment System
- **`frozen_q_model.h`** - Header for read-only model loading
- **`frozen_q_model.c`** - Implementation of frozen models
- **`example_frozen_usage.c`** - Example integration code
- **`compile_example.bat`** - Compile and test frozen model

---

## 🚀 Quick Start

### **RECOMMENDED: Dataset-Initialized Training**

This combines the best of both worlds: strategic knowledge from the dataset + continuous improvement.

```batch
cd "src/q-learning training"
start_training_dataset.bat
```

**What happens:**
1. **First run**: Loads non-terminal dataset (4,520 positions)
2. **Initialization**: Evaluates each move with Minimax → ~16,000 Q-values
3. **Continuous learning**: Plays vs Minimax, refines Q-values
4. **Checkpoint**: Auto-saves every 1,000 episodes
5. **Resume**: Next run continues from saved checkpoint

**Model created:**
- `../../models/q learning/q_learning_o_dataset_continuous.txt`

---

### Alternative: Pure Self-Play Training

Start from zero knowledge (like original from-scratch model):

```batch
cd "src/q-learning training"
start_training.bat
```

**Models created:**
- `../../models/q learning/q_learning_x_continuous.txt` (Player X)
- `../../models/q learning/q_learning_o_continuous.txt` (Player O)

### 2. Check Training Progress

```batch
check_progress.bat
```

Shows:
- Model file sizes
- Number of entries (Q-table size)
- Last modified timestamps

---

## 🧠 Training Approaches Explained

### **Option 1: Dataset-Initialized (RECOMMENDED) ⭐**

**File:** `q_learning_continuous_dataset.c`

**How it works:**
```
Start
  ↓
Load dataset (4,520 non-terminal positions)
  ↓
For each position, evaluate all moves with Minimax
  ↓
Initialize ~16,000 Q-values (strategic knowledge)
  ↓
Play continuous games vs Minimax (depth 4)
  ↓
Update Q-values based on outcomes
  ↓
Save checkpoints every 1,000 episodes
```

**Advantages:**
✅ **Fast start** - Begins with strategic knowledge
✅ **Comprehensive** - Covers all dataset positions
✅ **Continuous improvement** - Learns beyond dataset
✅ **Best performance** - Combines human knowledge + RL
✅ **Matches production** - Same initialization as current game model

**Starting Q-values:** ~16,000 entries (dataset moves)
**After training:** Grows slowly, refines existing values

---

### **Option 2: Pure Self-Play**

**File:** `q_learning_continuous.c`

**How it works:**
```
Start
  ↓
Empty Q-table (all values = 0.0)
  ↓
Both X and O play against each other
  ↓
Learn from wins/losses/draws
  ↓
Discover strategies through trial and error
  ↓
Save both models
```

**Advantages:**
✅ **Pure RL** - No human guidance
✅ **Discovers unique strategies** - May find novel approaches
✅ **Compact** - Only learns critical positions
✅ **Educational** - Shows Q-Learning from scratch

**Starting Q-values:** 0 entries
**After training:** ~3,000 entries (critical positions only)

---

## 📊 Performance Comparison

| Approach | Initial Entries | After 10K Episodes | Win vs Random | Draw vs Minimax |
|----------|----------------|--------------------|--------------|--------------------|
| **Dataset-Init** | ~16,000 | ~16,500 | **79%** ✅ | **100%** ✅ |
| Pure Self-Play | 0 | ~3,000 | 50-60% | 100% ✅ |

**Recommendation:** Use dataset-initialized for production deployment.

### 3. Use Frozen Models in Your Game

```c
#include "frozen_q_model.h"

// Load model (read-only)
FrozenQModel *model = frozen_q_load("path/to/model.txt");

// Get best move
char board[9] = {'x', 'b', 'o', ...};
int move = frozen_q_get_best_action(model, board);

// Get Q-value for specific move
double q = frozen_q_get_q_value(model, board, 5);

// Cleanup
frozen_q_free(model);
```

**See `example_frozen_usage.c` for complete example.**

---

## 🧠 How It Works

### Dataset-Initialized Training Process (RECOMMENDED)

**Phase 1: Initialization (One-time)**
```
Load tic-tac-toe.data
  ↓
For each board state:
  Try every possible O move
  Evaluate with Minimax (depth 4)
  Convert score to Q-value
  ↓
Result: ~16,000 strategic Q-values
```

**Phase 2: Continuous Learning**
```
Episode N:
  X (Minimax depth 4) makes move
    ↓
  O (Q-Learning) chooses action (ε-greedy)
    ↓
  Game continues until end
    ↓
  Update Q-values based on outcome
    ↓
  Refine strategy
    ↓
Episode N+1:
  Play with improved Q-values...
```

**Q-Learning Update Rule:**
```
Q(s,a) ← Q(s,a) + α[r + γ·max Q(s',a') - Q(s,a)]

α = 0.3    (learning rate)
γ = 0.95   (discount factor)
r = reward (+1 win, +0.5 draw, -1 loss)
```

**Exploration vs Exploitation:**
- Starts at ε = 0.2 (20% random - lower than pure self-play since we have knowledge)
- Decays to ε = 0.05 (5% random moves)
- Balances trying new strategies vs using learned knowledge

### Model Growth

**Dataset-Initialized:**
| Training Time | Episodes | Entries | File Size | Notes |
|---------------|----------|---------|-----------|-------|
| 0 (initial)   | 0        | ~16,000 | ~800 KB   | Dataset loaded |
| 10 minutes    | ~10,000  | ~16,500 | ~850 KB   | Refined values |
| 1 hour        | ~50,000  | ~17,500 | ~900 KB   | Discovered new positions |
| 10 hours      | ~500,000 | ~19,000 | ~1 MB     | Near-optimal |

**Pure Self-Play:**
| Training Time | Episodes | Entries | File Size |
|---------------|----------|---------|-----------|
| 1 minute      | ~1,000   | ~500    | ~25 KB    |
| 10 minutes    | ~10,000  | ~3,000  | ~150 KB   |
| 1 hour        | ~50,000  | ~10,000 | ~500 KB   |
| 10 hours      | ~500,000 | ~20,000 | ~1 MB     |

*Dataset-init starts large and grows slowly. Pure self-play starts small and grows faster.*

---

## 📊 Training Parameters

### Hyperparameters

**Dataset-Initialized (`q_learning_continuous_dataset.c`):**
```c
#define ALPHA 0.3              // Learning rate
#define GAMMA 0.95             // Discount factor
#define EPSILON_START 0.2      // Initial exploration (lower - we have knowledge)
#define EPSILON_MIN 0.05       // Minimum exploration
#define EPSILON_DECAY 0.99995  // Exploration decay per episode

#define CHECKPOINT_INTERVAL 1000  // Save every N episodes
#define STATS_INTERVAL 100        // Print stats every N episodes
```

**Pure Self-Play (`q_learning_continuous.c`):**
```c
#define EPSILON_START 0.3      // Higher - need more exploration
// Other parameters same
```

### Rewards

```c
Win:  +1.0
Draw: +0.5
Loss: -1.0
```

---

## 🎮 Integration with Your Game

### Option 1: Direct Integration

Copy `frozen_q_model.c` and `frozen_q_model.h` to your game folder:

```c
// In your game code
#include "frozen_q_model.h"

FrozenQModel *ai_model;

void init_game() {
    ai_model = frozen_q_load("models/q learning/q_learning_o_continuous.txt");
    if (!ai_model) {
        // Fallback to hardcoded AI
    }
}

int get_ai_move(char board[9]) {
    return frozen_q_get_best_action(ai_model, board);
}

void cleanup_game() {
    frozen_q_free(ai_model);
}
```

### Option 2: Replace Existing Q-Learning Code

If you already have Q-Learning code:

1. **Keep your existing structure**
2. **Load from file instead of training**
3. **Disable Q-value updates** during gameplay
4. **Use checkpoint files** from continuous training

---

## 🔄 Training Workflow

### Typical Development Cycle (Dataset-Initialized)

**1. Initial Training (30 minutes - 1 hour)**
```batch
start_training_dataset.bat
→ Select mode 2
→ Enter 50000 episodes
→ Wait for completion
```

**Result:** Well-trained model ready for deployment

**2. Periodic Refinement (optional, overnight)**
```batch
start_training_dataset.bat
→ Select mode 1 (unlimited)
→ Let run overnight
→ Ctrl+C in morning
```

**Result:** Further optimized model

**3. Evaluation**
```batch
cd ../../evaluation
evaluate_all_models.exe
```

**4. Deployment**
- Copy `q_learning_o_dataset_continuous.txt` to deployment location
- Update game to load frozen model
- Test thoroughly before release

### Resume Training

Training automatically resumes from checkpoints:
```batch
start_training_dataset.bat
```

If checkpoint exists, it loads and continues! No dataset re-initialization needed.

---

## 🛡️ Why Freeze Models for Deployment?

### Benefits of Frozen Models

✅ **Consistent Behavior**
- Users get same AI experience every time
- Easier to test and debug
- Fair competitive play

✅ **Performance**
- No Q-table updates during gameplay
- Faster move selection
- Lower memory overhead

✅ **Safety**
- Can't be exploited by users
- No risk of degrading through bad gameplay
- Prevents cheating via model manipulation

✅ **Version Control**
- Specific model versions for releases
- Easy rollback if issues found
- A/B testing between models

### Disadvantages

❌ Can't adapt to individual players
❌ Requires retraining to improve
❌ Static strategy (predictable over time)

---

## 🧪 Testing & Evaluation

### Quick Test

```batch
compile_example.bat
```

Shows:
- Model loads correctly
- Q-values for sample board
- Best move selection

### Full Evaluation

Use the evaluation tools in `../../evaluation/`:
```batch
cd ../../evaluation
compile_and_test_all.bat
```

Compares:
- Win rate vs random
- Draw rate vs minimax
- Move quality metrics

---

## 📈 Advanced: Tuning Training

### Increase Learning Speed
```c
#define ALPHA 0.5              // Higher = faster learning, less stable
#define CHECKPOINT_INTERVAL 500 // Save more frequently
```

### More Exploration
```c
#define EPSILON_START 0.5      // Try more random moves
#define EPSILON_DECAY 0.9999   // Slower decay
```

### Faster Convergence
```c
#define EPSILON_START 0.1      // Less exploration
#define ALPHA 0.2              // Smaller updates
```

---

## 🐛 Troubleshooting

### Training Hangs or Crashes
- Check available memory
- Reduce `Q_TABLE_SIZE` if needed
- Save more frequently (lower `CHECKPOINT_INTERVAL`)

### Model Doesn't Improve
- Increase training episodes (try 100K+)
- Adjust learning rate `ALPHA`
- Check epsilon decay rate

### Model File Too Large
- Normal for comprehensive training
- Expected: 500KB - 1MB after extensive training
- Can prune rarely-visited states manually

### Integration Issues
- Ensure board format matches ('x', 'o', 'b')
- Check array indexing (0-8)
- Verify file path is correct

---

## 📝 Command Line Options

### q_learning_continuous.exe

```batch
q_learning_continuous.exe [max_episodes] [model_x_path] [model_o_path]
```

**Examples:**
```batch
# Unlimited episodes (default paths)
q_learning_continuous.exe

# 10,000 episodes (default paths)
q_learning_continuous.exe 10000

# Custom paths
q_learning_continuous.exe 50000 custom_x.txt custom_o.txt
```

---

## 🎓 Learning More

### Reinforcement Learning Concepts
- **Q-Learning**: Learn optimal policy through trial and error
- **Epsilon-Greedy**: Balance exploration vs exploitation
- **Temporal Difference**: Learn from each state transition
- **Self-Play**: Train against copy of yourself

### Related Files
- `../model training/q_learning.c` - Original dataset-initialized training
- `../../evaluation/` - Model testing and comparison tools
- `../../models/q learning/` - Trained model files

---

## 🚦 Production Checklist

Before deploying to users:

- [ ] Train for at least 50,000 episodes
- [ ] Test vs minimax (should achieve 80%+ draws)
- [ ] Test vs random (should achieve 70%+ wins)
- [ ] Verify frozen model loads correctly
- [ ] Test all edge cases (full board, empty board)
- [ ] Benchmark performance (move selection speed)
- [ ] Create backup of working model
- [ ] Document model version and training date

---

## 💡 Tips & Best Practices

1. **Start with Dataset-Init**: Use `start_training_dataset.bat` for best results
2. **Train 50K Episodes**: Good balance of quality and time (~1 hour)
3. **Monitor Stats**: Watch draw rate vs Minimax (should stay near 100%)
4. **Save Checkpoints**: Keep multiple versions with timestamps
5. **Version Models**: Include date/episode count in filename when copying
6. **Test Often**: Evaluate after major training milestones
7. **Compare Approaches**: Try both pure self-play and dataset-init to see difference

---

## 🔗 Quick Links

- **RECOMMENDED Training**: `start_training_dataset.bat`
- Alternative Training: `start_training.bat`
- Check Progress: `check_progress.bat`
- Example: `compile_example.bat`
- Main Game: `../../TTTGUI/ttt_gui.exe`
- Evaluation: `../../evaluation/`

---

**Happy Training! 🎮🤖**

*Tip: For production, use dataset-initialized training - it combines strategic knowledge with continuous learning!*
