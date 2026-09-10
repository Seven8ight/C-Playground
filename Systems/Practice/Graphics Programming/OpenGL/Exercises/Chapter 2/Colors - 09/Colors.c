#include "../../../Header.h"

const int WINDOW_WIDTH = 800,
          WINDOW_HEIGHT = 600;

float deltaTime = 0.0f,
      lastFrame = 0.0f;

typedef struct
{
    const char *vertexFilePath;
    const char *fragmentFilePath;
    unsigned int vertexShader, fragmentShader, ShaderProgram;
} Shaders;

typedef struct
{
    bool firstMouse;
    float lastX, lastY;
    double pitch, yaw, zoom;
    vec3 *position, *front, *up;
} Camera;

void GLFWInit();
void frameBufferSizeCallback(GLFWwindow *window, int width, int height);
char *readFile(const char *filePath);
Shaders *createShaderProgram(const char *vertexFilePath, const char *fragmentFilePath);
Camera *createCamera(GLFWwindow *window);
void CameraAngleMovement(GLFWwindow *window, mat4 *viewMatrix);
void KeyBoardInput(GLFWwindow *window, float deltaTime);
void MouseInput(GLFWwindow *window, double xPosition, double yPosition);
void ScrollInput(GLFWwindow *window, double xPosition, double yPosition);

int main(void)
{
    GLFWInit();

    char WindowTitle[] = "Colors";
    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WindowTitle, NULL, NULL);

    if (!window)
    {
        perror("Window");
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        perror("GLAD");
        return -2;
    }

    glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, MouseInput);
    glfwSetScrollCallback(window, ScrollInput);

    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    glViewport(0, 0, fbWidth, fbHeight);

    Camera *camera = createCamera(window);
    if (!camera)
    {
        perror("Camera");
        return -3;
    }

    float vertices[] = {
        // Positions          // Dark Colors (RGB)     // Normals

        // Back face (Dark Red)
        -0.5f, -0.5f, -0.5f, 0.3f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        0.5f, -0.5f, -0.5f, 0.3f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        0.5f, 0.5f, -0.5f, 0.3f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        0.5f, 0.5f, -0.5f, 0.3f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        -0.5f, 0.5f, -0.5f, 0.3f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f, 0.3f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,

        // Front face (Dark Blue)
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.4f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.4f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.4f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.4f, 0.0f, 0.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.4f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.4f, 0.0f, 0.0f, 1.0f,

        // Left face (Dark Green)
        -0.5f, 0.5f, 0.5f, 0.0f, 0.3f, 0.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 0.3f, 0.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.3f, 0.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.3f, 0.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.3f, 0.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.3f, 0.0f, -1.0f, 0.0f, 0.0f,

        // Right face (Dark Purple)
        0.5f, 0.5f, 0.5f, 0.3f, 0.0f, 0.3f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 0.3f, 0.0f, 0.3f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.3f, 0.0f, 0.3f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.3f, 0.0f, 0.3f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.3f, 0.0f, 0.3f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.3f, 0.0f, 0.3f, 1.0f, 0.0f, 0.0f,

        // Bottom face (Dark Teal)
        -0.5f, -0.5f, -0.5f, 0.0f, 0.3f, 0.3f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 0.3f, 0.3f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.3f, 0.3f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.3f, 0.3f, 0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.3f, 0.3f, 0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.3f, 0.3f, 0.0f, -1.0f, 0.0f,

        // Top face (Dark Olive/Grey)
        -0.5f, 0.5f, -0.5f, 0.2f, 0.2f, 0.1f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 0.2f, 0.2f, 0.1f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.2f, 0.2f, 0.1f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.2f, 0.2f, 0.1f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.2f, 0.2f, 0.1f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.2f, 0.2f, 0.1f, 0.0f, 1.0f, 0.0f};
    vec3 cubePositions[] = {{0.0f, 0.0f, 0.0f}, {2.0f, 0.0f, -15.0f}, {-1.5f, -0.2f, -2.5f}, {-3.8f, -0.0f, -12.3f}},
         lightPosition = {1.2f, 1.0f, 2.0f};

    unsigned int vbo, vao, lightVao;
    glGenVertexArrays(1, &vao);
    glGenVertexArrays(1, &lightVao);
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Object Data
    glBindVertexArray(vao);
    glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 9, (void *)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 9, (void *)(sizeof(float) * 3));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 9, (void *)(sizeof(float) * 6));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    // Light Data
    glBindVertexArray(lightVao);

    glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 9, (void *)0);
    glEnableVertexAttribArray(0);

    const char objectVertexFilePath[] = "Exercises/Chapter 2/Colors - 09/Shaders/Colors.vert",
               objectFragmentFilePath[] = "Exercises/Chapter 2/Colors - 09/Shaders/Colors.frag",
               lightVertexFilePath[] = "Exercises/Chapter 2/Colors - 09/Shaders/Lights.vert",
               lightFragmentFilePath[] = "Exercises/Chapter 2/Colors - 09/Shaders/Lights.frag";

    Shaders *shaders = createShaderProgram(objectVertexFilePath, objectFragmentFilePath),
            *lightShaders = createShaderProgram(lightVertexFilePath, lightFragmentFilePath);

    if (!shaders || !lightShaders)
    {
        perror("Shaders");
        return -4;
    }

    mat4 projectionMatrix = GLM_MAT4_IDENTITY_INIT;

    glEnable(GL_DEPTH_TEST);

    while (glfwWindowShouldClose(window) == GL_FALSE)
    {
        // Calculate delta time
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        lightPosition[0] = sin(currentFrame) * 2.0f;
        lightPosition[2] = cos(currentFrame) * 2.0f;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        glfwPollEvents();

        KeyBoardInput(window, deltaTime);

        // Objects
        mat4 modelMatrix = GLM_MAT4_IDENTITY_INIT,
             viewMatrix = GLM_MAT4_IDENTITY_INIT,
             projectionMatrix = GLM_MAT4_IDENTITY_INIT;

        glBindVertexArray(vao);
        glUseProgram(shaders->ShaderProgram);

        unsigned int objModelLocation = glGetUniformLocation(shaders->ShaderProgram, "model"),
                     objViewLocation = glGetUniformLocation(shaders->ShaderProgram, "view"),
                     objProjectionLocation = glGetUniformLocation(shaders->ShaderProgram, "projection"),
                     lightColorLocation = glGetUniformLocation(shaders->ShaderProgram, "LightColor"),
                     objColorLocation = glGetUniformLocation(shaders->ShaderProgram, "ObjectColor"),
                     lightPositionLocation = glGetUniformLocation(shaders->ShaderProgram, "LightPosition"),
                     cameraPositionLocation = glGetUniformLocation(shaders->ShaderProgram, "CameraPosition"),
                     normalsInverseLocation = glGetUniformLocation(shaders->ShaderProgram, "normalInverse");

        glm_perspective(glm_rad(camera->zoom), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f, projectionMatrix);
        CameraAngleMovement(window, &viewMatrix);

        glUniformMatrix4fv(objViewLocation, 1, GL_FALSE, (float *)viewMatrix);
        glUniformMatrix4fv(objProjectionLocation, 1, GL_FALSE, (float *)projectionMatrix);
        glUniform3f(lightColorLocation, 1.0f, 1.0f, 1.0f);
        glUniform3f(objColorLocation, 1.0f, 0.5f, 0.31f);
        glUniform3f(lightPositionLocation, lightPosition[0], lightPosition[1], lightPosition[2]);
        glUniform3f(cameraPositionLocation, (*camera->position)[0], (*camera->position)[1], (*camera->position)[2]);

        for (int i = 0; i < sizeof(cubePositions) / sizeof(vec3); i++)
        {
            glm_mat4_identity(modelMatrix);

            glm_translate(modelMatrix, cubePositions[i]);

            vec3 rotateAxis = {0.5, 0.5, 0.0};
            glm_rotate(modelMatrix, glm_rad(glfwGetTime()), (float *)rotateAxis);

            mat4 inverseModel;
            mat3 normalMatrix;
            glm_mat4_inv(modelMatrix, inverseModel);
            glm_mat4_transpose(inverseModel);
            glm_mat4_pick3(inverseModel, normalMatrix);

            glUniformMatrix3fv(normalsInverseLocation, 1, GL_FALSE, (float *)normalMatrix);

            glUniformMatrix4fv(objModelLocation, 1, GL_FALSE, (float *)modelMatrix);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // Light
        unsigned int lightModelLocation = glGetUniformLocation(lightShaders->ShaderProgram, "model"),
                     lightViewLocation = glGetUniformLocation(lightShaders->ShaderProgram, "view"),
                     lightProjectionLocation = glGetUniformLocation(lightShaders->ShaderProgram, "projection");

        glBindVertexArray(lightVao);
        glUseProgram(lightShaders->ShaderProgram);

        glUniformMatrix4fv(lightViewLocation, 1, GL_FALSE, (float *)viewMatrix);
        glUniformMatrix4fv(lightProjectionLocation, 1, GL_FALSE, (float *)projectionMatrix);

        glm_mat4_identity(modelMatrix);
        glm_translate(modelMatrix, lightPosition);

        // ADD THIS: Scale the light cube down to 20% of its normal size
        vec3 lightScale = {0.2f, 0.2f, 0.2f};
        glm_scale(modelMatrix, lightScale);

        glUniformMatrix4fv(lightModelLocation, 1, GL_FALSE, (float *)modelMatrix);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
    }

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteVertexArrays(1, &lightVao);
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
void frameBufferSizeCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}
char *readFile(const char *filePath)
{
    FILE *file = fopen(filePath, "r");
    if (!file)
    {
        perror("File");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = calloc(fileSize + 1, 1);
    if (!buffer)
    {
        perror("Memory");
        fclose(file);
        return NULL;
    }

    fread(buffer, sizeof(char), fileSize, file);
    buffer[fileSize] = '\0';
    fclose(file);

    return buffer;
}
Shaders *createShaderProgram(const char *vertexFilePath, const char *fragmentFilePath)
{
    Shaders *shaders = calloc(1, sizeof(Shaders));
    if (!shaders)
    {
        perror("Shaders");
        return NULL;
    }

    const char *vertexFile = readFile(vertexFilePath),
               *fragmentFile = readFile(fragmentFilePath);

    if (!vertexFile || !fragmentFile)
    {
        perror("Shader files");
        free(shaders);
        return NULL;
    }

    unsigned vertexShader = glCreateShader(GL_VERTEX_SHADER),
             fragmentShader = glCreateShader(GL_FRAGMENT_SHADER),
             shaderProgram = glCreateProgram();

    glShaderSource(vertexShader, 1, &vertexFile, NULL);
    glCompileShader(vertexShader);

    int vertexCompileStatus;
    char vertexCompileLog[BUFSIZ];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertexCompileStatus);
    if (!vertexCompileStatus)
    {
        glGetShaderInfoLog(vertexShader, BUFSIZ, NULL, vertexCompileLog);
        printf("Vertex compile error: %s", vertexCompileLog);
        free(shaders);
        return NULL;
    }

    glShaderSource(fragmentShader, 1, &fragmentFile, NULL);
    glCompileShader(fragmentShader);

    int fragmentCompileStatus;
    char fragmentCompileLog[BUFSIZ];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragmentCompileStatus);
    if (!fragmentCompileStatus)
    {
        glGetShaderInfoLog(fragmentShader, BUFSIZ, NULL, fragmentCompileLog);
        printf("Fragment compile error: %s", fragmentCompileLog);
        free(shaders);
        return NULL;
    }

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int programLinkStatus;
    char programLinkLog[BUFSIZ];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &programLinkStatus);
    if (!programLinkStatus)
    {
        glGetProgramInfoLog(shaderProgram, BUFSIZ, NULL, programLinkLog);
        printf("Program Link error: %s", programLinkLog);
        free(shaders);
        return NULL;
    }

    shaders->fragmentFilePath = fragmentFilePath;
    shaders->vertexFilePath = vertexFilePath;
    shaders->vertexShader = vertexShader;
    shaders->fragmentShader = fragmentShader;
    shaders->ShaderProgram = shaderProgram;

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    free((void *)vertexFile);
    free((void *)fragmentFile);

    return shaders;
}
Camera *createCamera(GLFWwindow *window)
{
    Camera *camera = malloc(sizeof(Camera));
    if (!camera)
    {
        perror("Camera Memory");
        return NULL;
    }

    camera->firstMouse = true;
    camera->lastX = WINDOW_WIDTH / 2;
    camera->lastY = WINDOW_HEIGHT / 2;
    camera->pitch = 0.0f;
    camera->zoom = 45.0f;
    camera->yaw = -90.0f;

    vec3 *position = calloc(1, sizeof(vec3)),
         *front = calloc(1, sizeof(vec3)),
         *up = calloc(1, sizeof(vec3));

    if (!position || !front || !up)
    {
        perror("Camera positions memory");
        return NULL;
    }

    (*up)[1] = 1.0f;
    (*position)[2] = -3.0f;
    (*front)[2] = -1.0f;

    camera->position = position;
    camera->front = front;
    camera->up = up;

    glfwSetWindowUserPointer(window, camera);
    return camera;
}
void CameraAngleMovement(GLFWwindow *window, mat4 *viewMatrix)
{
    if (!window)
    {
        perror("Window error");
        return;
    }

    Camera *camera = (Camera *)glfwGetWindowUserPointer(window);
    if (!camera)
    {
        perror("Non-existent camera");
        return;
    }

    vec3 directionalAxis = {
        cos(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch)), // changed sin to cos
        sin(glm_rad(camera->pitch)),
        sin(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch)) // changed sin to cos
    };

    glm_normalize_to(directionalAxis, (float *)camera->front);
    glm_mat4_identity((vec4 *)viewMatrix);

    vec3 center;
    glm_vec3_add((float *)camera->position, (float *)camera->front, center);
    glm_lookat((float *)camera->position, center, (float *)camera->up, (vec4 *)viewMatrix);
}
void KeyBoardInput(GLFWwindow *window, float deltaTime)
{
    if (!window)
    {
        perror("Window");
        return;
    }

    Camera *camera = (Camera *)glfwGetWindowUserPointer(window);
    if (!camera)
    {
        perror("Camera not set");
        return;
    }

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    float cameraSpeed = deltaTime * 2.5f;

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
        cameraSpeed *= 2;

    vec3 velocity;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GL_TRUE)
    {
        glm_vec3_scale((float *)camera->front, cameraSpeed, velocity);
        glm_vec3_add((float *)camera->position, velocity, (float *)camera->position);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        glm_vec3_scale((float *)camera->front, cameraSpeed, velocity);
        glm_vec3_sub((float *)camera->position, velocity, (float *)camera->position);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        vec3 rightDirection;
        glm_cross((float *)camera->front, (float *)camera->up, rightDirection);
        glm_normalize(rightDirection);

        glm_vec3_scale(rightDirection, cameraSpeed, velocity);
        glm_vec3_sub((float *)camera->position, velocity, (float *)camera->position);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        vec3 rightDirection;
        glm_cross((float *)camera->front, (float *)camera->up, rightDirection);
        glm_normalize(rightDirection);

        glm_vec3_scale(rightDirection, cameraSpeed, velocity);
        glm_vec3_add((float *)camera->position, velocity, (float *)camera->position);
    }
}
void MouseInput(GLFWwindow *window, double xPosition, double yPosition)
{
    if (!window)
    {
        perror("Non-existant Window");
        return;
    }

    Camera *camera = (Camera *)glfwGetWindowUserPointer(window);
    if (!camera)
    {
        perror("Non-existent Camera");
        return;
    }

    if (camera->firstMouse)
    {
        camera->lastX = xPosition;
        camera->lastY = yPosition;
        camera->firstMouse = false;
        return;
    }

    double sensitivity = 0.01f;
    double xOffset = xPosition - camera->lastX,
           yOffset = -(yPosition - camera->lastY);

    xOffset *= sensitivity;
    yOffset *= sensitivity;

    camera->yaw += xOffset;
    camera->pitch += yOffset;

    if (camera->pitch > 89.0f)
        camera->pitch = 89.0f;
    if (camera->pitch < -89.0f)
        camera->pitch = -89.0f;

    camera->lastX = xPosition;
    camera->lastY = yPosition;
}
void ScrollInput(GLFWwindow *window, double xOffset, double yOffset)
{
    if (!window)
    {
        perror("Window non-existent error");
        return;
    }

    Camera *camera = (Camera *)glfwGetWindowUserPointer(window);
    if (!camera)
    {
        perror("Camera non-existent");
        return;
    }

    camera->zoom -= (float)yOffset;
    if (camera->zoom > 45.0f)
        camera->zoom = 45.0f;
    if (camera->zoom < 1.0f)
        camera->zoom = 1.0f;
}