#ifndef MODEL_CONFIG_H
#define MODEL_CONFIG_H

typedef enum {
    AI_MODEL_NAIVE_BAYES,
    AI_MODEL_LINEAR_REGRESSION,
    AI_MODEL_Q_LEARNING,
    AI_MODEL_MINIMAX_EASY,
    AI_MODEL_MINIMAX_HARD
} AIModelType;

typedef struct {
    AIModelType easy_model;
    AIModelType medium_model;
    AIModelType hard_model;
} AIConfig;

void ai_config_init(AIConfig *config);
void ai_config_set_level(AIConfig *config, int level, AIModelType model);
AIModelType ai_config_get_level(const AIConfig *config, int level);
const char* ai_config_get_model_name(AIModelType model);
const char* ai_config_get_model_description(AIModelType model);
void ai_config_preset_default(AIConfig *config);
void ai_config_preset_ml_showcase(AIConfig *config);
void ai_config_preset_minimax_only(AIConfig *config);
void ai_config_preset_best_models(AIConfig *config);

#endif // MODEL_CONFIG_H
