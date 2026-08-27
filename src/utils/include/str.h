#ifndef STR_H
#define STR_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/**
 * Converts an 8-bit unsigned integer to a decimal string using a static buffer.
 * Note: Non-reentrant (returns pointer to static memory).
 */
static inline const char* Str_U8(uint8_t value) {
    static char buf[4];
    (void)utoa(value, buf, 10);
    return buf;
}

/**
 * Converts a 16-bit unsigned integer to a decimal string using a static buffer.
 */
static inline const char* Str_U16(uint16_t value) {
    static char buf[6];
    (void)utoa(value, buf, 10);
    return buf;
}

/**
 * Safely appends a source string onto a destination buffer, preventing overflow.
 */
static inline void Str_Cat(char *dest, const char *src, size_t dest_max_size) {
    if (dest == NULL || src == NULL || dest_max_size == 0) return;
    
    size_t current_len = strlen(dest);
    if (current_len >= dest_max_size - 1) return;

    strncat(dest, src, dest_max_size - current_len - 1);
}

/**
 * Safely appends an unsigned 16-bit integer to a target buffer string.
 */
static inline void Str_CatNum(char *dest, uint16_t number, size_t dest_max_size) {
    char num_buf[6];
    (void)utoa(number, num_buf, 10);
    Str_Cat(dest, num_buf, dest_max_size);
}

/**
 * Safely appends a Key=Value string pair to a target buffer (e.g., " dest=Tanta").
 */
static inline void Str_CatKV(char *dest, const char *key, const char *val, size_t dest_max_size) {
    Str_Cat(dest, " ", dest_max_size);
    Str_Cat(dest, key, dest_max_size);
    Str_Cat(dest, "=", dest_max_size);
    Str_Cat(dest, val, dest_max_size);
}

/**
 * Safely appends a Key=NumericValue string pair to a target buffer (e.g., " qty=2").
 */
static inline void Str_CatKVNum(char *dest, const char *key, uint16_t val, size_t dest_max_size) {
    Str_Cat(dest, " ", dest_max_size);
    Str_Cat(dest, key, dest_max_size);
    Str_Cat(dest, "=", dest_max_size);
    Str_CatNum(dest, val, dest_max_size);
}

#endif /* STR_H */