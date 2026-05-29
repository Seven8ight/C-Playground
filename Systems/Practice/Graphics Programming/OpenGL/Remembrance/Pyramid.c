#include "../Header.h"

const int WINDOW_WIDTH = 800,
          WINDOW_HEIGHT = 600;

typedef struct
{
    unsigned vertexShader, fragmentShader, shaderProgram;
} Shaders;
typedef struct
{
    float yaw, pitch, zoom, lastX, lastY;
    bool firstMouse;
} Camera;

void GLFWInit();
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window, float deltaTime, vec3 cameraFront, vec3 cameraPosition, vec3 cameraTarget, vec3 cameraUp);
void mouseCallback(GLFWwindow *window, double xpos, double ypos);
void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);
char *shaderFile(char *filePath);
Shaders *createShader(char *vertexFilePath, char *fragmentFilePath);
Camera *createCamera();

int main()
{
    GLFWInit();

    char windowTitle[] = "Coordinates";
    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, windowTitle, NULL, NULL);
    if (!window)
    {
        perror("Window");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);

    Camera *camera = createCamera();
    if (!camera)
    {
        perror("Camera");
        return -2;
    }
    glfwSetWindowUserPointer(window, camera);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        perror("GLAD");
        return -3;
    }

    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    glViewport(0, 0, fbWidth, fbHeight);

    float vertices[] = {
        // FRONT FACE (facing +Z)        pos                    color
        0.0f,
        1.0f,
        0.0f,
        1.0f,
        0.0f,
        0.0f, // apex    - red
        -0.5f,
        -0.5f,
        0.5f,
        0.0f,
        1.0f,
        0.0f, // bot-left  - green
        0.5f,
        -0.5f,
        0.5f,
        0.0f,
        0.0f,
        1.0f, // bot-right - blue

        // RIGHT FACE (facing +X)
        0.0f,
        1.0f,
        0.0f,
        1.0f,
        0.0f,
        0.0f,
        0.5f,
        -0.5f,
        0.5f,
        0.0f,
        1.0f,
        0.0f,
        0.5f,
        -0.5f,
        -0.5f,
        0.0f,
        0.0f,
        1.0f,

        // BACK FACE (facing -Z)
        0.0f,
        1.0f,
        0.0f,
        1.0f,
        0.0f,
        0.0f,
        0.5f,
        -0.5f,
        -0.5f,
        0.0f,
        1.0f,
        0.0f,
        -0.5f,
        -0.5f,
        -0.5f,
        0.0f,
        0.0f,
        1.0f,

        // LEFT FACE (facing -X)
        0.0f,
        1.0f,
        0.0f,
        1.0f,
        0.0f,
        0.0f,
        -0.5f,
        -0.5f,
        -0.5f,
        0.0f,
        1.0f,
        0.0f,
        -0.5f,
        -0.5f,
        0.5f,
        0.0f,
        0.0f,
        1.0f,

        // BASE (two triangles, winding viewed from below)
        -0.5f,
        -0.5f,
        0.5f,
        0.5f,
        0.5f,
        0.5f,
        0.5f,
        -0.5f,
        0.5f,
        0.5f,
        0.5f,
        0.5f,
        0.5f,
        -0.5f,
        -0.5f,
        0.5f,
        0.5f,
        0.5f,

        0.5f,
        -0.5f,
        -0.5f,
        0.5f,
        0.5f,
        0.5f,
        -0.5f,
        -0.5f,
        -0.5f,
        0.5f,
        0.5f,
        0.5f,
        -0.5f,
        -0.5f,
        0.5f,
        0.5f,
        0.5f,
        0.5f,
    };
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(1);

    int imageWidth, imageHeight, nrChannels;
    unsigned char *data = stbi_load("Remembrance/Images/Donut-3.png", &imageWidth, &imageHeight, &nrChannels, 0);

    GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, format, imageWidth, imageHeight, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        perror("Texture");
        return -4;
    }

    stbi_image_free(data);

    unsigned vbo, vao;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void *)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void *)(sizeof(float) * 3));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    char vertexFilePath[] = "Remembrance/Shaders/PyramidVert.glsl",
         fragmentFilePath[] = "Remembrance/Shaders/PyramidFrag.glsl";

    Shaders *shaders = createShader(vertexFilePath, fragmentFilePath);
    if (!shaders)
    {
        perror("Shaders");
        return -5;
    }

    glUseProgram(shaders->shaderProgram);

    // Transformations
    // Translation -> rotation ->scale
    mat4 translationMatrix = GLM_MAT4_IDENTITY_INIT;
    vec3 positionVector = {0.25, -0.5, 1.0};
    vec3 rotationVector = {0.0, 1.0, 0.0};
    vec3 scaleVector = {1.5f, 1.15f, 1.0};

    mat4 translationMatrix2 = GLM_MAT4_IDENTITY_INIT;
    vec3 positionVector2 = {-0.25, 0.5, 1.0};
    vec3 rotationVector2 = {-2.5, -4.5, 1.0};
    vec3 scaleVector2 = {1.25f, 1.15f, 1.0};

    mat4 viewMatrix = GLM_MAT4_IDENTITY_INIT,
         projectionMatrix = GLM_MAT4_IDENTITY_INIT;
    vec3 viewTranslation = {0.0f, 0.0f, -3.0f};

    vec3 cameraPosition = {0.0f, 0.0f, 3.0f},
         cameraFront = {0.0f, 0.0f, -1.0f},
         cameraUp = {0.0f, 1.0f, 0.0f},
         cameraTarget,
         worldUp = {0.0f, 1.0f, 0.0f};

    float deltaTime = 0.0f,
          lastFrame = 0.0f;

    unsigned int modelMatPosition = glGetUniformLocation(shaders->shaderProgram, "modelMatrix"),
                 viewMatPosition = glGetUniformLocation(shaders->shaderProgram, "viewMatrix"),
                 projectionMatPosition = glGetUniformLocation(shaders->shaderProgram, "projectionMatrix");

    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClearColor(0.0f, 0.5f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUniform1i(glGetUniformLocation(shaders->shaderProgram, "texture1"), 0);

        glm_mat4_identity(translationMatrix);
        glm_rotate(translationMatrix, (float)glfwGetTime(), rotationVector);
        glm_translate(translationMatrix, positionVector);
        glm_scale(translationMatrix, scaleVector);

        glm_mat4_identity(translationMatrix2);
        glm_rotate(translationMatrix2, -(float)glfwGetTime(), rotationVector);
        glm_translate(translationMatrix2, positionVector2);
        glm_scale(translationMatrix2, scaleVector2);

        glm_mat4_identity(viewMatrix);
        glm_translate(viewMatrix, viewTranslation);
        glm_perspective(glm_rad(camera->zoom), (float)800 / (float)600, 0.1f, 100.0f, projectionMatrix);

        glUniformMatrix4fv(modelMatPosition, 1, GL_FALSE, (float *)translationMatrix);

        vec3 direction = {
            cos(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch)),
            sin(glm_rad(camera->pitch)),
            sin(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch))};
        glm_normalize_to(direction, cameraFront);

        glm_vec3_add(cameraPosition, cameraFront, cameraTarget);
        glm_lookat(cameraPosition, cameraTarget, cameraUp, viewMatrix);

        glUniformMatrix4fv(viewMatPosition, 1, GL_FALSE, (float *)viewMatrix);
        glUniformMatrix4fv(projectionMatPosition, 1, GL_FALSE, (float *)projectionMatrix);
        glDrawArrays(GL_TRIANGLES, 0, 18);

        glUniformMatrix4fv(glGetUniformLocation(shaders->shaderProgram, "modelMatrix"), 1, GL_FALSE, (float *)translationMatrix2);
        glDrawArrays(GL_TRIANGLES, 0, 18);

        processInput(window, deltaTime, cameraFront, cameraPosition, cameraTarget, cameraUp);

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
        static bool polygon = false;
        float cameraSpeed = 0.25f * deltaTime;

        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        {
            polygon = !polygon;
            glPolygonMode(GL_FRONT_AND_BACK, polygon ? GL_LINE : GL_FILL);
        }

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
            vec3 crossResult;
            glm_vec3_cross(cameraFront, cameraUp, crossResult);
            glm_normalize(crossResult);

            vec3 velocity;
            glm_vec3_scale(crossResult, cameraSpeed, velocity);
            glm_vec3_sub(cameraPosition, velocity, cameraPosition);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            vec3 crossResult;
            glm_vec3_cross(cameraFront, cameraUp, crossResult);
            glm_normalize(crossResult);

            vec3 velocity;
            glm_vec3_scale(crossResult, cameraSpeed, velocity);
            glm_vec3_add(cameraPosition, velocity, cameraPosition);
        }
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

    float xoffset = xpos - camera->lastX;
    float yoffset = camera->lastY - ypos;

    camera->lastX = xpos;
    camera->lastY = ypos;

    const float sensitivity = 0.01f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    camera->yaw += xoffset;
    camera->pitch += yoffset;

    if (camera->pitch > 89.0f)
        camera->pitch = 89.0f;
    if (camera->pitch < -89.0f)
        camera->pitch = -89.0f;
}
void scrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
    Camera *camera = (Camera *)glfwGetWindowUserPointer(window);

    camera->zoom -= (float)yoffset;
    if (camera->zoom < 1.0f)
        camera->zoom = 1.0f;
    if (camera->zoom > 45.0f)
        camera->zoom = 45.0f;
}
char *shaderFile(char *filePath)
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
        perror("Memory");
        return NULL;
    }

    fread(buffer, sizeof(char), fileSize, file);
    fclose(file);

    return buffer;
}
Shaders *createShader(char *vertexFilePath, char *fragmentFilePath)
{
    Shaders *shaders = malloc(sizeof(Shaders));
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
        printf("Vertex error: %s", vertexInfoLog);
        free(shaders);
        return NULL;
    }

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

    int shaderLinkSuccess;
    char shaderProgramLog[BUFSIZ];
    glGetProgramiv(shaders->shaderProgram, GL_LINK_STATUS, &shaderLinkSuccess);
    if (!shaderLinkSuccess)
    {
        glGetProgramInfoLog(shaders->shaderProgram, BUFSIZ, NULL, shaderProgramLog);
        printf("Shader link error: %s", shaderProgramLog);
        free(shaders);
        return NULL;
    }

    return shaders;
}
Camera *createCamera()
{
    Camera *camera = calloc(1, sizeof(Camera));
    if (!camera)
    {
        perror("Memory");
        return NULL;
    }

    camera->pitch = 0.0f;
    camera->zoom = 45.0f;
    camera->yaw = -90.0f;
    camera->firstMouse = true;
    camera->lastX = WINDOW_WIDTH / 2;
    camera->lastY = WINDOW_HEIGHT / 2;

    return camera;
}