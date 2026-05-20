#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include "String.h"
#include "util.h"

#define HSIZE 128

#define PRIME32_1 2654435761U
#define PRIME32_2 2246822519U
#define PRIME32_3 3266489917U
#define PRIME32_4  668265263U
#define PRIME32_5  374761393U
#define ROTL32(x, r) (((x) << (r)) | ((x) >> (32 - (r))))

typedef struct HNode {
    uint32_t full_hash;
    String key;
    String value;
    struct HNode *next;
} HNode;

typedef struct {
    HNode **buckets;
    size_t count;
} Config;

uint32_t xxhash32(const void* input, size_t length, uint32_t seed) {
    const uint8_t* p = (const uint8_t*)input;
    const uint8_t* const bEnd = p + length;
    uint32_t h32;

    if (length >= 16) {
        const uint8_t* const limit = bEnd - 16;
        uint32_t v1 = seed + PRIME32_1 + PRIME32_2;
        uint32_t v2 = seed + PRIME32_2;
        uint32_t v3 = seed + 0;
        uint32_t v4 = seed - PRIME32_1;

        do {
            uint32_t p1, p2, p3, p4;
            p1 = p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24);
            p2 = p[4] | (p[5] << 8) | (p[6] << 16) | (p[7] << 24);
            p3 = p[8] | (p[9] << 8) | (p[10] << 16) | (p[11] << 24);
            p4 = p[12] | (p[13] << 8) | (p[14] << 16) | (p[15] << 24);

            v1 = ROTL32(v1 + p1 * PRIME32_2, 13) * PRIME32_1;
            v2 = ROTL32(v2 + p2 * PRIME32_2, 13) * PRIME32_1;
            v3 = ROTL32(v3 + p3 * PRIME32_2, 13) * PRIME32_1;
            v4 = ROTL32(v4 + p4 * PRIME32_2, 13) * PRIME32_1;

            p += 16;
        } while (p <= limit);

        h32 = ROTL32(v1, 1) + ROTL32(v2, 7) + ROTL32(v3, 12) + ROTL32(v4, 18);
    } else {
        h32 = seed + PRIME32_5;
    }

    h32 += (uint32_t)length;

    while (p + 4 <= bEnd) {
        uint32_t val = p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24);
        h32 += val * PRIME32_3;
        h32 = ROTL32(h32, 17) * PRIME32_4;
        p += 4;
    }

    while (p < bEnd) {
        h32 += (*p) * PRIME32_5;
        h32 = ROTL32(h32, 11) * PRIME32_1;
        p++;
    }

    h32 ^= h32 >> 15;
    h32 *= PRIME32_2;
    h32 ^= h32 >> 13;
    h32 *= PRIME32_3;
    h32 ^= h32 >> 16;

    return h32;
}

void config_set(Config *cfg, const char *key_str, const char *value_str) {
    if (cfg == NULL || key_str == NULL || value_str == NULL) return;

    uint32_t hash_val = xxhash32(key_str, strlen(key_str), 0);
    size_t idx = hash_val % HSIZE;

    HNode *current = cfg->buckets[idx];
    while (current != NULL) {
        if (current->full_hash == hash_val && strcmp(current->key.data, key_str) == 0) {
            string_clear(&current->value);
            string_init(&current->value, value_str); 
            return;
        }
        current = current->next;
    }

    HNode *new_node = s_malloc(sizeof(HNode));
    new_node->full_hash = hash_val;
    
    string_init(&new_node->key, key_str);
    string_init(&new_node->value, value_str);
    
    new_node->next = cfg->buckets[idx];
    cfg->buckets[idx] = new_node;

    cfg->count++;
}

const char* config_get(const Config *cfg, const char *key_str) {
    if (cfg == NULL || key_str == NULL) return NULL;

    uint32_t hash_val = xxhash32(key_str, strlen(key_str), 0);
    size_t idx = hash_val % HSIZE;

    HNode *current = cfg->buckets[idx];
    while (current != NULL) {
        if (current->full_hash == hash_val && strcmp(current->key.data, key_str) == 0) {
            return current->value.data;
        }
        current = current->next;
    }

    return NULL;
}

int config_load(Config *cfg, const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) return errno; 

    String tpm_key;
    String tpm_value;
    string_init(&tpm_key, NULL);
    string_init(&tpm_value, NULL);

    int ch;
    int is_parsing_value = 0;

    while ((ch = fgetc(fp)) != EOF) {
        if ((ch == '#' || ch == ';') && is_parsing_value == 0) {
            while ((ch = fgetc(fp)) != '\n' && ch != EOF);
        }

        if (ch == '\n' || ch == '\r') {
            if (tpm_key.length > 0) {
                config_set(cfg, tpm_key.data, tpm_value.data);
            }
            string_clear(&tpm_key);
            string_clear(&tpm_value);
            is_parsing_value = 0;
            continue;
        }

        if (ch == '=') {
            if (is_parsing_value == 1) {
                string_add_c(&tpm_value, ch);
            } else {
                is_parsing_value = 1;
            }
            continue;
        }

        if (ch == ' ' || ch == '\t') {
            if (is_parsing_value == 0) {
                continue;
            }
            if (is_parsing_value == 1 && tpm_value.length == 0) {
                continue;
            }
        }

        if (is_parsing_value == 0) {
            string_add_c(&tpm_key, ch);
        } else {
            string_add_c(&tpm_value, ch);
        }
    }

    if (tpm_key.length > 0) {
        config_set(cfg, tpm_key.data, tpm_value.data);
    }

    string_free(&tpm_key);
    string_free(&tpm_value);
    fclose(fp);

    return 0;
}

void config_init(Config *cfg) {
    cfg->buckets = s_calloc(HSIZE, sizeof(HNode*));
    cfg->count = 0;
}

void config_free(Config *cfg) {
    if (cfg->buckets == NULL) return;

    for (size_t i = 0; i < HSIZE; i++) {
        HNode *current = cfg->buckets[i];
        
        while (current != NULL) {
            HNode *to_free = current;
            current = current->next;
            
            string_free(&to_free->key);
            string_free(&to_free->value);
            
            s_free(to_free);
        }
        cfg->buckets[i] = NULL;
    }
    
    s_free(cfg->buckets);
    cfg->count = 0;
}