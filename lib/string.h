/* lib/string.h */
#ifndef STRING_H
#define STRING_H
#include "kernel/kernel.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void *memset(void *s, int c, size_t n);

    void *memcpy(void *dst, const void *src, size_t n);
    int memcmp(const void *a, const void *b, size_t n);
    size_t strlen(const char *s);
    char *strcpy(char *dst, const char *src);
    int strcmp(const char *a, const char *b);
    char *strchr(const char *s, int c);

#ifdef __cplusplus
}
#endif

#endif
