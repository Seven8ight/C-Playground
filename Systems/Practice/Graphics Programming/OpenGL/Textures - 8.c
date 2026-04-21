#include "Header.h"

typedef struct
{
    unsigned int shaderProgram, vertexShader, fragmentShader;
} Shaders;

void GLFWinit();
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
char *shaderFile(char *filePath);
Shaders *createShaders(char *vertexFilePath, char *fragmentFilePath);

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
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        perror("GLAD");
        return -2;
    }

    int width, height, nrChannels,
        width2, height2, nrChannels2;

    stbi_set_flip_vertically_on_load(1);
    unsigned char *data = stbi_load("Images/Donut-3.png", &width, &height, &nrChannels, 0),
                  *data2 = stbi_load("Images/graffiti.png", &width2, &height2, &nrChannels2, 0);

    unsigned int textureId, textureId2;
    glGenTextures(1, &textureId);
    glGenTextures(1, &textureId2);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        printf("Failed to load texture\n");
        return -3;
    }

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureId2);
    stbi_image_free(data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    char vertexFilePath[] = "GLSL/TextureVert2.glsl",
         fragmentFilePath[] = "GLSL/TextureFrag2.glsl";

    Shaders *shaders = createShaders(vertexFilePath, fragmentFilePath);
    GLenum format2 = (nrChannels2 == 4) ? GL_RGBA : GL_RGB;

    if (data2)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, format2, width2, height2, 0, format2, GL_UNSIGNED_BYTE, data2);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        printf("Failed to load texture\n");
        return -3;
    }

    stbi_image_free(data2);

    float vertices[] = {
        // positions // colors // texture coords
        0.5f,
        0.5f,
        0.0f,
        1.0f,
        0.0f,
        0.0f,
        1.0f,
        1.0f, // top right
        0.5f,
        -0.5f,
        0.0f,
        0.0f,
        1.0f,
        0.0f,
        1.0f,
        0.0f, // bottom right
        -0.5f,
        -0.5f,
        0.0f,
        0.0f,
        0.0f,
        1.0f,
        0.0f,
        0.0f,                                           // bottom left
        -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f // Index 3: Top Left (ADD THIS)
    };
    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3};

    unsigned int vao, vbo, ebo;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof indices, indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *)(3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *)(6 * sizeof(float)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);

        glClearColor(0.3f, 0.5f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureId);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textureId2);
        glUseProgram(shaders->shaderProgram);

        glUniform1i(glGetUniformLocation(shaders->shaderProgram, "ourTexture"), 0);
        glUniform1i(glGetUniformLocation(shaders->shaderProgram, "ourTexture2"), 1);
        glBindVertexArray(vao);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

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

    int success;
    char infoLog[512];

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        printf("Program link error: %s\n", infoLog);
    }

    shaders->shaderProgram = shaderProgram;

    return shaders;
}