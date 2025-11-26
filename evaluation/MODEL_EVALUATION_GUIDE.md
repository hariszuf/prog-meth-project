# Model Evaluation with Confusion Matrices

## Overview

This guide shows how to evaluate your trained machine learning models using **confusion matrices** - a fundamental tool for assessing binary classifier performance.

---

## What is a Confusion Matrix?

A confusion matrix shows the performance of a classification model by comparing predicted vs actual outcomes:

```
                  Predicted
               Win        Lose
      Win   |  TP   |   FN   |  (True Positives, False Negatives)
Actual      |-------|--------|
      Lose  |  FP   |   TN   |  (False Positives, True Negatives)
```

### Key Metrics Calculated:

- **Accuracy**: `(TP + TN) / Total` - Overall correctness
- **Precision**: `TP / (TP + FP)` - How many predicted wins were correct
- **Recall**: `TP / (TP + FN)` - How many actual wins were detected
- **Specificity**: `TN / (TN + FP)` - How many actual losses were detected
- **F1-Score**: `2 × (Precision × Recall) / (Precision + Recall)` - Harmonic mean

---

## Quick Start - Evaluate All Models

### **One Command Evaluation:**

```bash
cd c:\Users\muhdh\Documents\GitHub\prog-meth-project\evaluation
.\evaluate_matrix_models.bat
```

**This will:**
1. ✅ Compile the confusion matrix evaluator
2. ✅ Evaluate Linear Regression on train/test datasets
3. ✅ Evaluate Naive Bayes on train/test datasets
4. ✅ Evaluate Q-Learning on train/test datasets
5. ✅ Generate detailed reports for each model
6. ✅ Create comparison summary

**Output Files:**
- `linear_regression_evaluation.txt` - Full confusion matrix & metrics
- `naive_bayes_evaluation.txt` - Full confusion matrix & metrics
- `q_learning_evaluation.txt` - Full confusion matrix & metrics

---

## Manual Evaluation (Single Model)

### **Compile the Evaluator:**

```bash
cd evaluation
gcc confusion-matrix.c -o confusion-matrix.exe -lm
```

### **Evaluate Linear Regression:**

```bash
confusion-matrix.exe ^
    ..\models\linear_regression_non_terminal\model_non_terminal_matrix.txt ^
    ..\dataset\new processed\train_non_terminal_matrix.data ^
    ..\dataset\new processed\test_non_terminal_matrix.data
```

### **Evaluate Naive Bayes:**

```bash
confusion-matrix.exe ^
    ..\models\naive_bayes_non_terminal\model_non_terminal_matrix.txt ^
    ..\dataset\new processed\train_non_terminal_matrix.data ^
    ..\dataset\new processed\test_non_terminal_matrix.data
```

### **Evaluate Q-Learning:**

```bash
confusion-matrix.exe ^
    "..\models\q learning\q_learning_non_terminal_matrix.txt" ^
    ..\dataset\new processed\train_non_terminal_matrix.data ^
    ..\dataset\new processed\test_non_terminal_matrix.data
```

---

## Understanding the Output

### **Example Output:**

```
========================================
 Linear Regression - Training Dataset
========================================

Confusion Matrix:
                  Predicted
               Win        Lose
      Win   |   550  |    59  |
Actual      |--------+---------|
      Lose  |    71  |   538  |

Performance Metrics:
  Accuracy:     89.32% (1088/1218 correct)
  Precision:    88.56% (Win predictions)
  Recall:       90.31% (Win detection)
  Specificity:  88.34% (Lose detection)
  F1-Score:     0.8943

========================================
 Linear Regression - Testing Dataset
========================================

Confusion Matrix:
                  Predicted
               Win        Lose
      Win   |   135  |    17  |
Actual      |--------+---------|
      Lose  |    22  |   131  |

Performance Metrics:
  Accuracy:     87.21% (266/305 correct)
  Precision:    85.99% (Win predictions)
  Recall:       88.82% (Win detection)
  Specificity:  85.62% (Lose detection)
  F1-Score:     0.8738

========================================
 TRAINING vs TESTING COMPARISON
========================================
                Training    Testing    Difference
Accuracy:       89.32%      87.21%     -2.11%
Precision:      88.56%      85.99%     -2.57%
Recall:         90.31%      88.82%     -1.49%
F1-Score:       0.8943      0.8738     -0.0205

✓ Good generalization: Training and testing performance are balanced.
```

---

## Interpretation Guide

### **1. Confusion Matrix Analysis:**

- **True Positives (TP)**: Model correctly predicted winning positions
  - High TP = Good at recognizing winning scenarios
  
- **True Negatives (TN)**: Model correctly predicted losing positions
  - High TN = Good at recognizing losing scenarios
  
- **False Positives (FP)**: Model predicted win, but actually lose
  - High FP = Overconfident, predicts wins too often
  
- **False Negatives (FN)**: Model predicted lose, but actually win
  - High FN = Too conservative, misses winning opportunities

### **2. Metric Interpretation:**

| Metric | Good Range | What It Means |
|--------|-----------|---------------|
| **Accuracy** | > 85% | Overall correctness |
| **Precision** | > 80% | Reliability of win predictions |
| **Recall** | > 80% | Ability to find winning positions |
| **Specificity** | > 80% | Ability to identify losing positions |
| **F1-Score** | > 0.85 | Balanced precision & recall |

### **3. Overfitting Detection:**

```
Training Accuracy: 95.00%
Testing Accuracy:  72.00%
Difference:        -23.00%  ⚠ OVERFITTING!
```

**Signs of Overfitting:**
- Training accuracy >> Testing accuracy (difference > 5%)
- High training performance, poor testing performance
- Model memorized training data instead of learning patterns

**Solutions:**
- Increase training data
- Add regularization
- Reduce model complexity
- Use cross-validation

### **4. Underfitting Detection:**

```
Training Accuracy: 65.00%
Testing Accuracy:  63.00%
```

**Signs of Underfitting:**
- Both training and testing accuracy are low
- Model performs poorly on all datasets
- High bias, unable to capture patterns

**Solutions:**
- Increase model complexity
- Add more features
- Train longer (more epochs)
- Reduce regularization

---

## Expected Performance Ranges

Based on Tic-Tac-Toe non-terminal dataset (1218 train, 305 test):

| Model | Expected Accuracy | Training Time | Strengths |
|-------|------------------|---------------|-----------|
| **Linear Regression** | 70-75% | ~3 seconds | Fast, interpretable weights |
| **Naive Bayes** | 75-80% | ~1 second | Very fast, probabilistic reasoning |
| **Q-Learning** | 85-95% | ~5-10 min | Best performance, learns strategy |

---

## Comparing Models

### **Step 1: Run All Evaluations**

```bash
cd evaluation
.\evaluate_matrix_models.bat
```

### **Step 2: Compare Key Metrics**

Create a comparison table from the output files:

| Model | Train Acc | Test Acc | Precision | Recall | F1-Score |
|-------|-----------|----------|-----------|--------|----------|
| Linear Regression | 89.32% | 87.21% | 85.99% | 88.82% | 0.8738 |
| Naive Bayes | 91.15% | 89.51% | 88.76% | 90.13% | 0.8944 |
| Q-Learning | 95.08% | 93.44% | 93.12% | 93.75% | 0.9343 |

### **Step 3: Identify Best Model**

Consider:
1. **Highest test accuracy** - Best real-world performance
2. **Smallest train-test gap** - Best generalization
3. **Balanced precision/recall** - No bias toward wins/losses
4. **Highest F1-score** - Best overall balance

### **Step 4: Statistical Significance**

If accuracy differences are < 2-3%, models perform similarly.
Use additional metrics (precision, recall, F1) as tiebreakers.

---

## Academic Report Integration

### **Section: Model Evaluation Methodology**

```
All models were evaluated using confusion matrix analysis on an 80/20 
train-test split of the non-terminal Tic-Tac-Toe dataset (N=1523 positions).

Evaluation metrics included:
• Accuracy - Overall classification correctness
• Precision - Reliability of positive predictions
• Recall - Sensitivity to positive cases
• Specificity - Sensitivity to negative cases  
• F1-Score - Harmonic mean of precision and recall

Results demonstrate [Model Name] achieved the highest test accuracy of X%, 
with balanced precision (X%) and recall (X%), indicating robust generalization
without overfitting (train-test accuracy difference: X%).
```

### **Tables for Report:**

**Table 1: Confusion Matrices**
```
                Linear Regression    Naive Bayes      Q-Learning
           Win    Lose         Win    Lose         Win    Lose
Win     | 135  |  17  |    | 138  |  14  |    | 143  |   9  |
Lose    |  22  | 131  |    |  19  | 134  |    |  12  | 141  |
```

**Table 2: Performance Metrics**
```
Model               Accuracy  Precision  Recall  F1-Score
Linear Regression    87.21%     85.99%   88.82%   0.8738
Naive Bayes          89.51%     88.76%   90.13%   0.8944
Q-Learning           93.44%     93.12%   93.75%   0.9343
```

---

## Troubleshooting

### Issue: "Cannot open dataset file"
**Solution:** Ensure datasets were generated using `train_matrix_models.bat`
- Check: `dataset/new processed/train_non_terminal_matrix.data`
- Check: `dataset/new processed/test_non_terminal_matrix.data`

### Issue: "Failed to load model"
**Solution:** Ensure models were trained first
- Run: `cd src` then `.\train_matrix_models.bat`

### Issue: "Cannot determine model type"
**Solution:** Model path must contain `linear_regression`, `naive_bayes`, or `q_learning`

### Issue: Accuracy is 50% (random guessing)
**Solution:** 
- Model file may be corrupted
- Dataset format mismatch (character vs matrix)
- Retrain model using correct dataset processor

### Issue: Compilation errors
**Solution:** Ensure math library flag: `gcc confusion-matrix.c -o confusion-matrix.exe -lm`

---

## Advanced Usage

### **Evaluate on Different Datasets:**

```bash
# Combined dataset (terminal + non-terminal)
confusion-matrix.exe ^
    ..\models\linear_regression_combined\model_combined_matrix.txt ^
    ..\dataset\new processed\train_combined_matrix.data ^
    ..\dataset\new processed\test_combined_matrix.data
```

### **Compare Matrix vs Character Format Models:**

Evaluate same algorithm trained on different data formats:
```bash
# Matrix format
confusion-matrix.exe model_matrix.txt train_matrix.data test_matrix.data

# Character format (requires different evaluator)
# See evaluate_all_models.c for character-based evaluation
```

### **Cross-Validation (Manual):**

Train on different splits and average results:
```bash
# Split 1: 80/20
dataset_processor_matrix.exe dataset.data 0.8
# Train & evaluate

# Split 2: 70/30
dataset_processor_matrix.exe dataset.data 0.7
# Train & evaluate

# Average the accuracy metrics
```

---

## Next Steps

1. ✅ **Train models** - Use `src/train_matrix_models.bat`
2. ✅ **Evaluate models** - Use `evaluation/evaluate_matrix_models.bat`
3. ✅ **Analyze results** - Compare confusion matrices
4. ✅ **Document findings** - Include in academic report
5. ✅ **Test in GUI** - Integrate best model into game

---

## Quick Commands Reference

```bash
# Full evaluation pipeline
cd evaluation
.\evaluate_matrix_models.bat

# Single model evaluation
confusion-matrix.exe <model_path> <train_data> <test_data>

# View saved results
type linear_regression_evaluation.txt
type naive_bayes_evaluation.txt
type q_learning_evaluation.txt
```

---

For complete workflow, see `src/MATRIX_TRAINING_QUICKSTART.md`
