# Training Output Organization

This folder contains all the outputs from the `train_models_compare.bat` script, organized for easy access and comparison.

## 📁 Folder Structure

```
output/
├── combined/              # Models trained on COMBINED dataset (all positions)
│   ├── naive_bayes_model.txt
│   ├── q_learning_combined.txt
│   └── q_learning_model.txt
│
├── non_terminal/          # Models trained on NON-TERMINAL dataset (strategic positions)
│   └── q_learning_non_terminal.txt
│
├── datasets/              # Processed train/test splits
│   ├── train_non_terminal.data    (3,616 samples - strategic positions)
│   ├── test_non_terminal.data     (904 samples)
│   ├── train_combined.data        (varies - all positions)
│   ├── test_combined.data
│   ├── train.data                 (generic/latest)
│   └── test.data
│
└── reports/               # Statistical reports and analysis
    ├── report_non_terminal.txt    (stats for non-terminal dataset)
    ├── report_combined.txt        (stats for combined dataset)
    └── dataset_report.txt         (latest dataset processing report)
```

## 📊 What Each Folder Contains

### `combined/`
Models trained on the **complete dataset** including:
- Terminal states (game endings: X wins, O wins, draws)
- Non-terminal states (games in progress)

**Total samples**: ~5,500 positions

**Use Case**: General-purpose AI that knows both strategic moves and end-game recognition.

### `non_terminal/`
Models trained on **strategic positions only**:
- Only games in progress (no terminal states)
- Focuses on decision-making during gameplay

**Total samples**: ~4,520 positions

**Use Case**: Strategic AI focused on making good moves (may be better at actual gameplay).

### `datasets/`
Processed datasets ready for training:
- Split into 80% training / 20% testing
- **Shuffled randomly** before splitting to ensure good distribution
- Different suffixes indicate which original dataset they came from

### `reports/`
Statistical analysis of datasets:
- Sample counts per outcome (win/lose/draw)
- Distribution percentages
- Dataset characteristics

## 🎯 Comparing Model Performance

### Dataset Comparison
| Dataset Type | Samples | Focus | Expected Strength |
|--------------|---------|-------|-------------------|
| Non-Terminal | 4,520   | Strategic positions | Better gameplay decisions |
| Combined     | 5,500   | All positions | Better overall coverage |

### Key Questions to Answer
1. **Which dataset produces higher accuracy?**
   - Check the training output for test accuracy percentages
   - Review the reports for outcome distribution

2. **Which performs better in actual gameplay?**
   - Use AI vs AI testing
   - Compare win rates against minimax

3. **Does including terminal states help or hurt?**
   - Terminal states are "easy" (obvious outcomes)
   - May dilute learning on strategic positions
   - Or may provide better context for evaluation

## 🚀 Next Steps

### 1. Review Accuracy Metrics
Check the console output from training to see test accuracy for each model.

### 2. Compare Reports
```powershell
# View non-terminal statistics
cat reports\report_non_terminal.txt

# View combined statistics  
cat reports\report_combined.txt
```

### 3. Test Models in Gameplay
Copy the best-performing model to the default location:

**For Q-Learning:**
```powershell
# Test non-terminal model
copy non_terminal\q_learning_non_terminal.txt ..\q_learning_model.txt

# OR test combined model
copy combined\q_learning_combined.txt ..\q_learning_model.txt
```

**For Linear Regression/Naive Bayes:**
Check the `../models/` directory for those model outputs.

### 4. AI vs AI Battles
Run AI vs AI tests to see which model plays better:
```powershell
cd ..
.\run_ai_vs_ai.bat
```

## 📝 Notes

- All datasets were **randomly shuffled** before splitting to ensure representative train/test sets
- Models are independent - you can use different dataset versions for different algorithms
- The "generic" `train.data` and `test.data` are from the most recent processing run

## 🔄 Re-running Training

To regenerate these outputs:
```powershell
cd c:\Users\muhdh\Documents\GitHub\tic-tac-toe-ml-training\src-haris
.\train_models_compare.bat
```

This will process both datasets and train all algorithms again, automatically organizing outputs into these folders.
