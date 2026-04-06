#include "../Header.h"

void GLFWinit();
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
char *shaderFile(char *filePath);

int main(void)
{
    GLFWinit();

    char windowTitle[] = "Window";
    GLFWwindow *window = glfwCreateWindow(800, 600, windowTitle, NULL, NULL);
    if (!window)
    {
        perror("Window");
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        perror("GLAD");
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glViewport(0, 0, 800, 600);

    char vertexFilePath[] = "GLSL/Basic.vert",
         fragmentFilePath1[] = "GLSL/Basic.frag",
         fragmentFilePath2[] = "GLSL/Exercise.frag";

    const char *vertexFile = shaderFile(vertexFilePath),
               *fragmentFile1 = shaderFile(fragmentFilePath1),
               *fragmentFile2 = shaderFile(fragmentFilePath2);

    unsigned int vertexShader, fragmentShader1, fragmentShader2, shaderProgram1, shaderProgram2;

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

    fragmentShader1 = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader1, 1, &fragmentFile1, NULL);
    glCompileShader(fragmentShader1);

    int fragmentSuccess1;
    char fragmentInfoLog1[BUFSIZ];

    glGetShaderiv(fragmentShader1, GL_COMPILE_STATUS, &fragmentSuccess1);
    if (!fragmentSuccess1)
    {
        glGetShaderInfoLog(fragmentShader1, BUFSIZ, NULL, fragmentInfoLog1);
        printf("Fragment 1 shader error: %s", fragmentInfoLog1);
        return -1;
    }

    fragmentShader2 = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader2, 1, &fragmentFile2, NULL);
    glCompileShader(fragmentShader2);

    int fragmentSuccess2;
    char fragmentInfoLog2[BUFSIZ];

    glGetShaderiv(fragmentShader2, GL_COMPILE_STATUS, &fragmentSuccess2);
    if (!fragmentSuccess2)
    {
        glGetShaderInfoLog(fragmentShader2, BUFSIZ, NULL, fragmentInfoLog2);
        printf("Fragment 2 shader error: %s", fragmentInfoLog2);
        return -1;
    }

    shaderProgram1 = glCreateProgram();
    shaderProgram2 = glCreateProgram();

    glAttachShader(shaderProgram1, vertexShader);
    glAttachShader(shaderProgram2, vertexShader);

    glAttachShader(shaderProgram1, fragmentShader1);
    glAttachShader(shaderProgram2, fragmentShader2);

    glLinkProgram(shaderProgram1);
    glLinkProgram(shaderProgram2);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader1);
    glDeleteShader(fragmentShader2);

    // Triangles
    float vertices1[] = {
        -0.5f, 0.0f, 0.0f, // bottom right
        0.0f, 0.0f, 0.0f,  // bottom left
        -0.25f, 0.5f, 0.0f // top
    },
          vertices2[] = {
              0.5f, 0.0f, 0.0f, // bottom right
              0.0f, 0.0f, 0.0f, // bottom left
              0.25f, 0.5f, 0.0f // top
          };

    unsigned int indices1[] = {
        0, 1, 2},
                 indices2[] = {0, 1, 2};

    unsigned int vbo1, vao1, ebo1;
    unsigned int vbo2, vao2, ebo2;

    // First triangle
    glGenVertexArrays(1, &vao1);
    glGenBuffers(1, &vbo1);
    glGenBuffers(1, &ebo1);

    glBindVertexArray(vao1);
    glBindBuffer(GL_ARRAY_BUFFER, vbo1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo1);

    glBufferData(GL_ARRAY_BUFFER, sizeof vertices1, vertices1, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof indices1, indices1, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, NULL);
    glEnableVertexAttribArray(0);

    // Unbind first triangle
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Second triangle
    glGenVertexArrays(1, &vao2);
    glGenBuffers(1, &vbo2);
    glGenBuffers(1, &ebo2);

    glBindVertexArray(vao2);
    glBindBuffer(GL_ARRAY_BUFFER, vbo2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo2);

    glBufferData(GL_ARRAY_BUFFER, sizeof vertices2, vertices2, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof indices2, indices2, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, NULL);
    glEnableVertexAttribArray(0);

    // Unbind second triangle;
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    bool wireframe = false;
    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            wireframe = !wireframe;

            if (wireframe)
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            else
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        glClearColor(0.1f, 0.3f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram1);
        glBindVertexArray(vao1);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, NULL);

        glUseProgram(shaderProgram2);
        glBindVertexArray(vao2);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, NULL);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteBuffers(1, &vbo1);
    glDeleteBuffers(1, &ebo1);
    glDeleteBuffers(1, &vbo2);
    glDeleteBuffers(1, &ebo2);
    glDeleteVertexArrays(1, &vao1);
    glDeleteVertexArrays(1, &vao2);

    glDeleteProgram(shaderProgram1);
    glDeleteProgram(shaderProgram2);
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
    glViewport(0, 0, 800, 600);
}
char *shaderFile(char *filePath)
{
    FILE *file = fopen(filePath, "rb");
    if (!file)
    {
        perror("File descriptor");
        return NULL;
    }

    // File size
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = calloc(fileSize + 1, sizeof(char));
    if (!buffer)
    {
        fclose(file);
        return NULL;
    }
    fread(buffer, sizeof(char), fileSize, file);
    buffer[fileSize] = '\0';

    fclose(file);
    return buffer;
}