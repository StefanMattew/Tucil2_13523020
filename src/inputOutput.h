#ifndef INPUT_OUTPUT_H
#define INPUT_OUTPUT_H

#include <stdbool.h>

long getFileSize(const char* filename);
void toLowerCase(char* str);
bool cekInputFormat(const char* filename);
bool cekOutputFormat(const char* str, const char* suffix);

#endif