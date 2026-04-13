#include "Header.h"

typedef struct
{
    unsigned int vertexShader, fragmentShader, shaderProgram;
} Shaders;

void GLFWInit();
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
char *shaderFile(char *filePath);
Shaders *createShaders(char *vertexFilePath, char *fragmentFilePath);

int main()
{
    // GLFW initialization
    GLFWInit();

    // Window creation
    char title[] = "Window";

    GLFWwindow *window = glfwCreateWindow(800, 600, title, NULL, NULL);
    if (!window)
        return -1;

    // Handle window resizing
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // Thread initialization, making the program execution the main thread
    glfwMakeContextCurrent(window);

    // Glad initialization - for opengl pointers necessary for OS specialization
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        perror("GLAD");
        return -1;
    }

    glViewport(0, 0, 800, 600);

    int width, height, nrChannels;

    stbi_set_flip_vertically_on_load(1);
    unsigned char *data = stbi_load("Images/Donut-3.png", &width, &height, &nrChannels, 0);

    GLenum format = nrChannels == 4 ? GL_RGBA : GL_RGB;

    unsigned int textureId, vao, vbo, ebo;
    glGenTextures(1, &textureId);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    Shaders *shaders = createShaders("GLSL/TextureVert.glsl", "GLSL/TextureFrag.glsl");

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        perror("Image");
        return -2;
    }

    stbi_image_free(data);

    float vertices[] = {
        // positions // colors // texture coords
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,   // top right
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
        -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f   // top left
    };
    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3};

    glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof indices, indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *)(sizeof(float) * 3));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *)(sizeof(float) * 6));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.5f, 0.4f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaders->shaderProgram);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void GLFWInit()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
}
void processInput(GLFWwindow *window)
{
    if (!window)
        return;

    // If not pressed returns GLFW_RELEASE
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    static int count = 0;

    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
        printf("Y key pressed: %d", count++);
}
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}
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
    fseek(file, 0, SEEK_SET);

    char *buffer = malloc(fileSize + 1);
    fread(buffer, sizeof(char), fileSize, file);

    fclose(file);
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

    const char *vertexFile = shaderFile(vertexFilePath),
               *fragmentFile = shaderFile(fragmentFilePath);

    if (!vertexFile || !fragmentFile)
    {
        free(shaders);
        return NULL;
    }

    shaders->vertexShader = glCreateShader(GL_VERTEX_SHADER);
    shaders->fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    shaders->shaderProgram = glCreateProgram();

    glShaderSource(shaders->vertexShader, 1, &vertexFile, 0);
    glShaderSource(shaders->fragmentShader, 1, &fragmentFile, 0);

    glCompileShader(shaders->vertexShader);
    glCompileShader(shaders->fragmentShader);

    int vertexSuccess, fragmentSuccess;
    char vertexInfoLog[BUFSIZ], fragmentInfoLog[BUFSIZ];

    glGetShaderiv(shaders->vertexShader, GL_COMPILE_STATUS, &vertexSuccess);
    glGetShaderiv(shaders->fragmentShader, GL_COMPILE_STATUS, &fragmentSuccess);

    if (!vertexSuccess)
    {
        glGetShaderInfoLog(shaders->vertexShader, BUFSIZ, 0, vertexInfoLog);
        printf("Vertex shader error: %s", vertexInfoLog);
        free(shaders);
        return NULL;
    }
    if (!fragmentSuccess)
    {
        glGetShaderInfoLog(shaders->fragmentShader, BUFSIZ, 0, fragmentInfoLog);
        printf("Fragment shader error: %s", fragmentInfoLog);
        free(shaders);
        return NULL;
    }

    glAttachShader(shaders->shaderProgram, shaders->vertexShader);
    glAttachShader(shaders->shaderProgram, shaders->fragmentShader);
    glLinkProgram(shaders->shaderProgram);

    int shaderSuccessLink;
    char shaderInfoLog[BUFSIZ];

    glGetShaderiv(shaders->shaderProgram, GL_LINK_STATUS, &shaderSuccessLink);
    if (!shaderSuccessLink)
    {
        glGetShaderInfoLog(shaders->shaderProgram, BUFSIZ, 0, shaderInfoLog);
        printf("Shader error: %s", shaderInfoLog);
        free(shaders);
        return NULL;
    }

    return shaders;
}