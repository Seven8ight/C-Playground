#include "../../Header.h"

const int WINDOW_WIDTH = 800,
          WINDOW_HEIGHT = 600;

float lastX = WINDOW_WIDTH / 2,
      lastY = WINDOW_HEIGHT / 2;

float yaw = -90.0f, pitch = 0.0f, zoom = 45.0f;
static bool firstMouse = true;

typedef struct
{
    unsigned int vertexShader, fragmentShader, shaderProgram;
} Shaders;

void GLFWInit();
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window, float deltaTime, vec3 cameraFront, vec3 cameraPosition, vec3 cameraTarget, vec3 cameraUp);
void mouseCallback(GLFWwindow *window, double xpos, double ypos);
void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);
Shaders *createShaders(char *vertexFilePath, char *fragmentFilePath);

int main(void)
{
    GLFWInit();

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        perror("Window");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        perror("GLAD");
        return -2;
    }

    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    glViewport(0, 0, fbWidth, fbHeight);

    // Hide the cursor and capture it.
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);

    // configure global opengl state
    // -----------------------------

    char vertexFilePath[] = "Chapters/Camera - 8/Shaders/CameraVert.glsl",
         fragmentFilePath[] = "Chapters/Camera - 8/Shaders/CameraFrag.glsl";
    Shaders *shaders = createShaders(vertexFilePath, fragmentFilePath);

    unsigned int textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    // wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load("Chapters/Camera - 8/Shaders/Donut-3.png", &width, &height, &nrChannels, 0);
    GLint format = nrChannels == 4 ? GL_RGBA : GL_RGB;
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        printf("Image load error");
        glfwTerminate();
        return -3;
    }
    stbi_image_free(data);

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    float vertices[] = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f};
    vec3 cubePositions[] = {
        {0.0f, 0.0f, 0.0f},
        {2.0f, 5.0f, -15.0f},
        {-1.5f, -2.2f, -2.5f},
        {-3.8f, -2.0f, -12.3f},
        {2.4f, -0.4f, -3.5f},
        {-1.7f, 3.0f, -7.5f},
        {1.3f, -2.0f, -2.5f},
        {1.5f, 2.0f, -2.5f},
        {1.5f, 0.2f, -1.5f},
        {-1.3f, 1.0f, -1.5f}};

    glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void *)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void *)(sizeof(float) * 3));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glEnable(GL_DEPTH_TEST);
    // // Process of calculating the camera's x,y,z coordinate - Gram-Schmidt
    // // Z axis position
    // vec3 cameraPosition = {0.0f, 0.0f, 3.0f},
    //      cameraTarget = {0.0f, 0.0f, 0.0f},
    //      // Camera Direction
    //     cameraDirection;

    // glm_normalize_to(cameraPosition - cameraTarget, cameraDirection);

    // // Right axis - positive x axis coordinate
    // vec3 up = {0.0f, 1.0f, 0.0f},
    //      cameraRight;

    // glm_cross_to(up, cameraDirection, cameraRight);
    // glm_normalize(cameraRight);

    // // Up axis - y axis
    // vec3 cameraUp;
    // glm_vec3_cross(cameraDirection, cameraRight, cameraUp);
    // glm_cross(cameraDirection, cameraRight, cameraUp);
    // glm_cross_to(cameraDirection, cameraRight);

    unsigned int modelLocation = glGetUniformLocation(shaders->shaderProgram, "model");
    unsigned int viewLocation = glGetUniformLocation(shaders->shaderProgram, "view");
    unsigned int projectionLocation = glGetUniformLocation(shaders->shaderProgram, "projection");

    float deltaTime = 0.0f,
          lastFrame = 0.0f;

    vec3 cameraPosition = {0.0f, 0.0f, 3.0f},
         cameraFront = {0.0f, 0.0f, -1.0f},
         cameraUp = {0.0f, 1.0f, 0.0f},
         cameraTarget,
         worldUp = {0.0f, 1.0f, 0.0f};

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window, deltaTime, cameraFront, cameraPosition, cameraTarget, cameraUp);
        glClearColor(0.5f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureId);

        glUseProgram(shaders->shaderProgram);
        glUniform1i(glGetUniformLocation(shaders->shaderProgram, "texture1"), 0);

        // Transformations
        // mat4 modelMatrix = GLM_MAT4_IDENTITY_INIT;
        vec3 translationVector = {0.0f, 0.0f, 3.0f};

        // View matrix
        mat4 viewMatrix = GLM_MAT4_IDENTITY_INIT;
        glm_vec3_add(cameraPosition, cameraFront, cameraTarget);
        glm_lookat(cameraPosition, cameraTarget, cameraUp, viewMatrix);

        vec3 direction = {
            cos(glm_rad(yaw)) * cos(glm_rad(pitch)),
            sin(glm_rad(pitch)),
            sin(glm_rad(yaw)) * cos(glm_rad(pitch))};

        glm_normalize_to(direction, cameraFront);

        mat4 projection = GLM_MAT4_IDENTITY_INIT;
        glm_perspective(glm_rad(zoom), (float)fbWidth / (float)fbHeight, 0.1f, 100.0f, projection);

        for (unsigned int i = 0; i < 10; i++)
        {
            mat4 modelMatrix = GLM_MAT4_IDENTITY_INIT;
            glm_translate(modelMatrix, cubePositions[i]);
            if (i % 3 == 0)
            {
                float angle = 20.0f * i;
                vec3 rotationAxis = {1.0f, 0.3f, 0.5f};
                glm_rotate(modelMatrix, glfwGetTime() * (i + 1), rotationAxis);
            }

            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, (float *)modelMatrix);
            glUniformMatrix4fv(viewLocation, 1, GL_FALSE, (float *)viewMatrix);
            glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, (float *)projection);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glfwSwapBuffers(window);
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
void processInput(GLFWwindow *window, float deltaTime, vec3 cameraFront, vec3 cameraPosition, vec3 cameraTarget, vec3 cameraUp)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    else
    {
        static bool polygon = false, cursorLocked = true, rPressedLastFrame = false;
        float cameraSpeed = 2.5f * deltaTime;

        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        {
            if (polygon)
            {
                glad_glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                polygon = true;
            }
            else
            {
                glad_glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                polygon = false;
            }
        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            vec3 velocity;
            glm_vec3_scale(cameraFront, cameraSpeed, velocity);
            glm_vec3_add(cameraPosition, velocity, cameraPosition);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            vec3 velocity;
            glm_vec3_scale(cameraFront, cameraSpeed, velocity);
            glm_vec3_sub(cameraPosition, velocity, cameraPosition);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            vec3 crossResult;
            glm_vec3_cross(cameraFront, cameraUp, crossResult);
            glm_normalize(crossResult);

            vec3 velocity;
            glm_vec3_scale(crossResult, cameraSpeed, velocity);
            glm_vec3_sub(cameraPosition, velocity, cameraPosition);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            vec3 crossResult;
            glm_vec3_cross(cameraFront, cameraUp, crossResult);
            glm_normalize(crossResult);

            vec3 velocity;
            glm_vec3_scale(crossResult, cameraSpeed, velocity);
            glm_vec3_add(cameraPosition, velocity, cameraPosition);
        }
        // if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS || !rPressedLastFrame)
        // {
        //     cursorLocked = !cursorLocked;

        //     if (cursorLocked)
        //         glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        //     else
        //         glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        //     rPressedLastFrame = true;
        // }

        // if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE)
        // {
        //     rPressedLastFrame = false;
        // }
    }
}
void mouseCallback(GLFWwindow *window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // y ranges from bottom to top

    lastX = xpos;
    lastY = ypos;

    const float sensitivity = 0.01f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;
}
void scrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
    zoom -= (float)yoffset;
    if (zoom < 1.0f)
        zoom = 1.0f;
    if (zoom > 45.0f)
        zoom = 45.0f;
}
Shaders *createShaders(char *vertexFilePath, char *fragmentFilePath)
{
    Shaders *shaders = malloc(sizeof(Shaders));
    if (!shaders)
    {
        perror("Memory");
        return NULL;
    }

    FILE *vertexFile = fopen(vertexFilePath, "r"),
         *fragmentFile = fopen(fragmentFilePath, "r");

    fseek(vertexFile, 0, SEEK_END);
    long vertexFileLength = ftell(vertexFile);
    fseek(vertexFile, 0, SEEK_SET);

    fseek(fragmentFile, 0, SEEK_END);
    long fragmentFileLength = ftell(fragmentFile);
    fseek(fragmentFile, 0, SEEK_SET);

    char *vertexFileContents = malloc(vertexFileLength + 1),
         *fragmentFileContents = malloc(fragmentFileLength + 1);

    fread(vertexFileContents, sizeof(char), vertexFileLength, vertexFile);
    vertexFileContents[vertexFileLength] = '\0';
    fread(fragmentFileContents, sizeof(char), fragmentFileLength, fragmentFile);
    fragmentFileContents[fragmentFileLength] = '\0';

    if (!vertexFile || !fragmentFile)
    {
        perror("File error");
        return NULL;
    }

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER),
                 fragmentShader = glCreateShader(GL_FRAGMENT_SHADER),
                 shaderProgram = glCreateProgram();

    glShaderSource(vertexShader, 1, (const char *const *)&vertexFileContents, NULL);
    glShaderSource(fragmentShader, 1, (const char *const *)&fragmentFileContents, NULL);
    glCompileShader(vertexShader);
    glCompileShader(fragmentShader);

    int vertexSuccess;
    char vertexInfoLog[BUFSIZ];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertexSuccess);
    if (!vertexSuccess)
    {
        glGetShaderInfoLog(vertexShader, BUFSIZ, NULL, vertexInfoLog);
        printf("Vertex error: %s", vertexInfoLog);
        free(shaders);
        return NULL;
    }

    int fragmentSuccess;
    char fragmentInfoLog[BUFSIZ];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragmentSuccess);
    if (!fragmentSuccess)
    {
        glGetShaderInfoLog(fragmentShader, BUFSIZ, NULL, fragmentInfoLog);
        printf("Fragment error: %s", fragmentInfoLog);
        free(shaders);
        return NULL;
    }

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int shaderSuccess;
    char shaderInfoLog[BUFSIZ];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &shaderSuccess);
    if (!shaderSuccess)
    {
        glGetProgramInfoLog(shaderProgram, BUFSIZ, NULL, shaderInfoLog);
        printf("Shader error: %s", shaderInfoLog);
        free(shaders);
        return NULL;
    }

    shaders->vertexShader = vertexShader;
    shaders->fragmentShader = fragmentShader;
    shaders->shaderProgram = shaderProgram;

    return shaders;
}