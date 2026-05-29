#include "../Header.h"

const int WINDOW_WIDTH = 800,
          WINDOW_HEIGHT = 600;

typedef struct
{
    unsigned int vertexShader, fragmentShader, shaderProgram;
} Shaders;
typedef struct
{
    bool firstMouse;
    float yaw, pitch, zoom;
    int lastX, lastY;
} Camera;

void GLFWInit();
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void setUniformMatrix(Shaders *shaders, char *uniformLabel, mat4 *value);
char *shaderFile(char *filePath);
Shaders *createShaders(char *vertexFilePath, char *fragmentFilePath);
Camera *createCamera();
void mouseCallback(GLFWwindow *window);
void handleCameraMovement(GLFWwindow *window, float deltaTime, vec3 *cameraPos, vec3 *cameraDirection, vec3 *cameraUp);
void handleCameraRotation(GLFWwindow *window);

int main(void)
{
    GLFWInit();

    char windowTitle[] = "Sandbox";

    GLFWwindow *sandboxWindow = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, windowTitle, NULL, NULL);
    if (!sandboxWindow)
    {
        perror("Window");
        return -1;
    }

    glfwMakeContextCurrent(sandboxWindow);
    glfwSetFramebufferSizeCallback(sandboxWindow, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        perror("GLAD");
        return -2;
    }

    int fbWidth, fbHeight;
    glfwGetFramebufferSize(sandboxWindow, &fbWidth, &fbHeight);
    glViewport(0, 0, fbWidth, fbHeight);

    Camera *camera = createCamera();
    if (!camera)
    {
        perror("Camera");
        return -3;
    }
    glfwSetWindowUserPointer(sandboxWindow, camera);

    char vertexFilePath[] = "Shaders/SandboxVert.glsl",
         fragmentFilePath[] = "Shaders/SandboxFrag.glsl";

    Shaders *shaders = createShaders(vertexFilePath, fragmentFilePath);
    if (!shaders)
    {
        perror("Shaders");
        return -4;
    }

    float vertices[] = {
        //  X      Y      Z      R     G     B
        -0.5f, -0.5f, -0.5f, 0.5f, 0.6f, 0.2f, // 0. Left,  Bottom, Back
        0.5f, -0.5f, -0.5f, 0.2f, 0.8f, 0.5f,  // 1. Right, Bottom, Back
        0.5f, 0.5f, -0.5f, 0.6f, 0.2f, 0.6f,   // 2. Right, Top,    Back
        -0.5f, 0.5f, -0.5f, 0.4f, 0.1f, 0.9f,  // 3. Left,  Top,    Back
        -0.5f, -0.5f, 0.5f, 0.5f, 0.3f, 0.1f,  // 4. Left,  Bottom, Front
        0.5f, -0.5f, 0.5f, 0.5f, 0.3f, 0.6f,   // 5. Right, Bottom, Front
        0.5f, 0.5f, 0.5f, 0.9f, 0.4f, 0.8f,    // 6. Right, Top,    Front
        -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.4f    // 7. Left,  Top,    Front
    };
    unsigned int indices[] = {
        // Front face
        0, 1, 2, 2, 3, 0,
        // Right face
        1, 5, 6, 6, 2, 1,
        // Back face
        5, 4, 7, 7, 6, 5,
        // Left face
        4, 0, 3, 3, 7, 4,
        // Top face
        3, 2, 6, 6, 7, 3,
        // Bottom face
        4, 5, 1, 1, 0, 4};

    unsigned int vao, vbo, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof indices, indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void *)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void *)(sizeof(float) * 3));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glEnable(GL_DEPTH_TEST);

    glUseProgram(shaders->shaderProgram);

    mat4 modelMatrix = GLM_MAT4_IDENTITY_INIT,
         viewMatrix = GLM_MAT4_IDENTITY_INIT,
         projectionMatrix = GLM_MAT4_IDENTITY_INIT;

    vec3 translationVector = {0.0f, 0.0f, -3.0f};
    float aspectRatio = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;

    glm_translate(viewMatrix, translationVector);
    glm_perspective(glm_rad(45), aspectRatio, 0.1f, 100.0f, projectionMatrix);

    setUniformMatrix(shaders, "modelMatrix", (mat4 *)modelMatrix);
    setUniformMatrix(shaders, "projectionMatrix", (mat4 *)projectionMatrix);

    vec3 cameraPos = {0.0f, 0.0f, 3.0f},
         cameraTarget = {0.0f, 0.0f, 0.0f},
         cameraRight,
         cameraDirection = {0.0f, 0.0f, -1.0f},
         cameraUp = {0.0f, 1.0f, 0.0f},
         viewTranslation = {0.0f, 0.0f, -3.0f};

    float deltaTime = 0.0f,
          lastFrame = 0.0f;

    while (!glfwWindowShouldClose(sandboxWindow))
    {
        glClearColor(0.6f, 0.5f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(sandboxWindow);
        handleCameraMovement(sandboxWindow, deltaTime, (vec3 *)cameraPos, (vec3 *)cameraDirection, (vec3 *)cameraUp);

        glm_mat3_identity(viewMatrix);
        vec3 targetPoint;
        glm_vec3_add(cameraPos, cameraDirection, targetPoint); // Assuming cameraDirection acts as your front vector
        glm_translate(viewMatrix, viewTranslation);
        glm_lookat(cameraPos, targetPoint, cameraUp, viewMatrix);

        setUniformMatrix(shaders, "viewMatrix", (mat4 *)viewMatrix);

        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void *)0);

        glfwSwapBuffers(sandboxWindow);
        glfwPollEvents();
    }

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
        glfwSetWindowShouldClose(window, GL_TRUE);
}
void setUniformMatrix(Shaders *shaders, char *uniformLabel, mat4 *value)
{
    if (strlen(uniformLabel) <= 0)
    {
        printf("Invalid uniform label");
        return;
    }
    if (!shaders)
    {
        printf("Shader memory corrupted");
        return;
    }

    unsigned int uniformLocation = glGetUniformLocation(shaders->shaderProgram, uniformLabel);
    glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, (float *)value);
}
char *shaderFile(char *filePath)
{
    if (strlen(filePath) <= 0)
    {
        printf("Invalid file path");
        return NULL;
    }

    FILE *shFile = fopen(filePath, "r");
    if (!shFile)
    {
        perror("Invalid file");
        return NULL;
    }

    fseek(shFile, 0, SEEK_END);
    long fileSize = ftell(shFile);
    fseek(shFile, 0, SEEK_SET);

    char *buffer = calloc(fileSize + 1, sizeof(char));
    if (!buffer)
    {
        perror("Memory");
        return NULL;
    }

    fread(buffer, fileSize, sizeof(char), shFile);
    fclose(shFile);
    return buffer;
}
Shaders *createShaders(char *vertexFilePath, char *fragmentFilePath)
{
    Shaders *shaders = calloc(1, sizeof(Shaders));
    if (!shaders)
    {
        perror("Memory");
        return NULL;
    }

    shaders->vertexShader = glCreateShader(GL_VERTEX_SHADER);
    shaders->fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    shaders->shaderProgram = glCreateProgram();

    const char *vertexFile = shaderFile(vertexFilePath),
               *fragmentFile = shaderFile(fragmentFilePath);

    if (!vertexFile || !fragmentFile)
    {
        printf("Either or both files is invalid");
        free(shaders);
        return NULL;
    }

    glShaderSource(shaders->vertexShader, 1, &vertexFile, NULL);
    glShaderSource(shaders->fragmentShader, 1, &fragmentFile, NULL);
    glCompileShader(shaders->vertexShader);
    glCompileShader(shaders->fragmentShader);

    int vertexSuccess;
    char vertexInfoLog[BUFSIZ];
    glGetShaderiv(shaders->vertexShader, GL_COMPILE_STATUS, &vertexSuccess);
    if (!vertexSuccess)
    {
        glGetShaderInfoLog(shaders->vertexShader, BUFSIZ, NULL, vertexInfoLog);
        printf("Vertex shader compile error: %s", vertexInfoLog);
    }

    int fragmentSuccess;
    char fragmentInfoLog[BUFSIZ];
    glGetShaderiv(shaders->fragmentShader, GL_COMPILE_STATUS, &fragmentSuccess);
    if (!fragmentSuccess)
    {
        glGetShaderInfoLog(shaders->fragmentShader, BUFSIZ, NULL, fragmentInfoLog);
        printf("Fragment shader compile error: %s", fragmentInfoLog);
    }

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
    }

    free((void *)vertexFile);
    free((void *)fragmentFile);
    return shaders;
}
Camera *createCamera()
{
    Camera *camera = malloc(sizeof(Camera));
    if (!camera)
    {
        perror("Memory");
        return NULL;
    }

    camera->firstMouse = false;
    camera->zoom = 45.0f;
    camera->yaw = -90.0f;
    camera->lastX = WINDOW_WIDTH / 2;
    camera->lastY = WINDOW_HEIGHT / 2;

    return camera;
}
void handleCameraMovement(GLFWwindow *window, float deltaTime, vec3 *cameraPos, vec3 *cameraDirection, vec3 *cameraUp)
{
    float cameraSpeed = 2.5f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        vec3 newCameraPosition;
        glm_vec3_scale((float *)cameraDirection, cameraSpeed, newCameraPosition);
        glm_vec3_add((float *)cameraPos, newCameraPosition, (float *)cameraPos);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        vec3 newCameraPosition;
        glm_vec3_scale((float *)cameraDirection, cameraSpeed, newCameraPosition);
        glm_vec3_sub((float *)cameraPos, newCameraPosition, (float *)cameraPos);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        vec3 crossResult;
        glm_vec3_cross(cameraDirection, cameraUp, crossResult);
        glm_normalize(crossResult);

        vec3 velocity;
        glm_vec3_scale(crossResult, cameraSpeed, velocity);
        glm_vec3_sub(cameraPos, velocity, cameraPos);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        vec3 crossResult;
        glm_vec3_cross(cameraDirection, cameraUp, crossResult);
        glm_normalize(crossResult);

        vec3 velocity;
        glm_vec3_scale(crossResult, cameraSpeed, velocity);
        glm_vec3_add(cameraPos, velocity, cameraPos);
    }
}
void handleCameraRotation(GLFWwindow *window) {}