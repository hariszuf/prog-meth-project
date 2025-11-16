# Q-Learning Training & Deployment System

Complete system for training Q-Learning models and deploying them as frozen (read-only) models in your game.

---

## 🎯 Quick Start

### Train a New Model

```batch
cd "src/q-learning training"
train.bat
```

**Choose your approach:**
1. **From Scratch** - Pure RL (0 → ~3K entries over 50K episodes)
2. **Dataset-Init** - Start with knowledge (~16K entries, refines quickly)  
3. **Resume** - Continue training your existing model

---

## 📁 Your Current Models

| Model | Size | Entries | Type | Status |
|-------|------|---------|------|--------|
| `q_learning_from_scratch.txt` | 4 KB | 120 | Pure RL | ✅ Production-ready |
| `q_learning_non_terminal.txt` | 527 KB | 16,167 | Dataset-init | ✅ Production-ready |
| `q_learning_o_dataset_continuous.txt` | 72 KB | 2,182 | **Your new model** | ✅ Ready to use |

---

## 🚀 Training Approaches

### 1. From Scratch (Pure Reinforcement Learning)

**When to use:**
- Learning how Q-Learning works from first principles
- Want minimal model size
- Educational purposes

**How it works:**
```
Empty Q-table (Q=0.0)
  ↓
Play vs Minimax depth-4
  ↓
Win/Loss/Draw updates Q-values
  ↓
Discovers critical positions
  ↓
Result: ~3,000 entries (compact, strategic)
```

**Command:**
```batch
train.bat → Choice 1 → Enter 50000 episodes
```

**Characteristics:**
- Start: 0 entries
- After 10K: ~500 entries
- After 50K: ~3,000 entries
- Training time: 1-2 hours
- Performance: 100% draws vs Minimax, 50-60% wins vs random

---

### 2. Dataset-Initialized (Recommended for Production)

**When to use:**
- Need comprehensive coverage
- Want best performance quickly
- Production deployment
- Match current game behavior

**How it works:**
```
Load dataset (4,520 positions)
  ↓
Evaluate each move with Minimax
  ↓
Initialize ~16,000 Q-values
  ↓
Refine through continuous play
  ↓
Result: ~16,000+ entries (comprehensive)
```

**Command:**
```batch
train.bat → Choice 2 → Enter 50000 episodes
```

**Characteristics:**
- Start: ~16,000 entries (from dataset)
- After 10K: ~16,500 entries
- After 50K: ~17,500 entries
- Training time: 30-60 minutes (already has knowledge)
- Performance: 100% draws vs Minimax, 70-80% wins vs random

---

### 3. Resume Training (Continue Improving)

**When to use:**
- Already have a trained model
- Want to refine further
- Discovered new edge cases
- Continuous improvement

**How it works:**
```
Load existing model checkpoint
  ↓
Continue playing games
  ↓
Refine Q-values
  ↓
Discover new positions
  ↓
Result: Improved model
```

**Command:**
```batch
train.bat → Choice 3 → Select your model → Enter episodes
```

**Your models available for resuming:**
1. `q_learning_from_scratch.txt` (120 entries)
2. `q_learning_non_terminal.txt` (16,167 entries)  
3. `q_learning_o_dataset_continuous.txt` (2,182 entries) ⭐

---

## 🎮 Deploying Models in Your Game

### Step 1: Choose Your Model

Based on requirements:
- **Compact & Fast**: `q_learning_from_scratch.txt` (4 KB)
- **Comprehensive**: `q_learning_non_terminal.txt` (527 KB)
- **Your Latest**: `q_learning_o_dataset_continuous.txt` (72 KB)

### Step 2: Use Frozen Model Loader

```c
#include "frozen_q_model.h"

// Load model (read-only)
FrozenQModel *ai = frozen_q_load("../../models/q learning/q_learning_non_terminal.txt");

// Get best move
char board[9] = {'x', 'b', 'o', ...};
int move = frozen_q_get_best_action(ai, board);

// Cleanup when game exits
frozen_q_free(ai);
```

**Benefits of frozen models:**
✅ Read-only (can't be modified during gameplay)  
✅ Consistent behavior for all users  
✅ Fast inference  
✅ Works with any trained model format  

### Step 3: Test Integration

```batch
compile_example.bat
```

This tests the frozen model loader with a sample board state.

---

## 📊 Model Comparison

| Aspect | From-Scratch | Dataset-Init | Your New Model |
|--------|-------------|--------------|----------------|
| **Size** | 4 KB | 527 KB | 72 KB |
| **Entries** | 120 | 16,167 | 2,182 |
| **Training** | 10K episodes | Pre-init + training | Partial training |
| **Coverage** | Critical only | Comprehensive | Moderate |
| **Speed** | ⚡⚡⚡ Very fast | ⚡ Fast | ⚡⚡ Very fast |
| **vs Minimax** | 100% draws | 100% draws | 100% draws* |
| **vs Random** | ~50% wins | ~79% wins | ~60% wins* |

*Estimated - run evaluation to confirm

---

## 🔧 Unified Trainer Features

The `unified_q_trainer.c` handles all training modes:

**Modes:**
- `scratch` - From zero knowledge
- `dataset` - Initialize from dataset
- `resume` - Continue from checkpoint

**Features:**
- ✅ Auto-detects model format
- ✅ Checkpoints every 1,000 episodes
- ✅ Graceful Ctrl+C handling
- ✅ Adaptive epsilon (lower for resume)
- ✅ Compatible with all your existing models

**Manual usage:**
```batch
# From scratch
unified_q_trainer.exe scratch output.txt 50000

# Dataset init
unified_q_trainer.exe dataset ../../dataset/tic-tac-toe.data output.txt 50000

# Resume
unified_q_trainer.exe resume existing_model.txt 10000
```

---

## 🔄 Typical Workflow

### Development Cycle

**Phase 1: Initial Training**
```batch
train.bat → Dataset-Init → 50,000 episodes
```
Result: Production-ready model in ~1 hour

**Phase 2: Evaluation**
```batch
cd ../../evaluation
evaluate_all_models.exe
```
Compare performance metrics

**Phase 3: Deployment**
```batch
# Copy to game directory
copy "models\q learning\q_learning_dataset.txt" "TTTGUI\ai_model.txt"

# Update game code to use frozen_q_model.h
# Compile game
# Test thoroughly
```

**Phase 4: Continuous Improvement (Optional)**
```batch
train.bat → Resume → Your model → 10,000 more episodes
```
Periodically refine model based on user feedback

---

## 📈 Training Progress

### From-Scratch Growth
```
Episode 0:     0 entries | Random play
Episode 1K:    ~50 entries | Learning basics  
Episode 5K:    ~300 entries | Strategic play
Episode 10K:   ~500 entries | Strong defense
Episode 50K:   ~3,000 entries | Near-optimal
```

### Dataset-Init Growth  
```
Episode 0:     16,000 entries | Strong initial knowledge
Episode 1K:    16,100 entries | Quick refinement
Episode 5K:    16,300 entries | Optimized values
Episode 10K:   16,500 entries | Production-ready
Episode 50K:   17,500 entries | Near-perfect
```

### Resume Growth
```
Depends on starting model size
Typically grows slowly (refining existing knowledge)
Focus on edge cases and rare positions
```

---

## 🛡️ Why Freeze Models?

**Training Mode (Development):**
- Q-values update constantly
- Explores random moves
- Behavior changes over time
- Good for learning

**Frozen Mode (Production):**
- Q-values locked (read-only)
- No exploration (pure exploitation)
- Consistent behavior
- Good for users

**Separation of concerns:**
- Train offline → improve models
- Deploy frozen → stable gameplay
- Users get consistent AI
- You keep improving behind the scenes

---

## 💡 Tips & Best Practices

### For Training
1. **Start with dataset-init** for production models
2. **Train 50K episodes** minimum for good coverage
3. **Use checkpoints** - training can be interrupted
4. **Monitor stats** - watch draw rate vs Minimax (should be ~100%)
5. **Version your models** - keep date/episode count in filename

### For Deployment
1. **Test thoroughly** before releasing
2. **Keep backups** of working models
3. **Use frozen loader** for read-only access
4. **Benchmark performance** (inference speed)
5. **Compare models** side-by-side in evaluation

### For Your New Model (2,182 entries)
- ✅ Small and fast (72 KB)
- ⚠️ Moderate coverage (2K vs 16K)
- 💡 Consider: Resume training for 10-20K more episodes
- 💡 Or: Use as-is if performance is acceptable

---

## 🧪 Testing Models

### Quick Test
```batch
compile_example.bat
```

### Full Evaluation
```batch
cd ../../evaluation
evaluate_all_models.exe
```

Shows:
- Win/draw/loss rates vs different opponents
- Quality scores
- Comparison across all models

---

## 🔗 Files Reference

### Training
- `unified_q_trainer.c` - Main training program
- `train.bat` - Interactive launcher
- `check_progress.bat` - View model stats

### Deployment
- `frozen_q_model.h` - Header file
- `frozen_q_model.c` - Implementation
- `example_frozen_usage.c` - Integration example
- `compile_example.bat` - Test frozen loader

### Legacy (Still Work!)
- `q_learning_continuous.c` - Pure self-play
- `q_learning_continuous_dataset.c` - Dataset-init version
- `start_training.bat` - Old launcher
- `start_training_dataset.bat` - Old dataset launcher

---

## 🆘 Troubleshooting

### Model won't load
- Check file path is correct
- Verify file format (CSV with board,action,q_value)
- Ensure model isn't corrupted

### Training too slow
- Reduce episodes for testing
- Use from-scratch for faster iteration
- Dataset-init takes longer to initialize but trains faster

### Model performs poorly
- Train more episodes (try 50K+)
- Check if using correct epsilon values
- Evaluate vs Minimax (should get 80%+ draws)

### Resume doesn't work
- Make sure checkpoint file exists
- Check file path matches
- Verify model format is compatible

---

## 📝 Next Steps

### To use your new 2,182-entry model:
1. Test it: `cd ../../evaluation && evaluate_all_models.exe`
2. If good: Deploy with frozen loader
3. If needs work: Resume training for more episodes

### To train a production model:
1. Run: `train.bat → Dataset-Init → 50000`
2. Wait ~1 hour
3. Evaluate performance
4. Deploy if satisfactory

### To experiment:
1. Try from-scratch mode
2. Compare all three approaches
3. See which fits your needs

---

**Happy Training! 🎮🤖**

*The unified system supports all your models - train, resume, and deploy with ease!*
