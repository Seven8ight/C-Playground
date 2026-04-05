#include "Header.h"

void GLFWInit();
GLFWwindow *createWindow(int width, int height, char *windowTitle);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
char *shaderFile(char *filePath);
void processInput(GLFWwindow *window);
void loadShaders(unsigned int *shader);
void VBO(unsigned int *vao, unsigned int *vbo, unsigned int *shader);
void EBO(unsigned int *vao, unsigned int *ebo);

int main()
{

    GLFWInit();

    // Window creation
    char title[] = "Window";

    GLFWwindow *window = createWindow(800, 600, title);
    if (!window)
        return -1;

    // Handle window resizing
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // Thread intialization, making the program execution the main thread
    glfwMakeContextCurrent(window);

    // Glad initialization - for opengl pointers necessary for OS specialization
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        perror("GLAD");
        return -1;
    }

    // Coordinates for origin within the window to start placing items in the window
    glViewport(0, 0, 800, 600);
    unsigned int vao, vbo, ebo, shader;
    // VBO(&vao, &vbo);
    loadShaders(&shader);
    // Below code for ebo and shaders
    EBO(&vao, &ebo);
    // Ensures the window is kept open unless requested to close(pressing close button) - render loop
    // Each iteration loop for rendering is called a frame(frame update)
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader);
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // glBindVertexArray(0);
        // glDrawArrays(GL_TRIANGLES, 0, 3);
        // Swaps color buffer used in rendering and show its output to the screen during state update
        glfwSwapBuffers(window);
        // Ensures all triggers are recorded and state is updated(keyboard and mouse and anything else) and callbacks are called
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
void processInput(GLFWwindow *window)
{

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    int count = 0;
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
        printf("Y key pressed %d\n", count++);
}
GLFWwindow *createWindow(int width, int height, char *windowTitle)
{
    GLFWwindow *window = glfwCreateWindow(width, height, windowTitle, NULL, NULL);

    if (!window)
    {
        perror("Memory");
        return NULL;
    }

    return window;
}
void VBO(unsigned int *vao, unsigned int *vbo, unsigned int *shader)
{
    // The corresponding 3d coordinates for each vertex
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f};

    // Vertex buffer object to store our coordinates in the GPU

    glGenBuffers(1, vbo);                                                     // Used to create a vertex buffer object
    glGenVertexArrays(1, vao);                                                // Used to create a vertex array object
    glBindVertexArray(*vao);                                                  // Used to bind a vertex array object
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);                                      // Used to bind a buffer object type to a data object in our workspace. Any calls to array_buffer affect the binded data object
    glBindVertexArray(*vao);                                                  // Used to bind vertex array to the specified created vertex array object all subsequent calls to the array object affect the specified here
    glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW); // Used to load the vertices specified into the buffer object

    loadShaders(shader);

    // Informing opengl how to interpret the vertices specified provided by the binded array_buffer.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, NULL);
    glEnableVertexAttribArray(0);
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
void EBO(unsigned int *vao, unsigned int *ebo)
{
    float vertices[] = {
        0.5f, 0.5f, 0.0f,   // top right
        0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f, // bottom left
        -0.5f, 0.5f, 0.0f   // top left
    };

    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    glGenVertexArrays(1, vao);
    glBindVertexArray(*vao);
    glGenBuffers(1, ebo);

    glBindBuffer(GL_ARRAY_BUFFER, *vao);
    glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, NULL);
}
void loadShaders(unsigned int *shader)
{
    // Vertex shader
    char vertexFilePath[] = "./Basic.vert", *vertexGLShader = shaderFile(vertexFilePath);

    if (!vertexGLShader)
        return;

    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertexShader, 1, &vertexGLShader, NULL);
    glCompileShader(vertexShader);

    // Check for vertex shader errors
    int vertexSuccess;
    char vertexInfoLog[BUFSIZ];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertexSuccess);

    if (!vertexSuccess)
    {
        glGetShaderInfoLog(vertexShader, BUFSIZ, NULL, vertexInfoLog);
        printf("Vertex shader error on runtime: %s\n", vertexInfoLog);
        return;
    }

    // Fragment shader
    char fragmentFilePath[] = "./Basic.frag", *fragSource = shaderFile(fragmentFilePath);
    uint8_t fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragmentShader, 1, &fragSource, NULL);
    glCompileShader(fragmentShader);

    // Check for fragment shader errors
    int fragmentSuccess;
    char fragmentInfoLog[BUFSIZ];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragmentSuccess);

    if (!fragmentSuccess)
    {
        glGetShaderInfoLog(fragmentShader, BUFSIZ, NULL, fragmentInfoLog);
        printf("Fragment shader error on runtime: %s\n", fragmentInfoLog);
        return;
    }

    // Creating linker shader

    *shader = glCreateProgram();
    glAttachShader(*shader, vertexShader);
    glAttachShader(*shader, fragmentShader);
    glLinkProgram(*shader);

    // Check for errors
    int shaderSuccess;
    char shaderInfoLog[BUFSIZ];

    glGetShaderiv(*shader, GL_COMPILE_STATUS, &shaderSuccess);

    if (!shaderSuccess)
    {
        glGetShaderInfoLog(*shader, BUFSIZ, NULL, shaderInfoLog);
        printf("Shader Linker error on runtime: %s\n", shaderInfoLog);
        return;
    }
}