#ifndef UMKA_COSMO_H
#define UMKA_COSMO_H

#include <ctype.h>
#include <libc/stdbool.h>
#include <stdio.h>

#ifdef __COSMOPOLITAN__
#define _GNU_SOURCE

// Helper wrapper around standard character classification to avoid
// __ctype_b_loc
static int fsscanf(bool string, void *stream, const char *format, void *arg,
                   int *len) {
  if (string) {
    // When scanning a string, use sscanf on a pointer offset by len
    char *str = (char *)stream + *len;
    int result = sscanf(str, format, arg);
    // Advance len by counting characters up to the next whitespace
    if (result > 0) {
      while (str[*len] && !isspace(str[*len])) {
        (*len)++;
      }
      // Skip trailing whitespace
      while (str[*len] && isspace(str[*len])) {
        (*len)++;
      }
    }
    return result;
  } else {
    // For files, use regular fscanf
    return fscanf((FILE *)stream, format, arg);
  }
}

// Simple cosmocc-compatible scanning without varargs
static inline int scanf_wrapper(bool string, void *stream, const char *format,
                                void *arg) {
  return string ? sscanf((char *)stream, format, arg)
                : fscanf((FILE *)stream, format, arg);
}

// #define isspace(c) is_space(c)
// #define vsscanf(str, fmt, args) sscanf(str, fmt, va_arg(args, void *))

#endif

#endif /* UMKA_COSMO_H */
