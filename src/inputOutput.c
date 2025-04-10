#include <stdio.h> 
#include <stdlib.h> 
#include <math.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <stdbool.h>
#include <ctype.h>


long getFileSize(const char* filename) {
    struct stat st;
    if (stat(filename, &st) == 0)
        return st.st_size;
    return -1;
}

void toLowerCase(char* str) {
    while (*str) {
        *str = tolower(*str);
        str++;
    }
}
bool cekInputFormat(const char* filename) {
    const char* ext = strrchr(filename, '.');
    if (!ext) return false;

    char lowerExt[10];
    strncpy(lowerExt, ext, sizeof(lowerExt));
    lowerExt[sizeof(lowerExt) - 1] = '\0';
    toLowerCase(lowerExt);

    return strcmp(lowerExt, ".jpg") == 0 ||
           strcmp(lowerExt, ".jpeg") == 0 ||
           strcmp(lowerExt, ".png") == 0;
}

bool cekOutputFormat(const char* str, const char* suffix) {
    if (!str || !suffix) return false;
    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);
    if (lensuffix > lenstr) return false;
    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}