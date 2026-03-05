#pragma once
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#pragma endregion
char* strsub_dup(char* src, size_t start, size_t end){
    size_t size = end - start + 1;
    char* dest = malloc(size);
    memcpy(dest, src + start, size);
    dest[size - 1] = '\0';
    return dest;
}