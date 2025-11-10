// model_config.h - Centralized AI Model Configuration
#ifndef MODEL_CONFIG_H
#define MODEL_CONFIG_H

// Available AI model types
typedef enum {
    AI_MODEL_NAIVE_BAYES,
    AI_MODEL_LINEAR_REGRESSION,
    AI_MODEL_Q_LEARNING,
    AI_MODEL_MINIMAX_EASY,      // Imperfect minimax
    AI_MODEL_MINIMAX_HARD       // Perfect minimax
} AIModelType;

// AI configuration structure
typedef struct {
    AIModelType easy_model;     // Level 1
    AIModelType medium_model;   // Level 2
    AIModelType hard_model;     // Level 3
} AIConfig;

// Initialize AI configuration with default settings
void ai_config_init(AIConfig *config);

// Set specific model for a difficulty level
void ai_config_set_level(AIConfig *config, int level, AIModelType model);

// Get the model type for a specific difficulty level
AIModelType ai_config_get_level(const AIConfig *config, int level);

// Get human-readable name for a model type
const char* ai_config_get_model_name(AIModelType model);

// Get human-readable description for a model type
const char* ai_config_get_model_description(AIModelType model);

// Preset configurations for quick switching
void ai_config_preset_default(AIConfig *config);           // Original setup
void ai_config_preset_ml_showcase(AIConfig *config);       // All ML models
void ai_config_preset_minimax_only(AIConfig *config);      // Traditional AI
void ai_config_preset_best_models(AIConfig *config);       // Best performing

#endif // MODEL_CONFIG_H
