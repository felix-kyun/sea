#pragma once

typedef enum ModuleType {
    MODULE_TYPE_TEXT,
} ModuleType;

typedef struct TextModule {
    ModuleType type;
    char* content;
} TextModule;

void text_module_cleanup(TextModule* module);
