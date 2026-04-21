#include "Header.h"

void GLFWinit();
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
char *shaderFile(char *filePath);

int main()
{
    // Window and GLAD initialization
    GLFWinit();

    char windowtitle[] = "Window";
    GLFWwindow *window = glfwCreateWindow(800, 600, windowtitle, NULL, NULL);
    if (!window)
    {
        perror("Window");
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        perror("GLAD");
        return -1;
    }

    glViewport(0, 0, 800, 600);

    // Triangle creation
    float vertices[] = {
        0.0f, 0.5f, 0.0f,
        -0.5f, 0.0f, 0.0f,
        0.5f, 0.0f, 0.0f};
    unsigned int vbo, vao;

    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void *)0);
    glEnableVertexAttribArray(0);

    // Shaders
    unsigned int vertexShader, fragmentShader, shaderProgram;
    char vertexShaderFilePath[] = "./GLSL/Basic.vert",
         fragmentShaderFilePath[] = "./GLSL/Basic.frag";

    const char *vertexShaderCode = shaderFile(vertexShaderFilePath),
               *fragmentShaderCode = shaderFile(fragmentShaderFilePath);

    // Load vertex shader
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertexShader, 1, &vertexShaderCode, NULL);
    glCompileShader(vertexShader);

    // Check for vertex shader errors
    int vertexSuccess;
    char vertexInfoLog[BUFSIZ];

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertexSuccess);
    if (!vertexSuccess)
    {
        glGetShaderInfoLog(vertexShader, BUFSIZ, NULL, vertexInfoLog);
        printf("Vertex shader error on runtime: %s", vertexInfoLog);
        return -1;
    }

    // Load fragment shader
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragmentShader, 1, &fragmentShaderCode, NULL);
    glCompileShader(fragmentShader);

    // Check for fragment shader errors
    int fragmentSuccess;
    char fragmentInfoLog[BUFSIZ];

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragmentSuccess);
    if (!fragmentSuccess)
    {
        glGetShaderInfoLog(fragmentShader, BUFSIZ, NULL, fragmentInfoLog);
        printf("Fragment shader error on runtime: %s", fragmentInfoLog);
        return -1;
    }

    // Link shaders
    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check link shader errors
    int shaderSuccess;
    char shaderInfoLog[BUFSIZ];

    glGetShaderiv(shaderProgram, GL_LINK_STATUS, &shaderSuccess);
    if (!fragmentSuccess)
    {
        glGetShaderInfoLog(shaderProgram, BUFSIZ, NULL, shaderInfoLog);
        printf("Shader linker error on runtime: %s", shaderInfoLog);
        return -1;
    }

    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GLFW_TRUE);

        glClearColor(0.3f, 0.5f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(vao);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

    glfwTerminate();

    return 0;
}

void GLFWinit()
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
