#include "../Header.h"

const int WINDOW_WIDTH = 800,
          WINDOW_HEIGHT = 500;

typedef struct
{
    unsigned vertexShader, fragmentShader, shaderProgram;
} Shaders;

void GLFWInit();
char *shaderFile(char *filePath);
Shaders *createShaders(char *vertexFilePath, char *fragmentFilePath);
void framebuffer_size_callback(GLFWwindow *windoow, int width, int height);
void processInput();

int main()
{
    // Initialization
    GLFWInit();

    char windowTitle[] = "Window";
    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, windowTitle, NULL, NULL);

    if (!window)
    {
        perror("GLFW Window");
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        perror("Glad");
        return -2;
    }

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    // Shaders
    char vertexFilePath[] = "./Exercises - 3/GLSL/Vertex1.glsl",
         fragmentFilePath[] = "./Exercises - 3/GLSL/Frag1.glsl";

    Shaders *shaders = createShaders(vertexFilePath, fragmentFilePath);
    if (!shaders)
    {
        perror("Shaders");
        return -3;
    }

    // Textures
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrChannels;
    unsigned char *data = stbi_load("./Images/Donut-3.png", &width, &height, &nrChannels, 0);

    GLenum format = nrChannels == 4 ? GL_RGBA : GL_RGB;
    unsigned int textureId, textureId2;

    glGenTextures(1, &textureId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        perror("Image");
        return -4;
    }

    stbi_image_free(data);

    int width2, height2, nrChannels2;
    unsigned char *data2 = stbi_load("./Images/PlaqueTale.png", &width2, &height2, &nrChannels2, 0);
    GLenum format2 = nrChannels2 == 4 ? GL_RGBA : GL_RGB;

    glGenTextures(1, &textureId2);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureId2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    if (data2)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, format2, width2, height2, 0, format2, GL_UNSIGNED_BYTE, data2);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        perror("Image");
        return -4;
    }

    stbi_image_free(data2);

    // Objects
    float vertices[] = {
        // positions // colors // texture coords
        0.5f,
        0.5f,
        0.0f,
        1.0f,
        0.0f,
        0.0f,
        1.0f,
        2.0f, // top right
        0.5f,
        -0.5f,
        0.0f,
        0.0f,
        1.0f,
        0.0f,
        1.4f,
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
    unsigned int indices[] = {0, 1, 3, 1, 2, 3};

    unsigned int vbo, vao, ebo;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof indices, indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *)(sizeof(float) * 3));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *)(sizeof(float) * 6));
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *)(sizeof(float) * 8));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);

    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);

        glClearColor(1.0f, .5f, 0.75f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaders->shaderProgram);

        glUniform1i(glGetUniformLocation(shaders->shaderProgram, "ourTexture"), 0);
        glUniform1i(glGetUniformLocation(shaders->shaderProgram, "ourTexture2"), 1);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteProgram(shaders->shaderProgram);
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
void framebuffer_size_callback(GLFWwindow *windoow, int width, int height)
{
    glViewport(0, 0, width, height);
}
char *shaderFile(char *filePath)
{
    if (strlen(filePath) <= 0)
        return NULL;

    FILE *file = fopen(filePath, "r");

    if (!file)
    {
        perror("File");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = malloc(fileSize + 1);

    fread(buffer, fileSize, 1, file);
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

    shaders->vertexShader = glCreateShader(GL_VERTEX_SHADER);
    shaders->fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    const char *vertexShader = shaderFile(vertexFilePath),
               *fragmentShader = shaderFile(fragmentFilePath);

    if (!vertexShader || !fragmentShader)
    {
        perror("File");
        free(shaders);
        return NULL;
    }

    glShaderSource(shaders->vertexShader, 1, &vertexShader, NULL);
    glCompileShader(shaders->vertexShader);

    int vertexSuccess;
    char vertexInfoLog[BUFSIZ];

    glGetShaderiv(shaders->vertexShader, GL_COMPILE_STATUS, &vertexSuccess);
    if (!vertexSuccess)
    {
        glGetShaderInfoLog(shaders->vertexShader, BUFSIZ, NULL, vertexInfoLog);
        printf("Vertex shader failure: %s", vertexInfoLog);
        free(shaders);
        return NULL;
    }

    glShaderSource(shaders->fragmentShader, 1, &fragmentShader, NULL);
    glCompileShader(shaders->fragmentShader);

    int fragmentSuccess;
    char fragmentInfoLog[BUFSIZ];

    glGetShaderiv(shaders->fragmentShader, GL_COMPILE_STATUS, &fragmentSuccess);
    if (!fragmentSuccess)
    {
        glGetShaderInfoLog(shaders->fragmentShader, BUFSIZ, NULL, fragmentInfoLog);
        printf("Fragment shader failure: %s", fragmentInfoLog);
        free(shaders);
        return NULL;
    }

    shaders->shaderProgram = glCreateProgram();
    glAttachShader(shaders->shaderProgram, shaders->vertexShader);
    glAttachShader(shaders->shaderProgram, shaders->fragmentShader);
    glLinkProgram(shaders->shaderProgram);

    int shaderSuccess;
    char shaderInfoLog[BUFSIZ];

    glGetProgramiv(shaders->shaderProgram, GL_LINK_STATUS, &shaderSuccess);
    if (!shaderSuccess)
    {
        glGetProgramInfoLog(shaders->shaderProgram, BUFSIZ, NULL, shaderInfoLog);
        printf("Shader link error: %s", shaderInfoLog);
        free(shaders);
        return NULL;
    }

    glDeleteShader(shaders->vertexShader);
    glDeleteShader(shaders->fragmentShader);
    return shaders;
}
