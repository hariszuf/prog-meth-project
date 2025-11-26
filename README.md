# Tic-Tac-Toe for Kids (PM Mini Project 2025)

A lightweight Tic-Tac-Toe game with a friendly GUI, two-player mode, and a computer opponent. You can pick classic minimax or try the included ML bots (Q-Learning, Linear Regression, Naive Bayes). Everything is wired for quick training, swapping models, and showing imperfect difficulty so kids can actually win.

## Highlights
- Raylib GUI: clear board, turn indicator, winner announcement.
- Two-player local play for quick matches.
- Single-player vs AI: minimax plus adjustable difficulty; ML bots for variety.
- Ready-made training scripts and stored model files.
- Runs on Windows with MinGW-w64; code is plain C.

## Repo map
- TTTGUI (Main Game Folder)/: GUI, game loop, prebuilt `ttt_gui.exe`, `game.c/h`, AI hooks.
- dataset/: 958-row tic-tac-toe dataset (features 1/-1/0, labels +1/-1).
- experiment/: batch scripts to train/apply ML models.
- evaluation/: saved metrics and summaries.
- src/: training code
  - model training matrix/: linear regression + Naive Bayes trainers
  - q-learning training/: Q-learning trainer and frozen models
- docs/: `ADDING_AI_MODELS.md`, `MODULAR_AI_QUICKSTART.md`, etc.

## Play the game (Windows)
```
cd "TTTGUI (Main Game Folder)"
ttt_gui.exe
```
- Pick two-player for manual play, or single-player to face the AI. Difficulty options are in the UI.

## Train or refresh ML models
```
cd experiment
./train_matrix_models_simple.bat      # trains linear regression + Naive Bayes on 80/20 split
./implement_matrix_models.bat         # copies trained weights/tables into the GUI models folder
```
- Check results in `evaluation/MATRIX_MODEL_EVALUATION_SUMMARY.txt` (accuracy, precision/recall, confusion matrix).
- Q-learning scripts and frozen weights live in `src/q-learning training/` (`frozen_q_model.*`).

## Tuning for the brief
- Minimax baseline: adjust depth/pruning if you need to save time/memory on low-end hardware.
- Imperfect AI: cap search depth or add light randomness so the computer doesn’t always win; log win rates.
- ML option: wire the trained model into the GUI AI hook and report train/test accuracy + confusion matrix.
- Compare minimax vs ML: note win rate, response time, and memory; recommend when to use each.

## Build notes
- C + GCC (MinGW-w64). Raylib powers the GUI.
- Data encoding: `1` = X, `-1` = O, `0` = empty; labels +1/-1.
- Model files are plain text (weights/tables/Q-values). See `ADDING_AI_MODELS.md` or `MODULAR_AI_QUICKSTART.md` to swap models.
