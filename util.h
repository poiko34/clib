#pragma once
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#define s_malloc(size) _secure_malloc(__FILE__, __LINE__, size)
#define s_calloc(n, size) _secure_calloc(__FILE__, __LINE__, n, size)
#define s_realloc(ptr, size) _secure_realloc(__FILE__, __LINE__, ptr, size)
#define s_free(ptr) do { free(ptr); (ptr) = NULL; } while(0)

#define print_info(format, ...) _log_info(format, ##__VA_ARGS__)
#define print_warning(format, ...) _log_warning(format, ##__VA_ARGS__)
#define print_error(format, ...) _log_error(__FILE__, __LINE__, format, ##__VA_ARGS__)

int _log_info(const char *format, ...) {
    va_list args;
    va_start(args, format);

    printf("\e[1;32m[INFO] ");
    vfprintf(stdout, format, args);
    printf("\e[0;37m\n");
    va_end(args);
    return 1;
}

int _log_warning(const char *format, ...) {
    va_list args;
    va_start(args, format);

    printf("\e[1;33m[WARNING] ");
    vfprintf(stdout, format, args);
    printf("\e[0;37m\n");
    va_end(args);
    return 1;
}

int _log_error(const char *file, int line, const char *format, ...) {
    va_list args;
    va_start(args, format);

    fprintf(stderr, "\e[1;31m[ERROR] (%s:%d): ", file, line);
    vfprintf(stderr, format, args);
    fprintf(stderr, "\e[0;37m\n");
    va_end(args);
    return 1;
}

void* _secure_malloc(const char *file, int line, size_t size) {
    if(size == 0) {
        _log_error(file, line, "Unable to malloc zero bytes");
        exit(EXIT_FAILURE);
    }
    void *tmp = malloc(size);
    if(tmp == NULL) {
        _log_error(file, line, "Out of Memory");
        exit(EXIT_FAILURE);
    }
    return tmp;
}

void* _secure_realloc(const char *file, int line, void *ptr, size_t size) {
    if(size == 0) {
        _log_error(file, line, "Unable to realloc zero bytes");
        exit(EXIT_FAILURE);
    }
    void *tmp = realloc(ptr, size);
    if(tmp == NULL) {
        _log_error(file, line, "Out of Memory");
        exit(EXIT_FAILURE);
    }
    return tmp;
}

void* _secure_calloc(const char *file, int line, size_t n, size_t size) {
    if(size == 0) {
        _log_error(file, line, "Unable to malloc zero bytes");
        exit(EXIT_FAILURE);
    }
    void *tmp = calloc(n, size);
    if(tmp == NULL) {
        _log_error(file, line, "Out of Memory");
        exit(EXIT_FAILURE);
    }
    return tmp;
}