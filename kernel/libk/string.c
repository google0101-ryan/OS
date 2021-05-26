#include "string.h"
#include <stddef.h>
#include <stdbool.h>

void* memset(void* dest, int fll, size_t nbytes)
{
    for (int i = 0; i < nbytes; ++i) {
        *((uint8_t*)dest + i) = fll;
    }
    return dest;
}

void* memmove(void* dest, const void* src, size_t nbytes)
{
    if (src > dest) {
        for (int i = 0; i < nbytes; ++i) {
            *((uint8_t*)dest + i) = *((uint8_t*)src + i);
        }
    } else {
        for (int i = nbytes - 1; i >= 0; --i) {
            *((uint8_t*)dest + i) = *((uint8_t*)src + i);
        }
    }
    return dest;
}

void* memcpy(void* dest, const void* src, size_t nbytes)
{
    for (int i = 0; i < nbytes; ++i) {
        *((uint8_t*)dest + i) = *((uint8_t*)src + i);
    }
    return dest;
}

void* memccpy(void* dest, const void* src, int stop, size_t nbytes)
{
    for (int i = 0; i < nbytes; ++i) {
        *((uint8_t*)dest + i) = *((uint8_t*)src + i);
        if (*((uint8_t*)src + i) == stop) {
            return ((uint8_t*)dest + i + 1);
        }
    }
    return NULL;
}

int memcmp(const void* src1, const void* src2, size_t nbytes)
{
    for (int i = 0; i < nbytes; ++i) {
        if (*((uint8_t*)src1 + i) < *((uint8_t*)src2 + i)) {
            return -1;
        }
        if (*((uint8_t*)src1 + i) > *((uint8_t*)src2 + i)) {
            return 1;
        }
    }
    return 0;
}

size_t strlen(const char* s)
{
    size_t i = 0;
    while (s[i] != '\0')
        ++i;
    return i;
}

char* strcpy(char* dest, const char* src)
{
    size_t i;
    for (i = 0; src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    dest[i] = '\0';
    return dest;
}

char* strncpy(char* dest, const char* src, size_t n)
{
    size_t i;
    for (i = 0; i < n && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }

    for (; i < n; i++) {
        dest[i] = '\0';
    }
    return dest;
}

int stoi(void* strv, int len)
{
    char* str = (char*)strv;
    if (len > 9) {
        return 0;
    }
    int res = 0;
    int mult = 1;
    char* end = str + len - 1;
    while (end >= str) {
        res += (*end - '0') * mult;
        mult *= 10;
        end--;
    }
    return res;
}

void htos(uint32_t hex, char str[])
{
    int i = 0;
    if (hex == 0) {
        str[0] = '0';
        i = 1;
    }
    while (hex != 0) {
        int num = hex % 16;
        hex /= 16;
        if (num < 10) {
            str[i++] = num + '0';
        } else {
            str[i++] = num + 'a' - 10;
        }
    }
    str[i] = '\0';
    reverse(str);
}

void dtos(uint32_t dec, char str[])
{
    int i = 0;
    if (dec == 0) {
        str[0] = '0';
        i = 1;
    }
    while (dec != 0) {
        int num = dec % 10;
        dec /= 10;
        str[i++] = num + '0';
    }
    str[i] = '\0';
    reverse(str);
}

void reverse(char s[])
{
    int c, i, j;
    for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

int strcmp(const char* a, const char* b)
{
    while (*a == *b && *a != 0 && *b != 0) {
        a++;
        b++;
    }

    if (a < b) {
        return -1;
    }
    if (a > b) {
        return 1;
    }
    return 0;
}

int strncmp(const char* a, const char* b, uint32_t num)
{
    while (*a == *b && *a != 0 && *b != 0 && num) {
        a++;
        b++;
        num--;
    }

    if (!num) {
        return 0;
    }

    if (a < b) {
        return -1;
    }
    if (a > b) {
        return 1;
    }
    return 0;
}

bool str_validate_len(const char* s, uint32_t len)
{
    for (int i = 0; i < len + 1; i++) {
        if (s[i] == 0) {
            return true;
        }
    }
    return false;
}

/**
 * Ptr array utils
 */

uint32_t ptrarr_len(const char** s)
{
    uint32_t len = 0;
    while (s[len] != 0) {
        ++len;
    }
    return len;
}

bool ptrarr_validate_len(const char** s, uint32_t len)
{
    for (int i = 0; i < len + 1; i++) {
        if (s[i] == 0) {
            return true;
        }
    }
    return false;
}

bool isdigit(char c) {
  return c >= '0' && c <= '9';
}

int atoi (const char * str) {
  int value = 0;
  while(isdigit(*str)) {
    value *= 10;
    value += (*str)-'0';
    str++;
  }

  return value;
}

char * itoa( int value, char * str, int base )
{
    char * rc;
    char * ptr;
    char * low;
    // Check for supported base.
    if ( base < 2 || base > 36 )
    {
        *str = '\0';
        return str;
    }
    rc = ptr = str;
    // Set '-' for negative decimals.
    if ( value < 0 && base == 10 )
    {
        *ptr++ = '-';
    }
    // Remember where the numbers start.
    low = ptr;
    // The actual conversion.
    do
    {
        // Modulo is negative for negative value. This trick makes abs() unnecessary.
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + value % base];
        value /= base;
    } while ( value );
    // Terminating the string.
    *ptr-- = '\0';
    // Invert the numbers.
    while ( low < ptr )
    {
        char tmp = *low;
        *low++ = *ptr;
        *ptr-- = tmp;
    }
    return rc;
}