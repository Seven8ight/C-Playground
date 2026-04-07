#include "Header.h"

char *shaderFile(char *filePath)
{
    FILE *file = fopen(filePath, "rb");
    if (!file)
    {
        perror("File");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);

    char *buffer = calloc(fileSize + 1, sizeof(char));
    if (!buffer)
    {
        perror("Memory");
        fclose(file);
        return NULL;
    }

    fread(buffer, sizeof(char), fileSize, file);
    buffer[fileSize] = '\0';
    fclose(file);

    return buffer;
}
void useShader(Shader *shader)
{
    if (!shader)
    {
        printf("Invalid shader or invalid program id");
        return;
    }

    glUseProgram(shader->shaderProgram);
}

void intSetter(unsigned int shaderProgram, char *uniformName, int value, int value2, int value3)
{
    unsigned int uniformLocation = glGetUniformLocation(shaderProgram, uniformName);
    glUniform3i(uniformLocation, value, value2, value3);
}
void boolSetter(unsigned int shaderProgram, char *uniformName, bool value)
{
    unsigned int uniformLocation = glGetUniformLocation(shaderProgram, uniformName);
    glUniform1i(uniformLocation, value);
}
void floatSetter(unsigned int shaderProgram, char *uniformName, float value, float value2, float value3)
{
    unsigned int uniformLocation = glGetUniformLocation(shaderProgram, uniformName);
    glUniform3f(uniformLocation, value, value2, value3);
}

Shader *createShader(char *vertexFilePath, char *fragmentFilePath, DataType type, Generic callback)
{
    Shader *shader = (malloc(sizeof(Shader)));
    if (!shader)
    {
        perror("Memory");
        return NULL;
    }

    if ((!vertexFilePath || !fragmentFilePath) && (strlen(vertexFilePath) <= 0 || strlen(fragmentFilePath) <= 0))
    {
        printf("Invalid file path");
        free(shader);
        return NULL;
    }

    shader->vertexFilePath = vertexFilePath;
    shader->fragmentFilePath = fragmentFilePath;

    if (!type)
    {
        printf("Data type should be provided");
        free(shader);
        return NULL;
    }

    shader->type = type;

    switch (type)
    {
    case INT:
        shader->boolSetter = NULL;
        shader->floatSetter = NULL;
        shader->intSetter = (SetInt)(callback);
        break;
    case BOOL:
        shader->boolSetter = (SetBool)(callback);
        shader->floatSetter = NULL;
        shader->intSetter = NULL;
        break;
    case FLOAT:
        shader->boolSetter = NULL;
        shader->floatSetter = (SetFloat)(callback);
        shader->intSetter = NULL;
        break;
    default:
        printf("Invalid type");
        free(shader);
        return NULL;
    }

    shader->vertexShader = glCreateShader(GL_VERTEX_SHADER);
    shader->vertexFile = shaderFile(shader->vertexFilePath);
    glShaderSource(shader->vertexShader, 1, &shader->vertexFile, NULL);
    glCompileShader(shader->vertexShader);

    int vertexSuccess;
    char vertexInfoLog[BUFSIZ];

    glGetShaderiv(shader->vertexShader, GL_COMPILE_STATUS, &vertexSuccess);
    if (!vertexSuccess)
    {
        glGetShaderInfoLog(shader->vertexShader, BUFSIZ, NULL, vertexInfoLog);
        printf("Vertex shader error: %s", vertexInfoLog);
        free(shader);
        return NULL;
    }

    shader->fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    shader->fragmentFile = shaderFile(shader->fragmentFilePath);
    glShaderSource(shader->fragmentShader, 1, &shader->fragmentFile, NULL);
    glCompileShader(shader->fragmentShader);

    int fragmentSuccess;
    char fragmentInfoLog[BUFSIZ];

    glGetShaderiv(shader->fragmentShader, GL_COMPILE_STATUS, &fragmentSuccess);
    if (!fragmentSuccess)
    {
        glGetShaderInfoLog(shader->fragmentShader, BUFSIZ, NULL, fragmentInfoLog);
        printf("Fragment shader error: %s", fragmentInfoLog);
        free(shader);
        return NULL;
    }

    shader->shaderProgram = glCreateProgram();
    glAttachShader(shader->shaderProgram, shader->vertexShader);
    glAttachShader(shader->shaderProgram, shader->fragmentShader);
    glLinkProgram(shader->shaderProgram);

    return shader;
}

void GLFWInit();
void frameBuffer_size_callback(GLFWwindow *window, int width, int height);

int main()
{
    glfwInit();

    char windowTitle[] = "Window";
    GLFWwindow *window = glfwCreateWindow(800, 600, windowTitle, NULL, NULL);
    if (!window)
    {
        perror("Window");
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, frameBuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        perror("GLAD");
        return -2;
    }

    char vertexFilePath[] = "./GLSL/Vertex.glsl",
         fragmentFilePath[] = "./GLSL/Fragment.glsl";

    Shader *shader = createShader(vertexFilePath, fragmentFilePath, FLOAT, floatSetter);
    if (!shader)
    {
        printf("Invalid shader memory initialized");
        return -3;
    }

    char uniformValue[] = "ourColor";

    float vertices[] = {
        0.0f, 0.5f, 0.0f,  // top
        -0.5f, 0.0f, 0.0f, // left
        0.5f, 0.0f, 0.0f   // right
    };
    unsigned int vbo, vao;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindVertexArray(vao);

    glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void *)0);

    glEnableVertexAttribArray(0);

    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);

        glClearColor(0.1f, 0.5f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        useShader(shader);
        (*shader->floatSetter)(shader->shaderProgram, uniformValue, 0.5f, 0.3f, 0.6f);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(shader->shaderProgram);

    free(shader);

    glfwTerminate();
    return 0;
}

void GLFWInit()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
}
void frameBuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}