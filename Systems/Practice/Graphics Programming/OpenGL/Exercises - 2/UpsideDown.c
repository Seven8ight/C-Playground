#include "../Header.h"

typedef struct
{
    unsigned int shaderProgram, vertexShader, fragmentShader;
} Shaders;

void GLFWInit();
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
char *shaderFile(char *filePath);
Shaders *createShaders(char *vertexFilePath, char *fragmentFilePath);

int main(int argc, char *args[])
{
    GLFWInit();

    char windowTitle[] = "Window";
    GLFWwindow *window = glfwCreateWindow(800, 600, windowTitle, NULL, NULL);
    if (!window)
    {
        perror("Window");
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        perror("GLAD");
        return -2;
    }

    char vertexFilePath[] = "Exercises - 2/Shaders/Vert1.glsl",
         fragmentFilePath[] = "Exercises - 2/Shaders/Frag1.glsl";

    Shaders *shaders = createShaders(vertexFilePath, fragmentFilePath);
    if (!shaders)
    {
        printf("Shader error\n");
        return -3;
    }
    const char uniformName[] = "RenderColor";
    int fragmentColor = glGetUniformLocation(shaders->shaderProgram, uniformName);

    float vertices[] = {
        -0.5f, 0.0f, 0.0f, // left
        0.0f, 0.5f, 0.0f,  // top
        0.5f, 0.0f, 0.0f   // right
    };
    unsigned int vao, vbo;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void *)0);

    glEnableVertexAttribArray(0);

    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);

        glClearColor(0.5f, 0.3f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaders->shaderProgram);
        glUniform4f(fragmentColor, 0.2f, 0.3f, 0.5f, 1.0f);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

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
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}
char *shaderFile(char *filePath)
{
    FILE *vertexFile = fopen(filePath, "r");

    if (!vertexFile)
    {
        perror("File descriptor");
        return NULL;
    }

    fseek(vertexFile, 0, SEEK_END);
    long fileSize = ftell(vertexFile);
    rewind(vertexFile);

    char *buffer = calloc(fileSize + 1, sizeof(char));
    fread(buffer, sizeof(char), fileSize, vertexFile);
    buffer[fileSize] = '\0';

    return buffer;
}
Shaders *createShaders(char *vertexFilePath, char *fragmentFilePath)
{
    Shaders *shaders = malloc(sizeof(Shaders));
    if (!shaders)
    {
        perror("Memory");
        return NULL;
    }

    if ((!vertexFilePath || !fragmentFilePath) || (strlen(vertexFilePath) <= 0 || strlen(fragmentFilePath) <= 0))
    {
        perror("File path");
        free(shaders);
        return NULL;
    }

    unsigned int vertexShader, fragmentShader, shaderProgram;

    const char *vertexShaderFile = shaderFile(vertexFilePath),
               *fragmentShaderFile = shaderFile(fragmentFilePath);

    if (!vertexShaderFile || !fragmentShaderFile)
    {
        perror("File");
        free(shaders);
        return NULL;
    }

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    shaderProgram = glCreateProgram();

    glShaderSource(vertexShader, 1, &vertexShaderFile, NULL);
    glCompileShader(vertexShader);

    int vertexStatus;
    char vertexInfoLog[BUFSIZ];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertexStatus);

    if (!vertexStatus)
    {
        glGetShaderInfoLog(vertexShader, BUFSIZ, NULL, vertexInfoLog);
        printf("Vertex error: %s", vertexInfoLog);
        free(shaders);
        return NULL;
    }

    glShaderSource(fragmentShader, 1, &fragmentShaderFile, NULL);
    glCompileShader(fragmentShader);

    int fragmentStatus;
    char fragmentInfoLog[BUFSIZ];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragmentStatus);

    if (!fragmentStatus)
    {
        glGetShaderInfoLog(fragmentShader, BUFSIZ, NULL, fragmentInfoLog);
        printf("Fragment error: %s", fragmentInfoLog);
        free(shaders);
        return NULL;
    }

    shaders->vertexShader = vertexShader;
    shaders->fragmentShader = fragmentShader;

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    shaders->shaderProgram = shaderProgram;

    return shaders;
}