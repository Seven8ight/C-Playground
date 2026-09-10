#include "../Header.h"

typedef struct
{
    int vertexShader;
    int fragmentShader;
    int shaderProgram;
    char *vertexFilePath;
    char *fragmentFilePath;
} ShaderProgram;

void GLFWInit();
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void KeyboardInput(GLFWwindow *window, bool *wireframe);
ShaderProgram *createShader(char *vertexFilePath, char *fragmentFilePath);
char *shaderFile(char *filePath);

int main(void)
{
    GLFWInit();

    const int WINDOW_WIDTH = 800,
              WINDOW_HEIGHT = 800;
    char title[] = "Back to Basics";

    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, title, NULL, NULL);
    if (!window)
    {
        perror("Window generation error");
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        perror("GLAD setup");
        return -2;
    }

    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);

    unsigned int vao, vbo, ebo;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    float vertices[] = {
        -0.5f, 0.0f, 0.0f, 1.0f, 0.8f, 0.3f, // bottom left - 3, colors - 3->6
        0.5f, 0.0f, 0.0f, 0.3f, 0.2f, 0.5f,  // top - 3, colors - 3->6
        0.0f, 0.5f, 0.0f, 0.1f, 0.4f, 1.0f   // bottom right - 3, colors - 3->6
    };
    unsigned int indices[] = {
        0, 1, 2 // Only 3 indices for 1 triangle
    };

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void *)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);

    char vertexFilePath[] = "Remembrance/Intro-Shaders/UniformVert.vert",
         fragmentFilePath[] = "Remembrance/Intro-Shaders/UniformFrag.frag";

    ShaderProgram *programShaders = createShader(vertexFilePath, fragmentFilePath);
    if (!programShaders)
    {
        printf("Shader memory load error");
        return -3;
    }

    glViewport(0, 0, fbWidth, fbHeight);
    glClearColor(0.0f, 0.5f, 0.2f, 1.0f);

    bool wireframe = false;

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        KeyboardInput(window, &wireframe);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(programShaders->shaderProgram);
        const unsigned int uniformLocation = glGetUniformLocation(programShaders->shaderProgram, "objColor");
        glUniform3f(uniformLocation, 0.1f, 0.1f, 0.1f);
        glBindVertexArray(vao);

        if (wireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

        glfwSwapBuffers(window);
    }

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
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

void KeyboardInput(GLFWwindow *window, bool *wireframe)
{
    if (!window)
    {
        perror("Window error");
        return;
    }

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GL_TRUE)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (glfwGetKey(window, GLFW_KEY_W) == GL_TRUE)
        *wireframe = true;
    else if (glfwGetKey(window, GLFW_KEY_S) == GL_TRUE)
        *wireframe = false;
}

char *shaderFile(char *filePath)
{
    FILE *file = fopen(filePath, "r");
    if (!file)
    {
        perror("File error");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);

    char *buffer = malloc(sizeof(char) * (fileSize + 1));
    fread(buffer, 1, fileSize, file);
    buffer[fileSize] = '\0';

    fclose(file);

    return buffer;
}

ShaderProgram *createShader(char *vertexFilePath, char *fragmentFilePath)
{
    ShaderProgram *shader = malloc(sizeof(ShaderProgram));
    if (!shader)
    {
        perror("Shader error");
        return NULL;
    }

    shader->vertexFilePath = vertexFilePath;
    shader->fragmentFilePath = fragmentFilePath;

    const char *vertexFile = shaderFile(vertexFilePath),
               *fragmentFile = shaderFile(fragmentFilePath);

    unsigned vertexShader, fragmentShader, shaderProgram;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    shaderProgram = glCreateProgram();

    glShaderSource(vertexShader, 1, &vertexFile, NULL);
    glCompileShader(vertexShader);

    int vertexSuccess;
    char vertexErrorMsg[BUFSIZ];

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertexSuccess);
    if (!vertexSuccess)
    {
        glGetShaderInfoLog(vertexShader, BUFSIZ, NULL, vertexErrorMsg);
        printf("Vertex shader compile error: %s", vertexErrorMsg);
        free(shader);
        return NULL;
    }

    glShaderSource(fragmentShader, 1, &fragmentFile, NULL);
    glCompileShader(fragmentShader);

    int fragmentShaderSuccess;
    char fragmentErrorMsg[BUFSIZ];

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragmentShaderSuccess);
    if (!fragmentShaderSuccess)
    {
        glGetShaderInfoLog(fragmentShader, BUFSIZ, NULL, fragmentErrorMsg);
        printf("Fragment shader compile error: %s", fragmentErrorMsg);
        free(shader);
        return NULL;
    }

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int linkStatus;
    char linkError[BUFSIZ];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkStatus);

    if (!linkStatus)
    {
        glGetProgramInfoLog(shaderProgram, BUFSIZ, NULL, linkError);
        printf("Shader Linker error: %s", linkError);
        free(shader);
        return NULL;
    }

    shader->vertexShader = vertexShader;
    shader->fragmentShader = fragmentShader;
    shader->shaderProgram = shaderProgram;

    free((void *)vertexFile);
    free((void *)fragmentFile);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shader;
}