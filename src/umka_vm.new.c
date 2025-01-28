#define __USE_MINGW_ANSI_STDIO 1

#ifdef _MSC_VER  // MSVC++ only
    #define FORCE_INLINE __forceinline
#else
    #define FORCE_INLINE __attribute__((always_inline)) inline
#endif

// For cosmocc compatibility
#ifdef __COSMOPOLITAN__
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <math.h>
#include <limits.h>
#include <ctype.h>
#include <inttypes.h>

#include "umka_vm.h"

// Helper wrapper around standard character classification to avoid __ctype_b_loc
static inline int is_space(unsigned char c) {
    return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v';
}

// Helper functions

static FORCE_INLINE int fsgetc(bool string, void *stream, int *len)
{
    int ch = string ? ((char *)stream)[*len] : fgetc((FILE *)stream);
    (*len)++;
    return ch;
}

static int fsnprintf(bool string, void *stream, int size, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int res = string ? vsnprintf((char *)stream, size, format, args) : vfprintf((FILE *)stream, format, args);
    va_end(args);
    return res;
}

// Simple cosmocc-compatible scanning without varargs
static int fsscanf(bool string, void *stream, const char *format, void *arg)
{
    return string ? sscanf((char *)stream, format, arg) : fscanf((FILE *)stream, format, arg);
}

static FORCE_INLINE char *fsscanfString(bool string, void *stream, int *len)
{
    int capacity = 8;
    char *str = malloc(capacity);

    *len = 0;
    int writtenLen = 0;
    int ch = ' ';

    // Skip whitespace
    while (is_space(ch))
        ch = fsgetc(string, stream, len);

    // Read string
    while (ch && ch != EOF && !is_space(ch))
    {
        str[writtenLen++] = ch;
        if (writtenLen == capacity - 1)
        {
            capacity *= 2;
            str = realloc(str, capacity);
        }
        ch = fsgetc(string, stream, len);
    }

    str[writtenLen] = '\0';
    return str;
}