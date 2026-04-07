#include "Header.h"

const int WINDOW_WIDTH = 800,
          WINDOW_HEIGHT = 600;

void GLFWinit();
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
char *shaderFile(char *filePath);

int main()
{
    GLFWinit();

    char windowTitle[] = "Window";
    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, windowTitle, NULL, NULL);

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        perror("GLAD");
        return -1;
    }

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    unsigned int vertexShader, fragmentShader, shaderProgram;
    char vertexFilePath[] = "./GLSL/Vertex.glsl",
         fragmentFilePath[] = "./GLSL/Fragment.glsl";

    const char *vertexFile = shaderFile(vertexFilePath),
               *fragmentFile = shaderFile(fragmentFilePath);

    if (!vertexFile || !fragmentFile)
    {
        perror("Memory");
        return -1;
    }

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexFile, NULL);
    glCompileShader(vertexShader);

    int vertexSuccess;
    char vertexInfoLog[BUFSIZ];

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertexSuccess);

    if (!vertexSuccess)
    {
        glGetShaderInfoLog(vertexShader, BUFSIZ, NULL, vertexInfoLog);
        printf("Vertex shader error: %s", vertexInfoLog);
        return -1;
    }

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentFile, NULL);
    glCompileShader(fragmentShader);

    int fragmentSuccess;
    char fragmentInfoLog[BUFSIZ];

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragmentSuccess);

    if (!fragmentSuccess)
    {
        glGetShaderInfoLog(fragmentShader, BUFSIZ, NULL, fragmentInfoLog);
        printf("Fragment shader error: %s", fragmentInfoLog);
        return -1;
    }

    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    float vertices[] = {
        -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left - 3, colors - 3->6
        0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // top - 3, colors - 3->6
        0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f   // bottom right - 3, colors - 3->6
    };
    unsigned int vbo, vao;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glUseProgram(shaderProgram);

    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);

        glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glfwTerminate();
    return 0;
}

void GLFWinit()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
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