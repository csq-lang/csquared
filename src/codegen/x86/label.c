/**
 * @file label.c
 * @brief Label generation for assembly code
 * @details Manages creation and storage of unique labels for jumps,
 * loops, and control flow structures.
 */

#include <codegen/x86/label.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define INITIAL_CAPACITY 64

/**
 * @brief Create label generator
 * @return Allocated label generator
 */
label_gen* label_gen_create(void) {
    label_gen* gen = malloc(sizeof(label_gen));
    if (!gen) return NULL;
    
    gen->counter = 0;
    gen->labels = malloc(sizeof(char*) * INITIAL_CAPACITY);
    if (!gen->labels) {
        free(gen);
        return NULL;
    }
    
    gen->count = 0;
    gen->capacity = INITIAL_CAPACITY;
    
    return gen;
}

/**
 * @brief Free label generator
 * @param gen Label generator to deallocate
 */
void label_gen_free(label_gen* gen) {
    if (!gen) return;
    
    for (size_t i = 0; i < gen->count; i++) {
        free(gen->labels[i]);
    }
    free(gen->labels);
    free(gen);
}

static char* store_label(label_gen* gen, char* label) {
    if (gen->count >= gen->capacity) {
        size_t new_capacity = gen->capacity * 2;
        char** new_labels = realloc(gen->labels, sizeof(char*) * new_capacity);
        if (!new_labels) {
            free(label);
            return NULL;
        }
        gen->labels = new_labels;
        gen->capacity = new_capacity;
    }
    
    gen->labels[gen->count++] = label;
    return label;
}

/**
 * @brief Generate new labeled with prefix
 * @param gen Label generator
 * @param prefix Prefix for label name
 * @return Newly allocated label string
 */
char* label_gen_new(label_gen* gen, const char* prefix) {
    if (!gen || !prefix) return NULL;
    
    int len = snprintf(NULL, 0, ".%s_%d", prefix, gen->counter++);
    char* label = malloc(len + 1);
    if (!label) return NULL;
    
    snprintf(label, len + 1, ".%s_%d", prefix, gen->counter - 1);
    return store_label(gen, label);
}

char* label_gen_anon(label_gen* gen) {
    if (!gen) return NULL;
    
    int len = snprintf(NULL, 0, ".L%d", gen->counter++);
    char* label = malloc(len + 1);
    if (!label) return NULL;
    
    snprintf(label, len + 1, ".L%d", gen->counter - 1);
    return store_label(gen, label);
}