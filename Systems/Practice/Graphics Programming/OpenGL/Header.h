#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define GLFW_INCLUDE_NONE

#include <glad.h>
#include <GLFW/glfw3.h>

typedef void (*SetBool)(unsigned int shaderProgram, char *uniformName, bool value);
typedef void (*SetInt)(unsigned int shaderProgram, char *uniformName, int value);
typedef void (*SetFloat)(unsigned int shaderProgram, char *uniformName, float value, float value2, float value3);
typedef void (*Generic)(void);

typedef enum
{
    INT,
    BOOL,
    FLOAT
} DataType;

typedef struct
{
    DataType type;
    char *vertexFilePath, *fragmentFilePath;
    const char *vertexFile, *fragmentFile;
    int vertexShader, fragmentShader, shaderProgram;
    SetBool *boolSetter;
    SetInt *intSetter;
    SetFloat *floatSetter;
} Shader;