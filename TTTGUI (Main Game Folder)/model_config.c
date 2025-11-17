// model_config.c - Centralized AI Model Configuration Implementation
#include "model_config.h"
#include <stdio.h>

// Initialize with default configuration
void ai_config_init(AIConfig *config) {
    ai_config_preset_default(config);
}

// Set specific model for a difficulty level
void ai_config_set_level(AIConfig *config, int level, AIModelType model) {
    switch (level) {
        case 1:
            config->easy_model = model;
            break;
        case 2:
            config->medium_model = model;
            break;
        case 3:
            config->hard_model = model;
            break;
        default:
            printf("Warning: Invalid level %d (must be 1-3)\n", level);
    }
}

// Get the model type for a specific difficulty level
AIModelType ai_config_get_level(const AIConfig *config, int level) {
    switch (level) {
        case 1:
            return config->easy_model;
        case 2:
            return config->medium_model;
        case 3:
            return config->hard_model;
        default:
            return AI_MODEL_MINIMAX_HARD; // Fallback
    }
}

// Get human-readable name for a model type
const char* ai_config_get_model_name(AIModelType model) {
    switch (model) {
        case AI_MODEL_NAIVE_BAYES:
            return "Naive Bayes";
        case AI_MODEL_LINEAR_REGRESSION:
            return "Linear Regression";
        case AI_MODEL_Q_LEARNING:
            return "Q-Learning";
        case AI_MODEL_MINIMAX_EASY:
            return "Imperfect Minimax";
        case AI_MODEL_MINIMAX_HARD:
            return "Perfect Minimax";
        default:
            return "Unknown";
    }
}

// Get human-readable description for a model type
const char* ai_config_get_model_description(AIModelType model) {
    switch (model) {
        case AI_MODEL_NAIVE_BAYES:
            return "Probabilistic ML model";
        case AI_MODEL_LINEAR_REGRESSION:
            return "Fast numeric evaluation";
        case AI_MODEL_Q_LEARNING:
            return "Reinforcement learning";
        case AI_MODEL_MINIMAX_EASY:
            return "Imperfect search (depth 3)";
        case AI_MODEL_MINIMAX_HARD:
            return "Perfect play (full depth)";
        default:
            return "Unknown model";
    }
}

// ===== PRESET CONFIGURATIONS =====

// Original setup: Naive Bayes (Easy), Minimax Medium (Medium), Minimax Hard (Hard)
void ai_config_preset_default(AIConfig *config) {
    config->easy_model = AI_MODEL_NAIVE_BAYES;
    config->medium_model = AI_MODEL_MINIMAX_EASY;
    config->hard_model = AI_MODEL_MINIMAX_HARD;
}

// ML Showcase: Different ML model at each level
void ai_config_preset_ml_showcase(AIConfig *config) {
    config->easy_model = AI_MODEL_NAIVE_BAYES;
    config->medium_model = AI_MODEL_Q_LEARNING;
    config->hard_model = AI_MODEL_LINEAR_REGRESSION;
}

// Traditional AI: All minimax
void ai_config_preset_minimax_only(AIConfig *config) {
    config->easy_model = AI_MODEL_MINIMAX_EASY;
    config->medium_model = AI_MODEL_MINIMAX_EASY;
    config->hard_model = AI_MODEL_MINIMAX_HARD;
}

// Best Models: Imperfect minimax (Easy), Best Q-Learning (Medium), Perfect minimax (Hard)
// This is your requested configuration
void ai_config_preset_best_models(AIConfig *config) {
    config->easy_model = AI_MODEL_MINIMAX_EASY;
    config->medium_model = AI_MODEL_Q_LEARNING;
    config->hard_model = AI_MODEL_MINIMAX_HARD;
}
