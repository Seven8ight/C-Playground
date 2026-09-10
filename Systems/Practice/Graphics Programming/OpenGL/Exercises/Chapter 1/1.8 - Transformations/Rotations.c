#include "../../Header.h"

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
    GLFWInit();

    char windowTitle[] = "Rotations";

    GLFWwindow *window = glfwCreateWindow(800, 650, windowTitle, NULL, NULL);
    if (!window)
    {
        perror("GLFW");
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        perror("GLAD");
        return -2;
    }

    Shaders *shaders = createShaders("Exercises/1.8 - Transformations/Shaders/RotationVert.glsl", "Exercises/1.8 - Transformations/Shaders/RotationFrag.glsl");
    if (!shaders)
        return -3;

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(1);
    unsigned char *data = stbi_load("Images/Donut-3.png", &width, &height, &nrChannels, 0);

    GLenum format = nrChannels == 4 ? GL_RGBA : GL_RGB;

    unsigned int textureId;
    glGenTextures(1, &textureId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

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

    float vertices[] = {
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f};
    unsigned int indices[] = {
        0, 1, 3,
        1, 3, 2};

    unsigned int vao, vbo, ebo;

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

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    mat4 trans = GLM_MAT4_IDENTITY_INIT;
    vec3 positionVector = {0.0f, 0.0f, 1.0f},
         scaleVector = {1.0f, 2.0f, 1.5f},
         translationVector = {-0.0f, -0.5f, 0.0f};

    glm_scale(trans, scaleVector);
    glm_rotate(trans, (float)glfwGetTime(), positionVector);
    glm_translate(trans, translationVector);

    mat4 trans2 = GLM_MAT4_IDENTITY_INIT;
    vec3 positionVector2 = {0.0f, 0.0f, 1.0f},
         scaleVector2 = {2.0f, 2.0f, 2.5f},
         translationVector2 = {1.0f, 0.5f, 0.0f};

    glm_scale(trans2, scaleVector2);
    glm_rotate(trans2, (float)glfwGetTime(), positionVector2);
    glm_translate(trans2, translationVector2);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(1.0f, 0.5f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        processInput(window);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

        glUseProgram(shaders->shaderProgram);
        glUniform1i(glGetUniformLocation(shaders->shaderProgram, "textureMapping"), 0);
        glUniformMatrix4fv(glGetUniformLocation(shaders->shaderProgram, "transform"), 1, GL_FALSE, (float *)trans);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

        glUseProgram(shaders->shaderProgram);
        glUniform1i(glGetUniformLocation(shaders->shaderProgram, "textureMapping"), 0);
        glUniformMatrix4fv(glGetUniformLocation(shaders->shaderProgram, "transform"), 1, GL_FALSE, (float *)trans2);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
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
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    static bool polygon = false;

    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
    {
        if (polygon)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            polygon = false;
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            polygon = true;
        }
    }
}
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}
char *shaderFile(char *filePath)
{
    FILE *shaderFile = fopen(filePath, "r");
    if (!shaderFile)
    {
        perror("File error");
        return NULL;
    }

    fseek(shaderFile, 0, SEEK_END);
    long fileSize = ftell(shaderFile);
    fseek(shaderFile, 0, SEEK_SET);

    char *shaderContents = calloc(fileSize + 1, sizeof(char));
    if (!shaderContents)
    {
        perror("Calloc error");
        return NULL;
    }

    fread(shaderContents, fileSize + 1, sizeof(char), shaderFile);

    return shaderContents;
}
Shaders *createShaders(char *vertexFilePath, char *fragmentFilePath)
{
    if (strlen(vertexFilePath) <= 0 || strlen(fragmentFilePath) <= 0)
    {
        printf("Invalid file path");
        return NULL;
    }

    Shaders *shaders = malloc(sizeof(Shaders));

    const char *vertexFile = shaderFile(vertexFilePath),
               *fragmentFile = shaderFile(fragmentFilePath);

    if (!vertexFile || !fragmentFile)
        return NULL;

    shaders->vertexShader = glCreateShader(GL_VERTEX_SHADER);
    shaders->fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    shaders->shaderProgram = glCreateProgram();

    glShaderSource(shaders->vertexShader, 1, &vertexFile, 0);
    glCompileShader(shaders->vertexShader);

    int vertexShaderSuccess;
    char vertexInfoLog[BUFSIZ];
    glGetShaderiv(shaders->vertexShader, GL_COMPILE_STATUS, &vertexShaderSuccess);

    if (!vertexShaderSuccess)
    {
        glGetShaderInfoLog(shaders->vertexShader, BUFSIZ, NULL, vertexInfoLog);
        printf("Vertex Compile Error: %s", vertexInfoLog);
        free(shaders);
        return NULL;
    }

    glShaderSource(shaders->fragmentShader, 1, &fragmentFile, 0);
    glCompileShader(shaders->fragmentShader);

    int fragmentShaderSuccess;
    char fragmentInfoLog[BUFSIZ];
    glGetShaderiv(shaders->fragmentShader, GL_COMPILE_STATUS, &fragmentShaderSuccess);

    if (!fragmentShaderSuccess)
    {
        glGetShaderInfoLog(shaders->fragmentShader, BUFSIZ, NULL, fragmentInfoLog);
        printf("Fragment Compile Error: %s", fragmentInfoLog);
        free(shaders);
        return NULL;
    }

    glAttachShader(shaders->shaderProgram, shaders->vertexShader);
    glAttachShader(shaders->shaderProgram, shaders->fragmentShader);
    glLinkProgram(shaders->shaderProgram);

    int shaderProgramSuccess;
    char programInfoLog[BUFSIZ];
    glGetProgramiv(shaders->shaderProgram, GL_LINK_STATUS, &shaderProgramSuccess);

    if (!shaderProgramSuccess)
    {
        glGetProgramInfoLog(shaders->shaderProgram, BUFSIZ, NULL, programInfoLog);
        printf("Shader link error: %s", programInfoLog);
        free(shaders);
        return NULL;
    }

    return shaders;
}