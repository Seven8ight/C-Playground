#include "../../../Header.h"

const int WINDOW_WIDTH = 1000,
          WINDOW_HEIGHT = 800;

typedef struct
{
    bool firstMouse;
    float yaw, pitch, zoom;
    int lastX, lastY;
    vec3 *Up, *Front, *Position;
} ViewCamera;

typedef struct
{
    unsigned int vertexShader, fragmentShader, shaderProgram;
    const char *vertexFile, *fragmentFile;
} ProgramShaders;

void GLFWInit();
void frame_buffer_size_callback(GLFWwindow *window, int width, int height);
void processKeyboardInput(GLFWwindow *window, float deltaTime);
void processMouseInput(GLFWwindow *window, double xpos, double ypos);
void processScrollInput(GLFWwindow *window, double xoffset, double yoffset);
void handleCameraMovement(GLFWwindow *window, mat4 matrix);
ViewCamera *createCamera();
char *readFile(const char *filePath);
ProgramShaders *createShaders(const char *vertexFilePath, const char *fragmentFilePath);
void glfw_error_callback(int error, const char *description);

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
        // Positions          // Colors (RGB)      // Normals
        // Back face (Red)
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,

        // Front face (Green)
        -0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,

        // Left face (Blue)
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,

        // Right face (Yellow)
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,

        // Bottom face (Cyan)
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,

        // Top face (Magenta)
        -0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f};
    vec3 cubePositions[] = {
        {0.0f, 0.0f, 0.0f},
        {2.0f, 0.0f, -15.0f},
        {-1.5f, -0.2f, -2.5f},
        {-3.8f, -0.0f, -12.3f}},
         lightPosition = {1.2f, 1.0f, 2.0f};

    unsigned int vao, vbo, lightVao;
    glGenVertexArrays(1, &vao);
    glGenVertexArrays(1, &lightVao);

    glGenBuffers(1, &vbo);

    // --- SETUP OBJECT VAO ---
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 9, NULL);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 9, (void *)(sizeof(float) * 3));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 9, (void *)(sizeof(float) * 6));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    // --- SETUP LIGHT VAO ---
    glBindVertexArray(lightVao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Light only cares about position data (index 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 9, NULL);
    glEnableVertexAttribArray(0);

    ViewCamera *camera = createCamera();
    if (!camera)
    {
        perror("Camera memory error");
        return -2;
    }

    glfwSetWindowUserPointer(window, camera);

    char vertexFilePath[] = "Chapters/Chapter 2/10 - Materials/Shaders/Material.vert",
         fragmentFilePath[] = "Chapters/Chapter 2/10 - Materials/Shaders/Material.frag";

    char lightVertexFilePath[] = "Chapters/Chapter 2/10 - Materials/Shaders/Light.vert",
         lightFragmentFilePath[] = "Chapters/Chapter 2/10 - Materials/Shaders/Light.frag";

    ProgramShaders *shaders = createShaders(vertexFilePath, fragmentFilePath);
    ProgramShaders *lightShaders = createShaders(lightVertexFilePath, lightFragmentFilePath);
    if (!shaders)
    {
        perror("Shaders");
        return -3;
    }
    if (!lightShaders)
    {
        perror("Light shaders");
        return -3;
    }

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    float deltaTime = 0.0f,
          lastFrame = 0.0f;

    while (glfwWindowShouldClose(window) == false)
    {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        lightPosition[0] = sin(currentFrame) * 2.0f;
        lightPosition[2] = cos(currentFrame) * 2.0f;

        mat4 viewMatrix = GLM_MAT4_IDENTITY_INIT,
             projectionMatrix = GLM_MAT4_IDENTITY_INIT;

        // 1. Calculate inputs and matrices
        processKeyboardInput(window, deltaTime);
        handleCameraMovement(window, viewMatrix);
        glm_perspective(glm_rad(camera->zoom), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f, projectionMatrix);

        // ---------------------------------------------------------
        // 2. RENDER THE OBJECT CUBES
        // ---------------------------------------------------------
        glBindVertexArray(vao);
        glUseProgram(shaders->shaderProgram);

        unsigned int viewUniformLocation = glGetUniformLocation(shaders->shaderProgram, "viewPosition");
        glUniform3fv(viewUniformLocation, 1, (float *)camera->Position);

        unsigned int modelLocation = glGetUniformLocation(shaders->shaderProgram, "modelMatrix"),
                     viewLocation = glGetUniformLocation(shaders->shaderProgram, "viewMatrix"),
                     projectionLocation = glGetUniformLocation(shaders->shaderProgram, "projectionMatrix"),
                     objectColorLocation = glGetUniformLocation(shaders->shaderProgram, "objectColor");

        // Lighting for each object
        unsigned int ambient = glGetUniformLocation(shaders->shaderProgram, "material.ambient"),
                     diffuse = glGetUniformLocation(shaders->shaderProgram, "material.diffuse"),
                     specular = glGetUniformLocation(shaders->shaderProgram, "material.specular"),
                     shininess = glGetUniformLocation(shaders->shaderProgram, "material.shininess");

        unsigned int lightUniformAmbient = glGetUniformLocation(shaders->shaderProgram, "light.ambient"),
                     lightUniformDiffuse = glGetUniformLocation(shaders->shaderProgram, "light.diffuse"),
                     lightUniformSpecular = glGetUniformLocation(shaders->shaderProgram, "light.specular"),
                     lightUniformPosition = glGetUniformLocation(shaders->shaderProgram, "light.position");

        glUniform3f(ambient, 1.0f, 0.5f, 0.31f);
        glUniform3f(diffuse, 1.0f, 0.5f, 0.31f);
        glUniform3f(specular, 0.5, 0.5, 0.5f);
        glUniform1f(shininess, 32.0f);

        vec3 lightColor;
        lightColor[0] = sin(glfwGetTime() * 1.7);
        lightColor[1] = sin(glfwGetTime() * 2.5);
        lightColor[2] = sin(glfwGetTime() * 0.6);

        vec3 ambientFactor = {0.5, 0.2, 0.7},
             diffuseFactor = {0.7, 0.1, 0.4};

        vec3 ambientResult, diffuseResult;
        glm_vec3_mul(ambientFactor, lightColor, ambientResult);
        glm_vec3_mul(diffuseFactor, lightColor, diffuseResult);

        glUniform3f(lightUniformAmbient, ambientResult[0], ambientResult[1], ambientResult[2]);
        glUniform3f(lightUniformDiffuse, diffuseResult[0], diffuseResult[1], diffuseResult[2]);
        glUniform3f(lightUniformSpecular, 1.0f, 1.0f, 1.0f);
        glUniform3f(lightUniformPosition, lightPosition[0], lightPosition[1], lightPosition[2]);

        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, (const float *)viewMatrix);
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, (const float *)projectionMatrix);

        glUniform3f(objectColorLocation, 1.0f, 0.5f, 0.31f);

        for (int i = 0; i < sizeof(cubePositions) / sizeof(vec3); i++)
        {
            mat4 modelMatrix = GLM_MAT4_IDENTITY_INIT;
            float rotationAngle = glfwGetTime();
            vec3 rotationAxis = {0.2f, 0.3f, 1.0f};

            glm_translate(modelMatrix, cubePositions[i]);
            glm_rotate(modelMatrix, glm_rad(rotationAngle), rotationAxis);

            // Inversion due to translation - more suitable for non-uniform scaling in most scenarios otherwise normal calculations
            mat4 inverseModel;
            mat3 normalMatrix;
            glm_mat4_inv(modelMatrix, inverseModel);
            glm_mat4_transpose(inverseModel);
            glm_mat4_pick3(inverseModel, normalMatrix);

            unsigned int normalMatrixLocation = glGetUniformLocation(shaders->shaderProgram, "normalsInverse");
            glUniformMatrix3fv(normalMatrixLocation, 1, GL_FALSE, (const float *)normalMatrix);

            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, (const float *)modelMatrix);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // ---------------------------------------------------------
        // 3. RENDER THE LIGHT CUBE
        // ---------------------------------------------------------
        glBindVertexArray(lightVao);
        glUseProgram(lightShaders->shaderProgram);

        unsigned int lightModelLoc = glGetUniformLocation(lightShaders->shaderProgram, "modelMatrix"),
                     lightViewLoc = glGetUniformLocation(lightShaders->shaderProgram, "viewMatrix"),
                     lightProjLoc = glGetUniformLocation(lightShaders->shaderProgram, "projectionMatrix"),
                     lightColorUniform = glGetUniformLocation(lightShaders->shaderProgram, "LightColor");

        mat4 lightModelMatrix = GLM_MAT4_IDENTITY_INIT;
        glm_translate(lightModelMatrix, lightPosition);
        vec3 scalingVector = {0.2f, 0.2f, 0.2f};
        glm_scale(lightModelMatrix, scalingVector);

        glUniformMatrix4fv(lightViewLoc, 1, GL_FALSE, (const float *)viewMatrix);
        glUniformMatrix4fv(lightProjLoc, 1, GL_FALSE, (const float *)projectionMatrix);
        glUniformMatrix4fv(lightModelLoc, 1, GL_FALSE, (const float *)lightModelMatrix);

        glUniform3f(lightColorUniform, lightColor[0], lightColor[1], lightColor[2]);

        glDrawArrays(GL_TRIANGLES, 0, 36);

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
void glfw_error_callback(int error, const char *description)
{
    fprintf(stderr, "GLFW ERROR %d: %s\n", error, description);
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

    vec3 *cameraPosition = calloc(1, sizeof(vec3));
    vec3 *cameraFront = calloc(1, sizeof(vec3));
    vec3 *cameraUp = calloc(1, sizeof(vec3));

    if (!cameraPosition || !cameraFront || !cameraUp)
    {
        perror("Memory");
        free(camera);

        if (cameraPosition)
            free(cameraPosition);
        if (cameraFront)
            free(cameraFront);
        if (cameraUp)
            free(cameraUp);
        return NULL;
    }

    // Default OpenGL front is -Z
    (*cameraFront)[2] = -1.0f;
    // Default Up vector is +Y
    (*cameraUp)[1] = 1.0f;
    (*cameraPosition)[2] = 3.0f;

    camera->Front = cameraFront;
    camera->Position = cameraPosition;
    camera->Up = cameraUp;

    return camera;
}
void handleCameraMovement(GLFWwindow *window, mat4 viewMatrix)
{
    ViewCamera *camera = glfwGetWindowUserPointer(window);
    if (!camera)
        return;

    vec3 direction = {
        cos(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch)),
        sin(glm_rad(camera->pitch)),
        sin(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch)),
    };

    glm_normalize_to(direction, (float *)camera->Front);
    glm_mat4_identity(viewMatrix);

    vec3 center;
    glm_vec3_add((float *)camera->Position, (float *)camera->Front, center);
    glm_lookat((float *)camera->Position, center, (float *)camera->Up, viewMatrix);
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
void processKeyboardInput(GLFWwindow *window, float deltaTime)
{
    ViewCamera *camera = (ViewCamera *)glfwGetWindowUserPointer(window);

    if (!camera)
    {
        perror("Camera error");
        return;
    }

    if (!window)
    {
        perror("Window error");
        return;
    }

    float cameraSpeed = deltaTime * 2.0f; // Adjusted for reasonable movement speed

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        cameraSpeed *= 2;

    vec3 velocity;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        glm_vec3_scale((float *)camera->Front, cameraSpeed, velocity);
        glm_vec3_add((float *)camera->Position, velocity, (float *)camera->Position);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        glm_vec3_scale((float *)camera->Front, cameraSpeed, velocity);
        glm_vec3_sub((float *)camera->Position, velocity, (float *)camera->Position);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        vec3 rightDirection;
        glm_cross((float *)camera->Front, (float *)camera->Up, rightDirection);
        glm_normalize(rightDirection);

        glm_vec3_scale(rightDirection, cameraSpeed, velocity);
        glm_vec3_sub((float *)camera->Position, velocity, (float *)camera->Position);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        vec3 rightDirection;
        glm_cross((float *)camera->Front, (float *)camera->Up, rightDirection);
        glm_normalize(rightDirection);

        glm_vec3_scale(rightDirection, cameraSpeed, velocity);
        glm_vec3_add((float *)camera->Position, velocity, (float *)camera->Position);
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