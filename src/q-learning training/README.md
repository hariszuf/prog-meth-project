# Q-Learning Training System

Unified Q-Learning training system with frozen deployment models.

## Files

- **`unified_q_trainer.c`** - Main training program (supports from-scratch, dataset-init, resume)
- **`frozen_q_model.h/c`** - Read-only model loader for game deployment

## Training

Compile and run:
```bash
gcc unified_q_trainer.c -o trainer.exe -lm
./trainer.exe
```

Choose training mode:
1. From scratch - Pure RL learning
2. Dataset-init - Start with minimax knowledge
3. Resume - Continue existing training

## Deployment

Load trained models in game using frozen_q_model API:
```c
FrozenQModel *model = frozen_q_load("model.txt");
int move = frozen_q_get_best_action(model, board);
frozen_q_free(model);
```
