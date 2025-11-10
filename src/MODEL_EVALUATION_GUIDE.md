# Model Evaluation Guide

## Quick Start

After training your models with `train_models_compare.bat`, evaluate them against perfect Minimax play:

```bash
cd src
.\evaluate_models.bat
```

## What It Does

The evaluation program tests each trained model by having it play **100 games** against perfect Minimax AI:

- **AI Model plays as O** (second player)
- **Minimax plays as X** (first player, perfect play)

## Understanding Results

### Against Perfect Play:
- ✅ **DRAWS = Good** - The AI is playing optimally
- ⚠️ **WINS = Suspicious** - Shouldn't happen against perfect Minimax (but if it does, great!)
- ❌ **LOSSES = Bad** - The AI made suboptimal moves

### Metrics Tracked:
1. **Win Rate** - Games won by AI (should be 0% ideally)
2. **Draw Rate** - Games drawn (should be high, ideally 100%)
3. **Loss Rate** - Games lost (should be 0%)
4. **Illegal Moves** - Invalid moves attempted (should be 0)
5. **Average Time** - Time per game in milliseconds

## Scoring System

Models are ranked using this formula:
- **Draw = 100 points** (optimal play)
- **Win = 50 points** (unexpected but good)
- **Loss = 0 points** (suboptimal)
- **Illegal Move = -10 points** (penalty)

**Higher score = Better model**

## Expected Results

### Ideal Performance:
```
Wins: 0 (0%)
Draws: 100 (100%)
Losses: 0 (0%)
```
This means the AI plays perfectly and never makes mistakes.

### Good Performance:
```
Wins: 0-5 (0-5%)
Draws: 90-100 (90-100%)
Losses: 0-10 (0-10%)
```
Mostly optimal play with occasional suboptimal moves.

### Poor Performance:
```
Losses: >20 (>20%)
```
The model makes frequent mistakes.

## Reading the Report

The generated `model_evaluation_report.txt` contains:

1. **Detailed Results** - Complete stats for each model
2. **Ranking Table** - Models ranked by performance score
3. **Recommendation** - Which model to use in your game

## Models Evaluated

### 1. Naive Bayes (Non-Terminal)
- Trained on strategic positions only
- Good for mid-game decisions

### 2. Naive Bayes (Combined)
- Trained on all game states
- More comprehensive coverage

### 3. Linear Regression (Non-Terminal)
- Numeric evaluation of strategic positions
- Fast predictions

### 4. Linear Regression (Combined)
- Numeric evaluation of all states
- Balanced approach

### 5. Q-Learning (Non-Terminal)
- Reinforcement learning on strategic positions
- Learns optimal policy through trial and error

### 6. Q-Learning (Combined)
- Reinforcement learning on all game states
- Complete state-action value mapping

## Using the Best Model

Once you identify the best model from the report:

1. **Copy the model file** to your game directory:
   ```bash
   copy ..\models\<best_model_folder>\model_*.txt <your_game_directory>
   ```

2. **Update your game code** to load from that model file

3. **Test in actual gameplay** to verify performance

## Troubleshooting

### No models found
- Run `train_models_compare.bat` first to train models
- Check that models exist in `../models/` directories

### Compilation errors
- Ensure gcc is installed and in PATH
- Try: `gcc --version` to verify

### All models perform poorly
- Models may need more training data
- Try regenerating dataset with more examples
- Consider adjusting model hyperparameters

## Advanced: Custom Evaluation

You can modify `evaluate_against_minimax.c` to:
- Change number of games (default: 100)
- Add verbose mode to see individual games
- Test with AI playing as X instead of O
- Compare against different difficulty levels

## Technical Details

### Game Setup:
- **Board representation**: 0-8 for empty cells, 'X' and 'O' for moves
- **Move evaluation**: Each model chooses best move from available positions
- **Win detection**: Checks all 8 possible winning lines

### Minimax Implementation:
- Full depth search (perfect play)
- Alpha-beta pruning not used (small game tree)
- Deterministic - always makes optimal move

## Example Report Output

```
Model: Naive Bayes (Non-Terminal)
  Wins:          0 (0.0%)
  Draws:        95 (95.0%)
  Losses:        5 (5.0%)
  Illegal Moves: 0
  Avg Time:      2.34 ms/game

RECOMMENDATION
Best performing model: Naive Bayes (Non-Terminal)
Draw rate: 95.0%
Loss rate: 5.0%
```

## Next Steps

1. ✅ Run evaluation on all trained models
2. ✅ Review the comparison report
3. ✅ Select the best performing model
4. ✅ Integrate into your game
5. ✅ Test with human players
6. 🔄 Iterate if needed (retrain with more data)

---

**Pro Tip**: A model with 100% draws against Minimax plays perfectly and will provide the best player experience!
