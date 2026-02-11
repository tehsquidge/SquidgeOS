#include <stddef.h>
#include "string.h"

void *memset(void *dest, int val, size_t size)
{
    unsigned char *d = dest;
    while (size--)
    {
        *d++ = (unsigned char)val;
    }
    return dest;
}

void *memcpy(void *dest, const void *src, size_t size)
{
    unsigned char *d = dest;
    const unsigned char *s = src;
    while (size--)
    {
        *d++ = *s++;
    }
    return dest;
}

int strcmp(const char *str1, const char *str2)
{
    while (*str1 == *str2)
    {
        if (*str1 == '\0')
        {
            return 0;
        }
        str1++;
        str2++;
    }
    return *str1 - *str2;
}

size_t strlen(const char *str)
{
    size_t c = 0;
    while (*str++ != '\0')
    {
        c++;
    }
    return c;
}