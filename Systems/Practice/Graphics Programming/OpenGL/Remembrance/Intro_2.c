#include "../Header.h"

const int WINDOW_WIDTH = 800,
          WINDOW_HEIGHT = 800;

float lastX = WINDOW_WIDTH / 2,
      lastY = WINDOW_HEIGHT / 2;

float yaw = -90.0f,
      pitch = 0.0f,
      zoom = 45.0f;

static bool firstMouse = true;

typedef struct
{
    unsigned int vertexShader;
    unsigned int fragmentShader;
    unsigned int shaderProgram;
    const char *vertexFile;
    const char *fragmentFile;
} GLShaderProgram;

void GLFWInit();
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processKeyboardInput(GLFWwindow *window, float deltaTime, vec3 cameraFront, vec3 cameraPosition, vec3 cameraTarget, vec3 cameraUp);
void processMouseInput(GLFWwindow *window, double xpos, double ypos);
void processScrollInput(GLFWwindow *window, double xoffset, double yoffset);
char *readFile(const char *filePath);
GLShaderProgram *createShader(const char *vertexFilePath, const char *fragmentFilePath);

// Mathematics behind Transformations
void Translation();
void Rotation();

int main(void)
{
    GLFWInit();

    char title[] = "Window";
    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, title, NULL, NULL);
    if (!window)
    {
        perror("Window generation error");
        return -1;
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        perror("GLAD");
        return -2;
    }
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    glViewport(0, 0, fbWidth, fbHeight);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, processMouseInput);
    glfwSetScrollCallback(window, processScrollInput);

    int imageWidth, imageHeight, nrChannels;

    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load("Remembrance/Images/Donut-3.png", &imageWidth, &imageHeight, &nrChannels, 0);

    GLenum format = nrChannels == 4 ? GL_RGBA : GL_RGB;

    float vertices[] = {
        // Positions          // Colors           // Texture Coordinates (U, V)
        // --- Front Face ---
        -0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // 0
        0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // 1
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,   // 2
        -0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,  // 3

        // --- Back Face ---
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // 4
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,  // 5
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,   // 6
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  // 7

        // --- Left Face ---
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,   // 8
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,  // 9
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // 10
        -0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // 11

        // --- Right Face ---
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,   // 12
        0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,  // 13
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // 14
        0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,  // 15

        // --- Bottom Face ---
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // 16
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,  // 17
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,   // 18
        -0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,  // 19

        // --- Top Face ---
        -0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // 20
        0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,  // 21
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,   // 22
        -0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f   // 23
    };
    unsigned int indices[] = {
        // Front Face
        0, 1, 2, 2, 3, 0,
        // Back Face
        4, 5, 6, 6, 7, 4,
        // Left Face
        8, 9, 10, 10, 11, 8,
        // Right Face
        12, 13, 14, 14, 15, 12,
        // Bottom Face
        16, 17, 18, 18, 19, 16,
        // Top Face
        20, 21, 22, 22, 23, 20};
    vec3 cubePositions[] = {
        {0.0f, 0.0f, 0.0f},
        {2.5f, 0.3f, -1.5f}, // Moved slightly back in Z so it's easier to see
        {-1.5f, -2.2f, -2.5f}};

    unsigned int vbo, vao, ebo, textureId;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glGenTextures(1, &textureId);

    glBindVertexArray(vao);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, format, imageWidth, imageHeight, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        perror("Image");
        return -3;
    }

    stbi_image_free(data);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, sizeof(float), GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *)0);
    glVertexAttribPointer(1, sizeof(float), GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *)(sizeof(float) * 3));
    glVertexAttribPointer(2, sizeof(float), GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *)(sizeof(float) * 6));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    const char vertexFilePath[] = "Remembrance/Intro-Shaders/Intro2Vert.vert",
               fragmentFilePath[] = "Remembrance/Intro-Shaders/Intro2Frag.frag";

    GLShaderProgram *programShaders = createShader(vertexFilePath, fragmentFilePath);

    vec3 translationVector = {0.0f, 0.0f, -3.0f},
         rotateVector = {0.5f, 1.0f, 0.0f};

    float deltaTime = 0.0f,
          lastFrame = 0.0f;

    vec3 cameraPosition = {0.0f, 0.0f, 3.0f},
         cameraFront = {0.0f, 0.0f, -1.0f},
         cameraUp = {0.0f, 1.0f, 0.0f},
         cameraTarget,
         worldUp = {0.0f, 1.0f, 0.0f};

    if (!programShaders)
    {
        perror("Shaders");
        return -4;
    }

    glClearColor(1.0f, .5f, .25f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    while (glfwWindowShouldClose(window) == false)
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        processKeyboardInput(window, deltaTime, cameraFront, cameraPosition, cameraTarget, cameraUp);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glUseProgram(programShaders->shaderProgram);

        // Get Uniform Locations
        unsigned int modelLocation = glGetUniformLocation(programShaders->shaderProgram, "modelMatrix");
        unsigned int viewLocation = glGetUniformLocation(programShaders->shaderProgram, "viewMatrix");
        unsigned int projectionLocation = glGetUniformLocation(programShaders->shaderProgram, "projectionMatrix");

        mat4 view = GLM_MAT4_IDENTITY_INIT,
             projection = GLM_MAT4_IDENTITY_INIT;

        // View matrix
        vec3 direction = {
            cos(glm_rad(yaw)) * cos(glm_rad(pitch)),  // X
            sin(glm_rad(pitch)),                      // Y
            sin(glm_rad(yaw)) * cos(glm_rad(pitch))}; // Z

        glm_normalize_to(direction, cameraFront);

        glm_mat4_identity(view);
        glm_vec3_add(cameraPosition, cameraFront, cameraTarget);
        glm_lookat(cameraPosition, cameraTarget, cameraUp, view);

        // Perspective matrix
        glm_perspective(glm_rad(zoom), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f, projection);

        for (int i = 0; i < 3; i++)
        {
            // Order of events translate -> rotate -> scale
            mat4 model = GLM_MAT4_IDENTITY_INIT;
            glm_mat4_identity(model);
            glm_translate(model, cubePositions[i]);
            glm_rotate(model, glm_rad(glfwGetTime()), rotateVector);

            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, (float *)model);
            glUniformMatrix4fv(viewLocation, 1, GL_FALSE, (float *)view);
            glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, (float *)projection);

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

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    if (!window)
    {
        perror("Window");
        return;
    }
    glViewport(0, 0, width, height);
}

void processKeyboardInput(GLFWwindow *window, float deltaTime, vec3 cameraFront, vec3 cameraPosition, vec3 cameraTarget, vec3 cameraUp)
{
    float cameraSpeed = 2.5f * deltaTime;

    if (!window)
    {
        perror("Window");
        return;
    }

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        cameraSpeed = 5.0f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        vec3 velocity;
        glm_vec3_scale(cameraFront, cameraSpeed, velocity);
        glm_vec3_add(cameraPosition, velocity, cameraPosition);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        vec3 velocity;
        glm_vec3_scale(cameraFront, cameraSpeed, velocity);
        glm_vec3_sub(cameraPosition, velocity, cameraPosition);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        // Find right vector by cross-multiplication of both up and front
        vec3 velocityCrossResult;
        glm_vec3_cross(cameraFront, cameraUp, velocityCrossResult);
        glm_normalize(velocityCrossResult);

        // Subtract since left is opposite of right
        vec3 velocity;
        glm_vec3_scale(velocityCrossResult, cameraSpeed, velocity);
        glm_vec3_sub(cameraPosition, velocity, cameraPosition);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        vec3 velocityCrossResult;
        glm_vec3_cross(cameraFront, cameraUp, velocityCrossResult);
        glm_normalize(velocityCrossResult);

        vec3 velocity;
        glm_vec3_scale(velocityCrossResult, cameraSpeed, velocity);
        glm_vec3_add(cameraPosition, velocity, cameraPosition);
    }
}

void processMouseInput(GLFWwindow *window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX,
          yoffset = ypos - lastY;

    lastX = xpos;
    lastY = ypos;

    const float sensitivity = 0.01f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    // For macbooks inverted mouse movement
    pitch += -yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;
}

void processScrollInput(GLFWwindow *window, double xoffset, double yoffset)
{
    zoom -= (float)yoffset;
    if (zoom < 1.0f)
        zoom = 1.0f;
    if (zoom > 45.0f)
        zoom = 45.0f;
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

    char *buffer = calloc(fileSize + 1, sizeof(char));
    if (!buffer)
    {
        perror("Memory allocation");
        return NULL;
    }

    fread(buffer, sizeof(char), fileSize, file);
    buffer[fileSize] = '\0';

    fclose(file);
    return buffer;
}

GLShaderProgram *createShader(const char *vertexFilePath, const char *fragmentFilePath)
{
    GLShaderProgram *shader = calloc(1, sizeof(GLShaderProgram));
    if (!shader)
    {
        perror("Memory");
        return NULL;
    }

    const char *vertexFile = readFile(vertexFilePath),
               *fragmentFile = readFile(fragmentFilePath);

    if (!vertexFile || !fragmentFile)
    {
        free(shader);
        perror("File error");
        return NULL;
    }

    shader->vertexFile = vertexFile;
    shader->fragmentFile = fragmentFile;

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER),
                 fragmentShader = glCreateShader(GL_FRAGMENT_SHADER),
                 shaderProgram = glCreateProgram();

    glShaderSource(vertexShader, 1, &vertexFile, NULL);
    glCompileShader(vertexShader);

    int vertexSuccess;
    char vertexInfoLog[BUFSIZ];

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertexSuccess);
    if (!vertexSuccess)
    {
        glGetShaderInfoLog(vertexShader, BUFSIZ, NULL, vertexInfoLog);
        printf("Vertex compile error: %s", vertexInfoLog);
        free(shader);
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
        printf("Fragment compile error: %s", fragmentInfoLog);
        free(shader);
        return NULL;
    }

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int programSuccess;
    char programInfoLog[BUFSIZ];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &programSuccess);

    if (!programSuccess)
    {
        glGetProgramInfoLog(shaderProgram, BUFSIZ, NULL, programInfoLog);
        printf("Shader link error: %s", programInfoLog);
        free(shader);
        return NULL;
    }

    shader->vertexShader = vertexShader;
    shader->fragmentShader = fragmentShader;
    shader->shaderProgram = shaderProgram;

    free((void *)vertexFile);
    free((void *)fragmentFile);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shader;
}

void Translation()
{
    vec4 originalPosition = {1.0f, 0.0f, 1.0f};
    vec3 translationPosition = {1.0f, 1.0f, 1.0f};
    mat4 translationMatrix = GLM_MAT4_IDENTITY_INIT;

    glm_translate(translationMatrix, translationPosition);

    vec4 result;
    glm_mat4_mulv(translationMatrix, originalPosition, result);

    for (int i = 0; i < 4; i++)
    {
        printf("%f", result[i]);
    }
}

void Rotation()
{
    mat4 translationMatrix = GLM_MAT4_IDENTITY_INIT;
    vec3 positions = {0.0f, 0.0f, 1.0f};

    glm_rotate(translationMatrix, glm_rad(45), positions);

    vec3 scaleFactor = {1.25f, 1.5f, 1.15f};
    glm_scale(translationMatrix, scaleFactor);
}