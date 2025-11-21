# Matrix Model Training - Quick Start Guide

## Overview

This guide shows you how to train all three ML models using the **standardized matrix format** on the non-terminal dataset.

---

## One-Command Training (Recommended)

### **Option 1: Automated Script (Easiest)**

```bash
cd c:\Users\muhdh\Documents\GitHub\prog-meth-project\src
.\train_matrix_models.bat
```

**This single script will:**
1. ✅ Compile the matrix processor
2. ✅ Process non-terminal dataset to matrix format
3. ✅ Compile all three training programs
4. ✅ Train Linear Regression
5. ✅ Train Naive Bayes
6. ✅ Train Q-Learning (5000 episodes)
7. ✅ Organize all output files properly

**Press Enter at each pause to continue to the next step.**

---

## Manual Step-by-Step (For Understanding)

### **Step 1: Process Dataset**

```bash
cd "c:\Users\muhdh\Documents\GitHub\prog-meth-project\src\data related"

# Compile processor
gcc dataset_processor_matrix.c -o dataset_processor_matrix.exe

# Process non-terminal dataset (80/20 split)
dataset_processor_matrix.exe ..\..\dataset\tic-tac-toe-minimax-non-terminal.data 0.8
```

**Output files:**
- `../../dataset/new processed/train_non_terminal_matrix.data`
- `../../dataset/new processed/test_non_terminal_matrix.data`
- `../../dataset/new results/report_non_terminal_matrix.txt`

---

### **Step 2: Compile Training Programs**

```bash
# Linear Regression
cd "..\model training matrix"
gcc linear_regression_matrix.c -o linear_regression_matrix.exe -lm

# Naive Bayes
gcc naive_bayes_matrix.c -o naive_bayes_matrix.exe -lm

# Q-Learning
cd "..\q-learning training"
gcc unified_q_trainer.c -o unified_q_trainer.exe -lm
```

---

### **Step 3: Train Linear Regression**

```bash
cd "c:\Users\muhdh\Documents\GitHub\prog-meth-project\src\model training matrix"

linear_regression_matrix.exe ^
    ..\..\dataset\new processed\train_non_terminal_matrix.data ^
    ..\..\dataset\new processed\test_non_terminal_matrix.data
```

**Expected output:**
```
Loading matrix format data...
Loaded 1218 samples (training)
Loaded 305 samples (testing)

Training linear regression model (Matrix Format)...
Epochs: 1000, Learning rate: 0.0100

Epoch    1: MSE = 0.985321
Epoch  100: MSE = 0.456789
...
Epoch 1000: MSE = 0.123456

Test Results:
  Accuracy: 72.35% (221/305 correct)
```

**Move model to proper location:**
```bash
move linear_regression_model.txt ..\..\models\linear_regression_non_terminal\model_non_terminal_matrix.txt
move linear_regression_model.bin ..\..\models\linear_regression_non_terminal\model_non_terminal_matrix.bin
```

---

### **Step 4: Train Naive Bayes**

```bash
naive_bayes_matrix.exe ^
    ..\..\dataset\new processed\train_non_terminal_matrix.data ^
    ..\..\dataset\new processed\test_non_terminal_matrix.data
```

**Expected output:**
```
Loading matrix format data...
Loaded 1218 samples (training)
Loaded 305 samples (testing)

Training Naive Bayes model (Matrix Format)...

Class distribution:
  Win (+1):  609 samples (50.00%)
  Lose (-1): 609 samples (50.00%)

Test Results:
  Accuracy: 76.72% (234/305 correct)
```

**Move model to proper location:**
```bash
move naive_bayes_model.txt ..\..\models\naive_bayes_non_terminal\model_non_terminal_matrix.txt
move naive_bayes_model.bin ..\..\models\naive_bayes_non_terminal\model_non_terminal_matrix.bin
```

---

### **Step 5: Train Q-Learning**

```bash
cd "..\q-learning training"

unified_q_trainer.exe dataset ^
    ..\..\dataset\new processed\train_non_terminal_matrix.data ^
    ..\..\models\q learning\q_learning_non_terminal_matrix.txt ^
    5000
```

**Expected output:**
```
========================================
UNIFIED Q-LEARNING TRAINING SYSTEM
========================================

Mode: DATASET-INITIALIZED

Loading dataset: ..\..\dataset\new processed\train_non_terminal_matrix.data
Format detected: MATRIX
Initializing Q-values with Minimax evaluation...
  Processed 500 boards, initialized 2345 moves...
  Processed 1000 boards, initialized 4678 moves...
✓ Dataset initialization complete!
  Boards: 1218 | Moves: 4894 | Q-entries: 4894

Training parameters:
  Initial entries: 4894
  Learning rate (α): 0.30
  Discount (γ): 0.95
  Epsilon: 0.20 → 0.05
  Checkpoint: every 1000 episodes
  Output: ..\..\models\q learning\q_learning_non_terminal_matrix.txt

Press Ctrl+C to stop and save.
Training...

=== Episode 100 ===
Last 100 games: O:45 (45.0%) X:32 (32.0%) Draw:23 (23.0%)
Epsilon: 0.1980 | Q-entries: 5234 | Time: 12s
...
```

**Note:** Q-Learning saves directly to the specified location.

---

## Final Folder Structure

After training, your files will be organized as:

```
prog-meth-project\
├── dataset\
│   ├── tic-tac-toe-minimax-non-terminal.data  (original)
│   ├── new processed\
│   │   ├── train_non_terminal_matrix.data
│   │   └── test_non_terminal_matrix.data
│   └── new results\
│       └── report_non_terminal_matrix.txt
│
├── models\
│   ├── linear_regression_non_terminal\
│   │   ├── model_non_terminal_matrix.txt
│   │   └── model_non_terminal_matrix.bin
│   ├── naive_bayes_non_terminal\
│   │   ├── model_non_terminal_matrix.txt
│   │   └── model_non_terminal_matrix.bin
│   └── q learning\
│       └── q_learning_non_terminal_matrix.txt
│
└── src\
    ├── train_matrix_models.bat  (automated script)
    ├── data related\
    │   ├── dataset_processor_matrix.c
    │   └── dataset_processor_matrix.exe
    ├── model training matrix\
    │   ├── linear_regression_matrix.c
    │   ├── linear_regression_matrix.exe
    │   ├── naive_bayes_matrix.c
    │   └── naive_bayes_matrix.exe
    └── q-learning training\
        ├── unified_q_trainer.c
        └── unified_q_trainer.exe
```

---

## Verification Checklist

After training, verify:

- [ ] All `.exe` files compiled without errors
- [ ] Dataset report generated in `dataset/new results/`
- [ ] Three model files in `models/linear_regression_non_terminal/`
- [ ] Three model files in `models/naive_bayes_non_terminal/`
- [ ] One model file in `models/q learning/`
- [ ] All models show reasonable accuracy (>60%)

---

## Troubleshooting

### Issue: "gcc is not recognized"
**Solution:** Install MinGW or MSYS2 and add to PATH

### Issue: "File not found" error
**Solution:** Ensure you're running from `src/` directory

### Issue: Compilation errors
**Solution:** Make sure to include `-lm` flag for math library

### Issue: Low accuracy (<50%)
**Solution:** Check dataset was processed correctly, verify train/test split

### Issue: Q-Learning takes too long
**Solution:** Reduce episodes (5000 → 1000) or press Ctrl+C to save early

---

## Expected Training Times

| Model | Training Time | Notes |
|-------|--------------|-------|
| Linear Regression | ~2-3 seconds | 1000 epochs |
| Naive Bayes | ~1 second | Very fast |
| Q-Learning | ~5-10 minutes | 5000 episodes with minimax opponent |

*Times on Intel i7, 16GB RAM*

---

## Next Steps

1. **Review accuracy metrics** from training output
2. **Compare models** - which performed best?
3. **Analyze results** in dataset report
4. **Test in gameplay** - integrate with GUI
5. **Document findings** for academic report

---

## Additional Training Options

### Train on Combined Dataset (all positions):

```bash
# Edit train_matrix_models.bat
# Change: tic-tac-toe-minimax-non-terminal.data
# To:     tic-tac-toe-minimax-complete.data
```

### Adjust train/test split:

```bash
# Change 0.8 to desired ratio (e.g., 0.7 for 70/30 split)
dataset_processor_matrix.exe dataset.data 0.7
```

### Train Q-Learning longer:

```bash
# Change 5000 to more episodes
unified_q_trainer.exe dataset input.data output.txt 10000
```

---

## Quick Commands Reference

```bash
# Full automated training
cd src
.\train_matrix_models.bat

# Process dataset only
cd "src\data related"
dataset_processor_matrix.exe ..\..\dataset\tic-tac-toe-minimax-non-terminal.data 0.8

# Train specific model
cd "src\model training matrix"
linear_regression_matrix.exe ..\..\dataset\new processed\train_non_terminal_matrix.data ..\..\dataset\new processed\test_non_terminal_matrix.data

# Resume Q-Learning training
cd "src\q-learning training"
unified_q_trainer.exe resume ..\..\models\q learning\q_learning_non_terminal_matrix.txt 5000
```

---

For detailed information, see `model training matrix/README.md`
