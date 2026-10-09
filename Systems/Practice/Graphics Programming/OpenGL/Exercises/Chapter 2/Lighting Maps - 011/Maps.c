#include "../../../Header.h"

static int g_fbWidth = 800, g_fbHeight = 600;

typedef enum
{
    VERTEX_SHADER,
    FRAGMENT_SHADER
} ShaderType;

typedef struct
{
    char const *vertexFilePath, *fragmentFilePath;
    unsigned int VertexShader, FragmentShader, ShaderProgram;
} Shaders;

typedef struct
{
    bool firstMouse;
    float yaw, pitch, xPosition, yPosition, zoom;
    vec3 up, front, position;
} Camera;

void GLFWInit(void);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void glfw_error_callback(int code, const char *description);
Camera *createCamera(GLFWwindow *window);
char *readFile(const char *filePath);
unsigned int loadShader(ShaderType type, const char *shaderFilePath);
Shaders *createShaders(const char *vertexFilePath, const char *fragmentFilePath);
void KeyboardInput(GLFWwindow *window, float deltaTime);
void MouseInput(GLFWwindow *window, double xPos, double yPos);
void ScrollInput(GLFWwindow *window, double xPos, double yPos);
int loadTexture(char *texturePath);
void GetViewMatrix(Camera *camera, mat4 view);

int main(void)
{
    glfwSetErrorCallback(glfw_error_callback);
    GLFWInit();

    char windowTitle[] = "Lighting Maps Exercise";

    GLFWwindow *window = glfwCreateWindow(g_fbWidth, g_fbHeight, windowTitle, NULL, NULL);
    if (!window)
    {
        perror("Window");
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        perror("GLAD error");
        return -2;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    Camera *camera = createCamera(window);
    if (camera == NULL)
    {
        perror("Camera");
        return -2;
    }

    glfwSetCursorPosCallback(window, MouseInput);
    glfwSetScrollCallback(window, ScrollInput);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwGetFramebufferSize(window, &g_fbWidth, &g_fbHeight);
    glViewport(0, 0, g_fbWidth, g_fbHeight);
    float aspectRatio = (float)g_fbWidth / (float)g_fbHeight;

    float vertices[] = {
        // Positions          // Texture Coords  // Normals
        // Back face
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,

        // Front face
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

        // Left face
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,

        // Right face
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

        // Bottom face
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,

        // Top face
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f};
    vec3 cubePositions[] = {
        {0.0f, 0.0f, 0.0f},
        {2.0f, 0.0f, -15.0f},
        {-1.5f, -0.2f, -2.5f},
        {-3.8f, 0.0f, -12.3f}},
         lightPosition = {1.2f, 1.0f, 2.0f};

    unsigned int vbo, worldVao, lightVao, diffuseTextureId, specularTextureId;
    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &worldVao);
    glGenVertexArrays(1, &lightVao);

    glBindVertexArray(worldVao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *)(sizeof(float) * 3));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *)(sizeof(float) * 5));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindVertexArray(lightVao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *)0);
    glEnableVertexAttribArray(0);

    glGenTextures(1, &diffuseTextureId);
    glGenTextures(1, &specularTextureId);

    glBindTexture(GL_TEXTURE_2D, diffuseTextureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

    int diffuseTexture = loadTexture("Exercises/Chapter 2/Lighting Maps - 011/Images/Image2.jpg"),
        specularTexture = loadTexture("Exercises/Chapter 2/Lighting Maps - 011/Images/Image3.png"),
        emissionTexture = loadTexture("Exercises/Chapter 2/Lighting Maps - 011/Images/EmissionMap.png");

    if (diffuseTexture == -1 || specularTexture == -1 || emissionTexture == -1)
    {
        perror("Texture error");
        return -3;
    }

    Shaders *worldShaders = createShaders("Exercises/Chapter 2/Lighting Maps - 011/Shaders/Maps.vert", "Exercises/Chapter 2/Lighting Maps - 011/Shaders/Maps.frag"),
            *lightShaders = createShaders("Exercises/Chapter 2/Lighting Maps - 011/Shaders/Light.vert", "Exercises/Chapter 2/Lighting Maps - 011/Shaders/Light.frag");

    if (worldShaders == NULL || lightShaders == NULL)
    {
        perror("Shaders");
        return -4;
    }

    glEnable(GL_DEPTH_TEST);
    float deltaTime = 0.0f, lastFrame = 0.0f;

    mat4 projectionMatrix = GLM_MAT4_IDENTITY_INIT,
         viewMatrix = GLM_MAT4_IDENTITY_INIT;

    unsigned int worldModelMatrix = glGetUniformLocation(worldShaders->ShaderProgram, "modelMatrix"),
                 worldViewMatrix = glGetUniformLocation(worldShaders->ShaderProgram, "viewMatrix"),
                 worldProjectionMatrix = glGetUniformLocation(worldShaders->ShaderProgram, "projectionMatrix"),
                 worldNormalsMatrix = glGetUniformLocation(worldShaders->ShaderProgram, "normalsMatrix");

    unsigned int lightModelMatrixUniform = glGetUniformLocation(lightShaders->ShaderProgram, "modelMatrix"),
                 lightViewMatrix = glGetUniformLocation(lightShaders->ShaderProgram, "viewMatrix"),
                 lightProjectionMatrix = glGetUniformLocation(lightShaders->ShaderProgram, "projectionMatrix");

    unsigned int materialDiffuseUniform = glGetUniformLocation(worldShaders->ShaderProgram, "material.diffuse"),
                 materialSpecularUniform = glGetUniformLocation(worldShaders->ShaderProgram, "material.specular"),
                 materialEmissionMap = glGetUniformLocation(worldShaders->ShaderProgram, "material.emission"),
                 materialShininessUniform = glGetUniformLocation(worldShaders->ShaderProgram, "material.shininess");

    unsigned int lightingAmbientUniform = glGetUniformLocation(worldShaders->ShaderProgram, "light.ambient"),
                 lightingDiffuseUniform = glGetUniformLocation(worldShaders->ShaderProgram, "light.diffuse"),
                 lightingSpecularUniform = glGetUniformLocation(worldShaders->ShaderProgram, "light.specular"),
                 lightingPositionUniform = glGetUniformLocation(worldShaders->ShaderProgram, "light.position"),
                 cameraPositionUniform = glGetUniformLocation(worldShaders->ShaderProgram, "CameraPosition");

    vec3 lightAmbientValues = {0.1f, 0.1f, 0.1f},
         lightDiffuseValues = {0.8f, 0.8f, 0.8f},
         lightSpecularValues = {1.0f, 1.0f, 1.0f};

    while (glfwWindowShouldClose(window) == false)
    {
        glfwPollEvents();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        KeyboardInput(window, deltaTime);

        glBindVertexArray(worldVao);
        glUseProgram(worldShaders->ShaderProgram);
        glm_perspective(glm_rad(camera->zoom), aspectRatio, 0.01f, 100.0f, projectionMatrix);
        GetViewMatrix(camera, viewMatrix);

        glUniformMatrix4fv(worldProjectionMatrix, 1, GL_FALSE, (const GLfloat *)projectionMatrix);
        glUniformMatrix4fv(worldViewMatrix, 1, GL_FALSE, (const GLfloat *)viewMatrix);
        glUniform3f(cameraPositionUniform, camera->position[0], camera->position[1], camera->position[2]);

        for (size_t i = 0; i < sizeof(cubePositions) / sizeof(vec3); i++)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, diffuseTexture);
            glUniform1i(materialDiffuseUniform, 0);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, specularTexture);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, emissionTexture);
            glUniform1i(materialSpecularUniform, 1);
            glUniform1i(materialEmissionMap, 2);
            glUniform1f(materialShininessUniform, 32.0f);

            mat4 modelMatrix = GLM_MAT4_IDENTITY_INIT;

            glm_translate(modelMatrix, cubePositions[i]);
            glUniformMatrix4fv(worldModelMatrix, 1, GL_FALSE, (const GLfloat *)modelMatrix);

            mat3 normals;
            glm_mat4_pick3(modelMatrix, normals);
            glm_mat3_inv(normals, normals);
            glm_mat3_transpose(normals);
            glUniformMatrix3fv(worldNormalsMatrix, 1, GL_FALSE, (const GLfloat *)normals);

            glUniform3f(lightingAmbientUniform, lightAmbientValues[0], lightAmbientValues[1], lightAmbientValues[2]);
            glUniform3f(lightingDiffuseUniform, lightDiffuseValues[0], lightDiffuseValues[1], lightDiffuseValues[2]);
            glUniform3f(lightingSpecularUniform, lightSpecularValues[0], lightSpecularValues[1], lightSpecularValues[2]);
            glUniform3f(lightingPositionUniform, lightPosition[0], lightPosition[1], lightPosition[2]);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glBindVertexArray(lightVao);
        glUseProgram(lightShaders->ShaderProgram);
        glm_perspective(glm_rad(camera->zoom), (float)(g_fbWidth / g_fbHeight), 0.01f, 100.0f, projectionMatrix);
        GetViewMatrix(camera, viewMatrix);

        glUniformMatrix4fv(lightProjectionMatrix, 1, GL_FALSE, (const GLfloat *)projectionMatrix);
        glUniformMatrix4fv(lightViewMatrix, 1, GL_FALSE, (const GLfloat *)viewMatrix);

        mat4 lightModelMatrix = GLM_MAT4_IDENTITY_INIT;
        glm_translate(lightModelMatrix, lightPosition);
        glm_scale_uni(lightModelMatrix, 0.2f);
        glUniformMatrix4fv(lightModelMatrixUniform, 1, GL_FALSE, (const GLfloat *)lightModelMatrix);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
    }

    free(worldShaders);
    free(lightShaders);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &worldVao);
    glDeleteVertexArrays(1, &lightVao);
    glfwTerminate();
    return 0;
}

void GLFWInit(void)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
}

void glfw_error_callback(int code, const char *description)
{
    printf("Error\nCode:%d\nDescription:%s", code, description);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    (void)window;
    g_fbWidth = width;
    g_fbHeight = height;
    glViewport(0, 0, width, height);
}

Camera *createCamera(GLFWwindow *window)
{
    Camera *camera = calloc(1, sizeof(Camera));
    if (!camera)
    {
        perror("Camera memory insufficient\n");
        return NULL;
    }

    glfwSetWindowUserPointer(window, camera);

    camera->firstMouse = true;
    camera->zoom = 45.0f;
    camera->yaw = -90.0f;
    camera->pitch = 0.0f;
    camera->xPosition = g_fbWidth / 2.0;
    camera->yPosition = g_fbHeight / 2.0;

    glm_vec3_copy((vec3){0.0f, 0.0f, 3.0f}, camera->position);
    glm_vec3_copy((vec3){0.0f, 0.0f, -1.0f}, camera->front);
    glm_vec3_copy((vec3){0.0f, 1.0f, 0.0f}, camera->up);

    return camera;
}

char *readFile(const char *filePath)
{
    FILE *file = fopen(filePath, "r");
    if (file == NULL)
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
        perror("Buffer");
        fclose(file);
        return NULL;
    }

    size_t read = fread(buffer, 1, fileSize, file);
    buffer[read] = '\0';

    fclose(file);

    return buffer;
}

unsigned int loadShader(ShaderType type, const char *shaderFilePath)
{
    unsigned int shaderId;
    char *shaderFile = readFile(shaderFilePath);
    if (shaderFile == NULL)
    {
        perror("Shader File load error");
        return -1;
    }

    if (type == VERTEX_SHADER)
        shaderId = glCreateShader(GL_VERTEX_SHADER);
    else if (type == FRAGMENT_SHADER)
        shaderId = glCreateShader(GL_FRAGMENT_SHADER);
    else
    {
        printf("Invalid shader type");
        return -1;
    }

    glShaderSource(shaderId, 1, (const char *const *)&shaderFile, NULL);
    glCompileShader(shaderId);

    int shaderSuccess;
    char shaderInfoLog[BUFSIZ];

    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &shaderSuccess);

    if (!shaderSuccess)
    {
        glGetShaderInfoLog(shaderId, BUFSIZ, NULL, shaderInfoLog);
        if (type == VERTEX_SHADER)
            printf("Vertex error: %s\n", shaderInfoLog);
        else
            printf("Fragment error: %s\n", shaderInfoLog);

        return -1;
    }

    free(shaderFile);
    return shaderId;
}

Shaders *createShaders(const char *vertexFilePath, const char *fragmentFilePath)
{
    int vertexShader = loadShader(VERTEX_SHADER, vertexFilePath),
        fragmentShader = loadShader(FRAGMENT_SHADER, fragmentFilePath);

    if (vertexShader == -1 || fragmentShader == -1)
    {
        printf("Shader load error");
        return NULL;
    }

    Shaders *shader = calloc(1, sizeof(Shaders));
    if (!shader)
    {
        perror("Memory");
        return NULL;
    }

    unsigned int ProgramShader = glCreateProgram();

    glAttachShader(ProgramShader, vertexShader);
    glAttachShader(ProgramShader, fragmentShader);
    glLinkProgram(ProgramShader);

    int programSuccess;
    char programInfoLog[BUFSIZ];
    glGetProgramiv(ProgramShader, GL_LINK_STATUS, &programSuccess);
    if (!programSuccess)
    {
        glGetProgramInfoLog(ProgramShader, BUFSIZ, NULL, programInfoLog);
        printf("Shader Program error: %s", programInfoLog);
        free(shader);
        return NULL;
    }

    shader->fragmentFilePath = fragmentFilePath;
    shader->vertexFilePath = vertexFilePath;
    shader->VertexShader = vertexShader;
    shader->FragmentShader = fragmentShader;
    shader->ShaderProgram = ProgramShader;

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shader;
}

void KeyboardInput(GLFWwindow *window, float deltaTime)
{
    float cameraSpeed = deltaTime * 2.5f;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        cameraSpeed *= 2;

    Camera *camera = glfwGetWindowUserPointer(window);
    if (!camera)
    {
        perror("Camera setup");
        return;
    }

    vec3 velocity, right;
    glm_cross(camera->front, camera->up, right);
    glm_vec3_normalize(right);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        glm_vec3_scale(camera->front, cameraSpeed, velocity);
        glm_vec3_add(camera->position, velocity, camera->position);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        glm_vec3_scale(camera->front, cameraSpeed, velocity);
        glm_vec3_sub(camera->position, velocity, camera->position);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        glm_vec3_scale(right, cameraSpeed, velocity);
        glm_vec3_sub(camera->position, velocity, camera->position);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        glm_vec3_scale(right, cameraSpeed, velocity);
        glm_vec3_add(camera->position, velocity, camera->position);
    }
}

void MouseInput(GLFWwindow *window, double xPos, double yPos)
{
    Camera *camera = glfwGetWindowUserPointer(window);
    if (!camera)
    {
        perror("Camera");
        return;
    }

    if (camera->firstMouse)
    {
        camera->xPosition = xPos;
        camera->yPosition = yPos;
        camera->firstMouse = false;
        return;
    }

    float xOffset = xPos - camera->xPosition;
    float yOffset = camera->yPosition - yPos;
    camera->xPosition = xPos;
    camera->yPosition = yPos;

    float sensitivity = 0.1f;
    camera->yaw += (float)(xOffset * sensitivity);
    camera->pitch += (float)(yOffset * sensitivity);

    if (camera->pitch < -89.0f)
        camera->pitch = -89.0f;
    else if (camera->pitch > 89.0f)
        camera->pitch = 89.0f;

    vec3 lookAtDirection = {
        cosf(glm_rad(camera->yaw)) * cosf(glm_rad(camera->pitch)),
        sinf(glm_rad(camera->pitch)),
        sinf(glm_rad(camera->yaw)) * cosf(glm_rad(camera->pitch)),
    };

    glm_normalize_to(lookAtDirection, camera->front);
}

void ScrollInput(GLFWwindow *window, double xScroll, double yScroll)
{
    (void)xScroll;

    Camera *camera = glfwGetWindowUserPointer(window);
    if (!camera)
    {
        perror("Camera scroll");
        return;
    }

    camera->zoom -= (float)yScroll;
    if (camera->zoom > 45.0f)
        camera->zoom = 45.0f;
    else if (camera->zoom < 1.0f)
        camera->zoom = 1.0f;
}

int loadTexture(char *texturePath)
{
    unsigned int textureId;

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char *textureData = stbi_load(texturePath, &width, &height, &nrChannels, 0);
    if (!textureData)
    {
        perror("Texture data");
        glDeleteTextures(1, &textureId);
        return -1;
    }

    GLenum format;
    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else
        format = GL_RGBA;

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, textureData);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(textureData);

    return textureId;
}

void GetViewMatrix(Camera *camera, mat4 view)
{
    vec3 center;
    glm_vec3_add(camera->position, camera->front, center);
    glm_lookat(camera->position, center, camera->up, view);
}