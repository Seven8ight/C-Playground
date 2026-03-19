#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum FileType
{
    TEXT,
    BINARY,
    TEXT_APPEND,
    BINARY_APPEND
} FType;

typedef struct BinaryData
{
    int value;
} Bin;

void writeFile(char *fileName, FType type, void *data, int noOfElements);
void readFile(char *fileName, FType);
void eraseBinData(char *fileName, int valueToErase);