/**
 * @file lists.c
 * @brief Type collection and field list management
 * @details Implements dynamic lists for storing types and type fields.
 */

#include <parser/types/lists.h>
#include <parser/types/core.h>
#include <stdlib.h>
#include <string.h>

void type_list_init(type_list* list) {
    list->items = NULL;
    list->count = 0;
    list->capacity = 0;
}

void type_list_free(type_list* list) {
    if (!list) return;
    
    for (size_t i = 0; i < list->count; i++) {
        type_free(list->items[i]);
    }
    free(list->items);
    list->items = NULL;
    list->count = 0;
    list->capacity = 0;
}

void type_list_add(type_list* list, csq_type* type) {
    if (!list || !type) return;
    
    if (list->count >= list->capacity) {
        size_t new_capacity = list->capacity == 0 ? 8 : list->capacity * 2;
        csq_type** new_items = realloc(list->items, new_capacity * sizeof(csq_type*));
        if (!new_items) return;
        
        list->items = new_items;
        list->capacity = new_capacity;
    }
    
    list->items[list->count++] = type;
}

void type_field_list_init(type_field_list* list) {
    list->items = NULL;
    list->count = 0;
    list->capacity = 0;
}

void type_field_list_free(type_field_list* list) {
    if (!list) return;
    
    for (size_t i = 0; i < list->count; i++) {
        free((void*)list->items[i].name);
    }
    free(list->items);
    list->items = NULL;
    list->count = 0;
    list->capacity = 0;
}

void type_field_list_add(type_field_list* list, const char* name, size_t len, csq_type* type) {
    if (!list) return;
    
    if (list->count >= list->capacity) {
        size_t new_capacity = list->capacity == 0 ? 8 : list->capacity * 2;
        type_field* new_items = realloc(list->items, new_capacity * sizeof(type_field));
        if (!new_items) return;
        
        list->items = new_items;
        list->capacity = new_capacity;
    }
    
    char* name_copy = malloc(len + 1);
    if (!name_copy) return;
    memcpy(name_copy, name, len);
    name_copy[len] = '\0';
    
    list->items[list->count].name = name_copy;
    list->items[list->count].name_len = len;
    list->items[list->count].type = type;
    list->count++;
}
