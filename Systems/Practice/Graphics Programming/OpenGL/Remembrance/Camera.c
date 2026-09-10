#include "../Header.h"

const int WINDOW_WIDTH = 1000,
          WINDOW_HEIGHT = 800;

typedef struct
{
    bool firstMouse;
    float yaw, pitch, zoom;
    int lastX, lastY;
} ViewCamera;

typedef struct
{
    unsigned int vertexShader, fragmentShader, shaderProgram;
    const char *vertexFile, *fragmentFile;
} ProgramShaders;

void GLFWInit();
void frame_buffer_size_callback(GLFWwindow *window, int width, int height);
void processKeyboardInput(GLFWwindow *window, float deltaTime, vec3 cameraPos, vec3 cameraFront, vec3 cameraUp);
void processMouseInput(GLFWwindow *window, double xpos, double ypos);
void processScrollInput(GLFWwindow *window, double xoffset, double yoffset);
void handleCameraMovement(GLFWwindow *window, vec3 cameraPosition, vec3 cameraFront, vec3 cameraUp, mat4 matrix);
ViewCamera *createCamera();
char *readFile(const char *filePath);
ProgramShaders *createShaders(const char *vertexFilePath, const char *fragmentFilePath);

void glfw_error_callback(int error, const char *description)
{
    fprintf(stderr, "ACTUAL GLFW ERROR %d: %s\n", error, description);
}

int main(void)
{
    GLFWInit();
    glfwSetErrorCallback(glfw_error_callback);

    char windowTitle[] = "System";
    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, windowTitle, NULL, NULL);
    if (!window)
    {
        perror("Window");
        return -1;
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        perror("GLAD");
        return -10;
    }

    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    glfwSetFramebufferSizeCallback(window, frame_buffer_size_callback);
    glViewport(0, 0, fbWidth, fbHeight);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, processMouseInput);
    glfwSetScrollCallback(window, processScrollInput);

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
    vec3 cubePositions[] = {
        {0.0f, 0.0f, 0.0f},
        {2.0f, 0.0f, -15.0f},
        {-1.5f, -0.2f, -2.5f},
        {-3.8f, -0.0f, -12.3f},
        {2.4f, -0.4f, -3.5f},
        {-1.7f, 0.0f, -7.5f},
        {1.3f, 0.0f, -2.5f},
        {1.5f, 0.0f, -2.5f},
        {1.5f, 0.2f, -1.5f},
        {-1.3f, 0.0f, -1.5f}};

    unsigned int vao, vbo, ebo, lightVao;
    glGenVertexArrays(1, &vao);
    glGenVertexArrays(1, &lightVao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof indices, indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, sizeof(float), GL_FLOAT, GL_FALSE, sizeof(float) * 6, NULL);
    glVertexAttribPointer(1, sizeof(float), GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void *)(sizeof(float) * 3));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    ViewCamera *camera = createCamera();
    if (!camera)
    {
        perror("Camera memory error");
        return -2;
    }

    glfwSetWindowUserPointer(window, camera);

    char vertexFilePath[] = "Chapters/Chapter 2/09 - Colors/ColorsVert.vert",
         fragmentFilePath[] = "Chapters/Chapter 2/09 - Colors/ColorsFrag.frag";
    ProgramShaders *shaders = createShaders(vertexFilePath, fragmentFilePath);
    if (!shaders)
    {
        perror("Shaders");
        return -3;
    }

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.4f, 0.2f, 0.75f, 1.0f);

    vec3 cameraPosition = {0.0f, 0.0f, 3.0f},
         cameraFront = {0.0f, 0.0f, -1.0f},
         cameraUp = {0.0f, 1.0f, 0.0f},
         cameraDirection = {},
         cameraTarget = {},
         viewTranslation = {0.0f, 0.0f, -3.0f};

    float deltaTime = 0.0f,
          lastFrame = 0.0f;

    while (glfwWindowShouldClose(window) == false)
    {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float currentFrame = glfwGetTime(),
              deltaTime = currentFrame - lastFrame,
              lastFrame = currentFrame;

        mat4 viewMatrix = GLM_MAT4_IDENTITY_INIT,
             projectionMatrix = GLM_MAT4_IDENTITY_INIT;

        processKeyboardInput(window, deltaTime, cameraPosition, cameraFront, cameraUp);
        handleCameraMovement(window, cameraPosition, cameraFront, cameraUp, viewMatrix);

        glUseProgram(shaders->shaderProgram);

        unsigned int modelLocation = glGetUniformLocation(shaders->shaderProgram, "modelMatrix");
        unsigned int viewLocation = glGetUniformLocation(shaders->shaderProgram, "viewMatrix");
        unsigned int projectionLocation = glGetUniformLocation(shaders->shaderProgram, "projectionMatrix");

        glm_perspective(glm_rad(camera->zoom), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f, projectionMatrix);

        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, (const float *)viewMatrix);
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, (const float *)projectionMatrix);

        for (int i = 0; i < sizeof(cubePositions) / sizeof(vec3); i++)
        {
            mat4 modelMatrix = GLM_MAT4_IDENTITY_INIT;
            float rotationAngle = glfwGetTime();
            vec3 rotationAxis = {0.2f, 0.3f, 1.0f};

            glm_translate(modelMatrix, cubePositions[i]);
            glm_rotate(modelMatrix, glm_rad(rotationAngle), rotationAxis);

            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, (const float *)modelMatrix);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);
        }

        glfwSwapBuffers(window);
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
void frame_buffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}
ViewCamera *createCamera()
{
    ViewCamera *camera = malloc(sizeof(ViewCamera));
    if (!camera)
    {
        perror("Camera");
        return NULL;
    }

    camera->zoom = 45.0f;
    camera->yaw = -90.0f;
    camera->pitch = 0.0f;
    camera->lastX = WINDOW_WIDTH / 2;
    camera->lastY = WINDOW_HEIGHT / 2;
    camera->firstMouse = true;

    return camera;
}
void handleCameraMovement(GLFWwindow *window, vec3 cameraPosition, vec3 cameraFront, vec3 cameraUp, mat4 viewMatrix)
{
    ViewCamera *camera = glfwGetWindowUserPointer(window);
    if (!camera)
        return;

    vec3 direction = {
        cos(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch)),
        sin(glm_rad(camera->pitch)),
        sin(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch)),
    };

    glm_normalize_to(direction, cameraFront);
    glm_mat4_identity(viewMatrix);

    vec3 center;
    glm_vec3_add(cameraPosition, cameraFront, center);
    glm_lookat(cameraPosition, center, cameraUp, viewMatrix);
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

    char *buffer = malloc(fileSize + 1);
    if (!buffer)
    {
        perror("Buffer memory");
        return NULL;
    }

    fread(buffer, sizeof(char), fileSize, file);
    buffer[fileSize] = '\0';
    fclose(file);

    return buffer;
}
ProgramShaders *createShaders(const char *vertexFilePath, const char *fragmentFilePath)
{
    ProgramShaders *shaders = malloc(sizeof(ProgramShaders));
    if (!shaders)
    {
        perror("Shaders memory");
        return NULL;
    }

    const char *vertexFile = readFile(vertexFilePath),
               *fragmentFile = readFile(fragmentFilePath);

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER),
                 fragmentShader = glCreateShader(GL_FRAGMENT_SHADER),
                 programShader = glCreateProgram();

    if (!vertexFile || !fragmentFile)
    {
        perror("Shader files");
        free(shaders);
        return NULL;
    }

    glShaderSource(vertexShader, 1, &vertexFile, NULL);
    glCompileShader(vertexShader);

    int vertexSuccess;
    char vertexInfoLog[BUFSIZ];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertexSuccess);
    if (!vertexSuccess)
    {
        glGetShaderInfoLog(vertexShader, BUFSIZ, NULL, vertexInfoLog);
        printf("Vertex shader compile error; %s", vertexInfoLog);
        free(shaders);
        return NULL;
    }

    glShaderSource(fragmentShader, 1, &fragmentFile, NULL);
    glCompileShader(fragmentShader);

    int fragmentSuccess;
    char fragmentInfoLog[BUFSIZ];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragmentSuccess);
    if (!fragmentSuccess)
    {
        glGetShaderInfoLog(fragmentShader, BUFSIZ, NULL, fragmentInfoLog);
        printf("Fragment shader compile error; %s", fragmentInfoLog);
        free(shaders);
        return NULL;
    }

    glAttachShader(programShader, vertexShader);
    glAttachShader(programShader, fragmentShader);
    glLinkProgram(programShader);

    int linkSuccess;
    char programInfoLog[BUFSIZ];
    glGetProgramiv(programShader, GL_LINK_STATUS, &linkSuccess);
    if (!linkSuccess)
    {
        glGetProgramInfoLog(programShader, BUFSIZ, NULL, programInfoLog);
        printf("Shader link error: %s", programInfoLog);
        free(shaders);
        return NULL;
    }

    shaders->vertexShader = vertexShader;
    shaders->vertexFile = vertexFile;
    shaders->fragmentShader = fragmentShader;
    shaders->fragmentFile = fragmentFile;
    shaders->shaderProgram = programShader;

    free((void *)vertexFile);
    free((void *)fragmentFile);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaders;
}
void processKeyboardInput(GLFWwindow *window, float deltaTime, vec3 cameraPos, vec3 cameraFront, vec3 cameraUp)
{
    if (!window)
    {
        perror("Window error");
        return;
    }

    float cameraSpeed = deltaTime * .025f;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        cameraSpeed *= 2;

    vec3 velocity;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {

        glm_vec3_scale((float *)cameraFront, cameraSpeed, (float *)velocity);
        glm_vec3_add(cameraPos, velocity, (float *)cameraPos);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        glm_vec3_scale((float *)cameraFront, cameraSpeed, (float *)velocity);
        glm_vec3_sub((float *)cameraPos, velocity, (float *)cameraPos);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        vec3 rightDirection;

        glm_cross((float *)cameraFront, (float *)cameraUp, rightDirection);
        glm_normalize(rightDirection);

        glm_vec3_scale(rightDirection, cameraSpeed, velocity);
        glm_vec3_sub((float *)cameraPos, velocity, (float *)cameraPos);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        vec3 rightDirection;

        glm_cross((float *)cameraFront, (float *)cameraUp, rightDirection);
        glm_normalize(rightDirection);

        glm_vec3_scale(rightDirection, cameraSpeed, velocity);
        glm_vec3_add((float *)cameraPos, velocity, (float *)cameraPos);
    }
}
void processMouseInput(GLFWwindow *window, double xpos, double ypos)
{
    ViewCamera *camera = (ViewCamera *)glfwGetWindowUserPointer(window);

    if (!camera)
    {
        perror("Camera not attached");
        return;
    }

    if (camera->firstMouse)
    {
        camera->lastX = xpos;
        camera->lastY = ypos;
        camera->firstMouse = false;
        return;
    }

    double xOffset = xpos - camera->lastX,
           yOffset = -(ypos - camera->lastY);

    const float sensitivity = 0.01f;
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    camera->yaw += xOffset;
    camera->pitch += yOffset;

    if (camera->pitch > 89.0f)
        camera->pitch = 89.0f;
    if (camera->pitch < -89.0f)
        camera->pitch = -89.0f;

    camera->lastX = xpos;
    camera->lastY = ypos;
}
void processScrollInput(GLFWwindow *window, double xoffset, double yoffset)
{
    ViewCamera *camera = (ViewCamera *)glfwGetWindowUserPointer(window);
    if (!camera)
    {
        perror("Camere not attached");
        return;
    }

    camera->zoom -= (float)yoffset;
    if (camera->zoom > 45.0f)
        camera->zoom = 45.0f;
    if (camera->zoom < 1.0f)
        camera->zoom = 1.0f;
}