#include "../../../Header.h"

const int WINDOW_WIDTH = 800,
          WINDOW_HEIGHT = 600;

struct Camera;
typedef struct Camera Camera;

typedef void (*MouseInput)(GLFWwindow *window);
typedef void (*KeyboardInput)(GLFWwindow *window);

typedef struct
{
    unsigned int vertexShader, fragmentShader, shaderProgram;
} Shaders;

struct Camera
{
    bool firstMouse;
    float pitch, yaw, zoom, deltaTime, lastFrame, cameraSpeed, lastX, lastY;
    vec3 cameraPosition, cameraFront, cameraUp, cameraTarget, worldUp;
    mat4 viewMatrix, projectionMatrix;
    MouseInput mouseCallback;
    KeyboardInput keyboardCallback;
};

void GLFWInit();
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
char *fileReader(char *filePath);
Shaders *createShader(char *vertexFilePath, char *fragmentFilePath);
void mouseCallback(GLFWwindow *window, double xpos, double ypos);
void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);
void setupKeyboardInput(GLFWwindow *window);
Camera *createCamera();

int main(void)
{
    GLFWInit();

    char windowTitle[] = "Camera";

    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, windowTitle, NULL, NULL);
    if (!window)
    {
        perror("Window");
        glfwTerminate();
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        perror("GLAD");
        glfwTerminate();
        return -2;
    }

    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    glViewport(0, 0, fbWidth, fbHeight);

    char vertexFilePath[] = "Exercises/2.0 - Camera/Camera/Shaders/CameraVert.glsl",
         fragmentFilePath[] = "Exercises/2.0 - Camera/Camera/Shaders/CameraFrag.glsl";

    Shaders *shaders = createShader(vertexFilePath, fragmentFilePath);
    if (!shaders)
    {
        perror("Shaders");
        return -3;
    }

    float vertices[] = {
        // positions // colors // texture coords
        0.5f,
        0.5f,
        0.0f,
        1.0f,
        1.0f, // top right
        0.5f,
        -0.5f,
        0.0f,
        1.0f,
        0.0f, // bottom right
        -0.5f,
        -0.5f,
        0.0f,
        0.0f,
        0.0f,                         // bottom left
        -0.5f, 0.5f, 0.0f, 0.0f, 1.0f // Index 3: Top Left (ADD THIS)
    };
    unsigned int indices[] = {
        0, 1, 3,
        1, 3, 2};

    unsigned int textureID;
    glGenTextures(1, &textureID);

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    int imageWidth, imageHeight, nrChannels;
    GLint format;

    char imageFilePath[] = "";
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load("Exercises/2.0 - Camera/Camera/Shaders/2145.jpg", &imageWidth, &imageHeight, &nrChannels, 0);

    if (!data)
    {
        perror("Image");
        return -4;
    }

    format = nrChannels == 4 ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, imageWidth, imageHeight, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof indices, indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void *)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void *)(sizeof(float) * 3));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    Camera *flyCamera = createCamera();
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetWindowUserPointer(window, flyCamera);
    flyCamera->mouseCallback(window);

    mat4 modelMatrix = GLM_MAT4_IDENTITY_INIT,
         viewMatrix = GLM_MAT4_IDENTITY_INIT,
         projectionMatrix = GLM_MAT4_IDENTITY_INIT;

    unsigned int modelULocation = glGetUniformLocation(shaders->shaderProgram, "model"),
                 viewULocation = glGetUniformLocation(shaders->shaderProgram, "view"),
                 projectionULocation = glGetUniformLocation(shaders->shaderProgram, "projection"),
                 textureLocation = glGetUniformLocation(shaders->shaderProgram, "texture1");

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        flyCamera->deltaTime = currentFrame - flyCamera->lastFrame;
        flyCamera->lastFrame = currentFrame;
        flyCamera->cameraSpeed = 2.5f * flyCamera->deltaTime;

        glClearColor(0.7f, 0.4f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        processInput(window);
        flyCamera->keyboardCallback(window);

        glUseProgram(shaders->shaderProgram);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glUniform1i(textureLocation, 0);

        glUniformMatrix4fv(modelULocation, 1, GL_FALSE, (float *)modelMatrix);

        vec3 direction = {
            cos(glm_rad(flyCamera->yaw)) * cos(glm_rad(flyCamera->pitch)),
            sin(glm_rad(flyCamera->pitch)),
            sin(glm_rad(flyCamera->yaw)) * cos(glm_rad(flyCamera->pitch))};

        glm_normalize_to(direction, flyCamera->cameraFront);

        glm_vec3_add(flyCamera->cameraPosition, flyCamera->cameraFront, flyCamera->cameraTarget);
        glm_lookat(flyCamera->cameraPosition, flyCamera->cameraTarget, flyCamera->cameraUp, flyCamera->viewMatrix);

        glm_perspective(glm_rad(flyCamera->zoom), (float)fbWidth / (float)fbHeight, 0.1f, 100.0f, flyCamera->projectionMatrix);

        glUniformMatrix4fv(viewULocation, 1, GL_FALSE, (float *)flyCamera->viewMatrix);
        glUniformMatrix4fv(projectionULocation, 1, GL_FALSE, (float *)flyCamera->projectionMatrix);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    free(shaders);
    free(flyCamera);
    glfwTerminate();
    return 0;
}

void GLFWInit()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3.0);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3.0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_COMPAT_PROFILE, GL_TRUE);
}
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}
void processInput(GLFWwindow *window)
{
    if (!window)
    {
        printf("Invalid window pointer reference");
        return;
    }

    if (glfwGetKey(window, GLFW_KEY_ESCAPE))
        glfwSetWindowShouldClose(window, GL_TRUE);
}
void setupMouseInput(GLFWwindow *window)
{
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouseCallback);
}
void setupKeyboardInput(GLFWwindow *window)
{
    Camera *camera = (Camera *)glfwGetWindowUserPointer(window);

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera->cameraSpeed *= 2;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        vec3 velocity;
        glm_vec3_scale(camera->cameraFront, camera->cameraSpeed, velocity);
        glm_vec3_add(camera->cameraPosition, velocity, camera->cameraPosition);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        vec3 velocity;
        glm_vec3_scale(camera->cameraFront, camera->cameraSpeed, velocity);
        glm_vec3_sub(camera->cameraPosition, velocity, camera->cameraPosition);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        vec3 crossResult;
        glm_vec3_cross(camera->cameraFront, camera->cameraUp, crossResult);
        glm_normalize(crossResult);

        vec3 velocity;
        glm_vec3_scale(crossResult, camera->cameraSpeed, velocity);
        glm_vec3_sub(camera->cameraPosition, velocity, camera->cameraPosition);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        vec3 crossResult;
        glm_vec3_cross(camera->cameraFront, camera->cameraUp, crossResult);
        glm_normalize(crossResult);

        vec3 velocity;
        glm_vec3_scale(crossResult, camera->cameraSpeed, velocity);
        glm_vec3_add(camera->cameraPosition, velocity, camera->cameraPosition);
    }
}
void mouseCallback(GLFWwindow *window, double xpos, double ypos)
{
    Camera *camera = (Camera *)glfwGetWindowUserPointer(window);

    if (camera->firstMouse)
    {
        camera->lastX = xpos;
        camera->lastY = ypos;
        camera->firstMouse = false;
    }

    float xOffset = xpos - camera->lastX;
    float yOffset = camera->lastY - ypos;

    camera->lastX = xpos;
    camera->lastY = ypos;

    const float sensitivity = 0.01f;
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    camera->yaw += xOffset;
    camera->pitch += yOffset;

    if (camera->pitch > 89.0f)
        camera->pitch = 89.0f;
    if (camera->pitch < -89.0f)
        camera->pitch = -89.0f;
}
void scrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
    Camera *camera = (Camera *)glfwGetWindowUserPointer(window);

    if (camera->zoom < 1.0f)
        camera->zoom = 1.0f;
    if (camera->zoom > 45.0f)
        camera->zoom = 45.0f;
}
char *fileReader(char *filePath)
{
    FILE *file = fopen(filePath, "r");
    if (!file)
    {
        perror("FILE");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *fileContents = calloc(fileSize + 1, sizeof(char));
    if (!fileContents)
    {
        perror("Memory");
        fclose(file);
        return NULL;
    }

    fread(fileContents, fileSize, sizeof(char), file);
    fclose(file);
    return fileContents;
}
Shaders *createShader(char *vertexFilePath, char *fragmentFilePath)
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

    const char *vertexFile = fileReader(vertexFilePath),
               *fragmentFile = fileReader(fragmentFilePath);

    glShaderSource(shaders->vertexShader, 1, &vertexFile, NULL);
    glCompileShader(shaders->vertexShader);

    int vertexSuccess;
    char vertexInfoLog[BUFSIZ];
    glGetShaderiv(shaders->vertexShader, GL_COMPILE_STATUS, &vertexSuccess);

    if (!vertexSuccess)
    {
        glGetShaderInfoLog(shaders->vertexShader, BUFSIZ, NULL, vertexInfoLog);
        printf("Vertex error: %s", vertexInfoLog);
        free(shaders);
        return NULL;
    }

    glShaderSource(shaders->fragmentShader, 1, &fragmentFile, NULL);
    glCompileShader(shaders->fragmentShader);

    int fragmentSuccess;
    char fragmentInfoLog[BUFSIZ];
    glGetShaderiv(shaders->fragmentShader, GL_COMPILE_STATUS, &fragmentSuccess);

    if (!fragmentSuccess)
    {
        glGetShaderInfoLog(shaders->fragmentShader, BUFSIZ, NULL, fragmentInfoLog);
        printf("Fragment error: %s", fragmentInfoLog);
        free(shaders);
        return NULL;
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
        printf("Shader error: %s", shaderInfoLog);
        free(shaders);
        return NULL;
    }

    free((void *)vertexFile);
    free((void *)fragmentFile);

    return shaders;
}
Camera *createCamera()
{
    Camera *flyCamera = calloc(1, sizeof(Camera));
    if (!flyCamera)
    {
        perror("Memory");
        return NULL;
    }

    // Calculation of x,y,z - Gram-Schmidt
    vec3 cameraPosition = {0.0f, 0.0f, 3.0f},
         cameraFront = {0.0f, 0.0f, -1.0f},
         cameraUp = {0.0f, 1.0f, 0.0f},
         worldUp = {0.0f, 1.0f, 0.0f};

    glm_vec3_copy(cameraPosition, flyCamera->cameraPosition);
    glm_vec3_copy(cameraFront, flyCamera->cameraFront);
    glm_vec3_copy(cameraUp, flyCamera->cameraUp);
    glm_vec3_copy(worldUp, flyCamera->worldUp);

    flyCamera->pitch = 0.0f;
    flyCamera->yaw = -90.0f;
    flyCamera->zoom = 45.0f;
    flyCamera->firstMouse = true;
    flyCamera->deltaTime = 0.0f;
    flyCamera->lastFrame = 0.0f;
    flyCamera->lastX = WINDOW_WIDTH / 2;
    flyCamera->lastY = WINDOW_HEIGHT / 2;

    flyCamera->keyboardCallback = setupKeyboardInput;
    flyCamera->mouseCallback = setupMouseInput;

    return flyCamera;
}