# Dataset Processing and Model Training Guide

## Overview

The updated dataset processor automatically creates appropriately named files based on your input dataset, making it easy to train and compare models on different datasets.

## File Naming Convention

### Input → Output Mapping

| Input Dataset | Training Data | Testing Data | Report | Model Suffix |
|--------------|---------------|--------------|---------|--------------|
| `tic-tac-toe-minimax-non-terminal.data` | `train_non_terminal.data` | `test_non_terminal.data` | `report_non_terminal.txt` | `_non_terminal` |
| `tic-tac-toe-minimax-complete.data` | `train_combined.data` | `test_combined.data` | `report_combined.txt` | `_combined` |
| Any other file | `train.data` | `test.data` | `dataset_report.txt` | (default) |

## Usage

### 1. Process Non-Terminal Dataset

```powershell
cd src-haris

# Process non-terminal dataset
.\dataset_processor.exe tic-tac-toe-minimax-non-terminal.data 0.8

# Creates:
#   - train_non_terminal.data
#   - test_non_terminal.data
#   - report_non_terminal.txt
```

### 2. Process Combined Dataset

```powershell
# Process combined dataset
.\dataset_processor.exe tic-tac-toe-minimax-complete.data 0.8

# Creates:
#   - train_combined.data
#   - test_combined.data
#   - report_combined.txt
```

### 3. Train All Models (Automated)

```powershell
# Train all 3 algorithms on both datasets
.\train_models_compare.bat

# This will:
# 1. Process both datasets
# 2. Train Naive Bayes on both
# 3. Train Linear Regression on both
# 4. Train Q-Learning on both (if compiled)
# 5. Save models with appropriate names
```

## Model Output Structure

After training, you'll have this directory structure:

```
models/
├── naive_bayes_non_terminal/
│   └── model_non_terminal.txt
├── naive_bayes_combined/
│   └── model_combined.txt
├── linear_regression_non_terminal/
│   ├── model_non_terminal.txt
│   └── model_non_terminal.bin
├── linear_regression_combined/
│   ├── model_combined.txt
│   └── model_combined.bin
└── (default locations for active models)
    ├── naive bayes/model.txt
    └── linear regression/model.bin

src-haris/
├── q_learning_non_terminal.txt
├── q_learning_combined.txt
├── train_non_terminal.data
├── test_non_terminal.data
├── train_combined.data
└── test_combined.data
```

## Training Individual Algorithms

### Naive Bayes

```powershell
# For non-terminal
copy train_non_terminal.data train.data
copy test_non_terminal.data test.data
.\naive_bayes.exe

# For combined
copy train_combined.data train.data
copy test_combined.data test.data
.\naive_bayes.exe
```

### Linear Regression

```powershell
# For non-terminal
copy train_non_terminal.data ..\dataset\processed\train.data
copy test_non_terminal.data ..\dataset\processed\test.data
.\linear_regression.exe

# For combined
copy train_combined.data ..\dataset\processed\train.data
copy test_combined.data ..\dataset\processed\test.data
.\linear_regression.exe
```

### Q-Learning

```powershell
# For non-terminal (bootstrapping)
.\q_learning.exe tic-tac-toe-minimax-non-terminal.data

# For combined (bootstrapping)
.\q_learning.exe tic-tac-toe-minimax-complete.data
```

## Using Specific Models in Gameplay

To use a specific trained model:

### Activate Non-Terminal Models

```powershell
# Linear Regression
copy ..\models\linear_regression_non_terminal\model_non_terminal.bin ..\models\linear regression\model.bin

# Naive Bayes
copy ..\models\naive_bayes_non_terminal\model_non_terminal.txt ..\models\naive bayes\model.txt
```

### Activate Combined Models

```powershell
# Linear Regression
copy ..\models\linear_regression_combined\model_combined.bin ..\models\linear regression\model.bin

# Naive Bayes
copy ..\models\naive_bayes_combined\model_combined.txt ..\models\naive bayes\model.txt
```

## Complete Workflow Example

```powershell
# Step 1: Navigate to directory
cd c:\Users\muhdh\Documents\GitHub\tic-tac-toe-ml-training\src-haris

# Step 2: Generate datasets (if not already done)
.\dataset-gen.exe
# Choose option 2 for non-terminal
# Run again and choose option 3 for complete

# Step 3: Train all models on both datasets
.\train_models_compare.bat

# Step 4: Review results
type report_non_terminal.txt
type report_combined.txt

# Step 5: Test specific model
# Copy desired model to active location and test
```

## Comparison Analysis

### Expected Differences

**Non-Terminal Models:**
- Train on strategic positions (games in progress)
- Better at mid-game decision making
- Learn move sequences that lead to wins
- ~3000-4000 training samples

**Combined Models:**
- Train on all positions (strategic + finished games)
- More comprehensive coverage
- Better at recognizing finished states
- ~5000-6000 training samples

### Performance Metrics to Compare

1. **Training Accuracy** - Check output during training
2. **Test Accuracy** - Check output during training
3. **Win Rate** - Test in AI vs AI gameplay
4. **Strategic Quality** - Review actual game moves

## Quick Commands

```powershell
# Process non-terminal only
.\dataset_processor.exe tic-tac-toe-minimax-non-terminal.data

# Process combined only
.\dataset_processor.exe tic-tac-toe-minimax-complete.data

# Train everything
.\train_models_compare.bat

# Check environment
.\check_environment.bat
```

## Troubleshooting

**Problem:** Files not created with correct names
- **Solution:** Ensure input filename contains "non-terminal" or "complete"

**Problem:** Models overwriting each other
- **Solution:** The batch script now saves to separate directories automatically

**Problem:** Can't find trained models
- **Solution:** Check the model directories listed above

## Summary

The system now automatically:
✅ Detects dataset type from filename
✅ Creates appropriately named output files
✅ Saves models to separate directories
✅ Preserves both non-terminal and combined models
✅ Allows easy comparison between training approaches

This makes it simple to train all three algorithms (Naive Bayes, Linear Regression, Q-Learning) on both datasets and compare their performance!
