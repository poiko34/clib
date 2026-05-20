#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

typedef struct {
    char *data;
    size_t length;
    size_t capacity;
} String;

void string_init(String *str, const char *data) {
    size_t _length = 0;
    if(data != NULL) {
        _length = strlen(data);
    }
    str->capacity = _length + 1; 
    str->length = _length;
    
    str->data = s_malloc(str->capacity);

    if (data != NULL) {
        strcpy(str->data, data);
    } else {
        str->data[0] = '\0';
    }
}

void string_free(String *str) {
    if (str->data) {
        s_free(str->data);
        str->data = NULL;
    }
    str->length = 0;
    str->capacity = 0;
}

void string_append(String *str, const char *suffix) {
    size_t suffix_len = strlen(suffix);
    size_t total_len = str->length + suffix_len;

    if (total_len + 1 > str->capacity) {
        size_t new_cap = (total_len + 1) * 2;
        char *new_ptr = s_realloc(str->data, new_cap);
        str->data = new_ptr;
        str->capacity = new_cap;
    }

    memcpy(&str->data[str->length], suffix, suffix_len + 1);
    str->length = total_len;
}

void string_add_c(String *str, char ch) {
    if(str->length + 2 > str->capacity) {
        char* tpm_data = s_realloc(str->data, str->capacity * 2);
        str->data = tpm_data;
        str->capacity *= 2;
    }
    str->data[str->length] = ch;
    str->data[str->length + 1] = '\0';
    str->length++;
}

void string_clear(String *str) {
    str->data[0] = '\0';
    str->length = 0;
}

void string_full_clear(String *str) {
    if(str->length == 0) return;
    memset(str->data, 0, str->length);
    str->length = 0;
}

char string_at(const String *str, size_t index) {
    if(index < str->length) {
        return str->data[index];
    }
    return '\0';
}

int string_reserve(String *str, size_t new_cap) {
    if(new_cap == 0) {
        s_free(str->data);
        str->length = 0;
        str->capacity = 0;
        return 0;
    }
    if(new_cap > str->capacity) {
        char* tpmdata = s_realloc(str->data, new_cap);
        str->data = tpmdata;
        str->capacity = new_cap;
    }
    return 0;
}

int string_cmp(const String *s1, const String *s2) {
    if(s1->length != s2->length) return (s1->length > s2->length) ? 1 : -1;
    return strcmp(s1->data, s2->data);
}

void string_copy(String *dest, const String *src) {
    if(dest->capacity < src->capacity) {
        char *tpmdata = s_realloc(dest->data, src->capacity);
        dest->data = tpmdata;
        dest->capacity = src->capacity;
    }
    strcpy(dest->data, src->data);
    dest->length = src->length;
}

int string_find(const String *str, const char *target) {
    char *p = strstr(str->data, target);
    if (p) return p - str->data;
    return -1;
}

void string_insert(String *str, size_t index, const char *insdata) {
    if(index > str->length) return;
    size_t ins_length = strlen(insdata);
    if(str->length + ins_length > str->capacity) {
        char *tpmdata = s_realloc(str->data, str->length + ins_length + 1);
        str->data = tpmdata;
        str->capacity = str->length + ins_length + 1;
    }
    memmove(&str->data[index + ins_length], &str->data[index], str->length - index + 1);
    memcpy(&str->data[index], insdata, ins_length);
    str->length += ins_length;
}

void string_erase(String *str, size_t index, size_t count) {
    if (index >= str->length) return;
    if (index + count > str->length) count = str->length - index;

    memmove(&str->data[index], &str->data[index + count], str->length - (index + count) + 1);
    
    str->length -= count;
}

void string_replace(String *str, size_t index, char ch) {
    if(index >= str->length) return;
    str->data[index] = ch;
}

int string_shrink_to_fit(String *str) {
    if(str->length+1 == str->capacity) return 1;
    char *tpmdata = s_realloc(str->data, str->length + 1);
    str->data = tpmdata;
    str->capacity = str->length + 1;
    return 0;
}