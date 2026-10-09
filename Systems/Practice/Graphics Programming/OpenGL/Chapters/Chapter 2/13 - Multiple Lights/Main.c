#define GL_SILENCE_DEPRECATION
#include "../../../Header.h" /* must pull in glad BEFORE glfw, plus cglm, stdio, stdlib, stdbool, math */

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

typedef struct
{
    unsigned int program;
} Shaders;

typedef struct
{
    bool firstMouse;
    double lastX, lastY;
    float zoom, yaw, pitch;
    vec3 position, up, front; /* plain arrays: no malloc, no pointer-to-array confusion */
} Camera;

static int g_fbWidth = WINDOW_WIDTH, g_fbHeight = WINDOW_HEIGHT;

void GLFWInit(void);
void glfw_framebuffer_size_callback(GLFWwindow *window, int width, int height);
void glfw_error_callback(int error, const char *description);
Camera *createCamera(GLFWwindow *window);
char *readFile(const char *filePath);
Shaders *createShaders(const char *vertexFilePath, const char *fragmentFilePath);
unsigned int loadTexture(const char *path);
static GLint uniformLoc(GLuint program, const char *name);
static GLint pointLoc(GLuint program, int i, const char *field);
void KeyboardInput(GLFWwindow *window, float deltaTime);
void MouseInput(GLFWwindow *window, double xPosition, double yPosition);
void ScrollInput(GLFWwindow *window, double xScroll, double yScroll);
void GetViewMatrix(Camera *camera, mat4 view);

int main(void)
{
    GLFWInit();

    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Light Casters", NULL, NULL);
    if (!window)
    {
        fprintf(stderr, "Failed to create window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        fprintf(stderr, "Failed to load GLAD\n");
        return -2;
    }

    glfwGetFramebufferSize(window, &g_fbWidth, &g_fbHeight);
    glfwSetFramebufferSizeCallback(window, glfw_framebuffer_size_callback);
    glViewport(0, 0, g_fbWidth, g_fbHeight);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, MouseInput);
    glfwSetScrollCallback(window, ScrollInput);

    Camera *camera = createCamera(window);
    if (!camera)
        return -3;

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
        {-3.8f, 0.0f, -12.3f}};
    // positions of the point lights
    vec3 pointLightPositions[] = {
        {0.7f, 0.2f, 2.0f},
        {2.3f, -3.3f, -4.0f},
        {-4.0f, 2.0f, -12.0f},
        {0.0f, 0.0f, -3.0f}};

    const int stride = 8 * sizeof(float);

    /* One VBO shared by both VAOs */
    unsigned int vbo, vao, lightVao, textureId, textureId2;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);

    /* World cubes: position, color, normal */
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void *)(3 * sizeof(float)));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void *)(5 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    /* Light cube: position only. Must bind ITS OWN VAO before setting attributes. */
    glGenVertexArrays(1, &lightVao);
    glBindVertexArray(lightVao);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *)0);
    glEnableVertexAttribArray(0);

    Shaders *worldShaders = createShaders("Chapters/Chapter 2/13 - Multiple Lights/Shaders/World.vert",
                                          "Chapters/Chapter 2/13 - Multiple Lights/Shaders/World.frag");
    Shaders *lightShaders = createShaders("Chapters/Chapter 2/13 - Multiple Lights/Shaders/Light.vert",
                                          "Chapters/Chapter 2/13 - Multiple Lights/Shaders/Light.frag");
    if (!worldShaders || !lightShaders)
    {
        fprintf(stderr, "Shader creation failed\n");
        return -4;
    }

    unsigned int diffuseMap = loadTexture("Chapters/Chapter 2/13 - Multiple Lights/Shaders/Image2.jpg");
    unsigned int specularMap = loadTexture("Chapters/Chapter 2/13 - Multiple Lights/Shaders/Image3.png");
    if (!diffuseMap || !specularMap)
        return -5;

    GLuint wp = worldShaders->program;
    GLuint lp = lightShaders->program;

    /* per-frame locations */
    GLint wModel = uniformLoc(wp, "modelMatrix"),
          wView = uniformLoc(wp, "viewMatrix"),
          wProj = uniformLoc(wp, "projectionMatrix"),
          wNormal = uniformLoc(wp, "normalsMatrix"), /* match your .vert */
        wViewPos = uniformLoc(wp, "viewPosition");

    GLint lModel = uniformLoc(lp, "modelMatrix"),
          lView = uniformLoc(lp, "viewMatrix"),
          lProj = uniformLoc(lp, "projectionMatrix");

    /* constant uniforms: set once */
    glUseProgram(wp);
    glUniform1i(uniformLoc(wp, "material.diffuse"), 0);
    glUniform1i(uniformLoc(wp, "material.specular"), 1);
    glUniform1f(uniformLoc(wp, "material.shininess"), 32.0f);

    glUniform3f(uniformLoc(wp, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
    glUniform3f(uniformLoc(wp, "dirLight.ambient"), 0.05f, 0.05f, 0.05f);
    glUniform3f(uniformLoc(wp, "dirLight.diffuse"), 0.4f, 0.4f, 0.4f);
    glUniform3f(uniformLoc(wp, "dirLight.specular"), 0.5f, 0.5f, 0.5f);

    for (int i = 0; i < 4; i++)
    {
        glUniform3fv(pointLoc(wp, i, "position"), 1, pointLightPositions[i]);
        glUniform3f(pointLoc(wp, i, "ambient"), 0.05f, 0.05f, 0.05f);
        glUniform3f(pointLoc(wp, i, "diffuse"), 0.8f, 0.8f, 0.8f);
        glUniform3f(pointLoc(wp, i, "specular"), 1.0f, 1.0f, 1.0f);
        glUniform1f(pointLoc(wp, i, "constant"), 1.0f);
        glUniform1f(pointLoc(wp, i, "linear"), 0.09f);
        glUniform1f(pointLoc(wp, i, "quadratic"), 0.032f);
    }

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    float deltaTime = 0.0f, lastFrame = (float)glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        KeyboardInput(window, deltaTime);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mat4 view, projection;
        GetViewMatrix(camera, view);
        glm_perspective(glm_rad(camera->zoom), (float)g_fbWidth / (float)g_fbHeight, 0.1f, 100.0f, projection);

        /* world cubes */
        glUseProgram(wp);
        glUniformMatrix4fv(wProj, 1, GL_FALSE, (float *)projection);
        glUniformMatrix4fv(wView, 1, GL_FALSE, (float *)view);
        glUniform3fv(wViewPos, 1, camera->position);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        glBindVertexArray(vao);
        vec3 rotateAxis = {1.0f, 0.3f, 0.5f};
        for (int i = 0; i < 4; i++)
        {
            mat4 model;
            mat3 normals;

            glm_mat4_identity(model);
            glm_translate(model, cubePositions[i]);
            glm_rotate(model, (float)glfwGetTime(), rotateAxis);

            glm_mat4_pick3(model, normals);
            glm_mat3_inv(normals, normals);
            glm_mat3_transpose(normals);

            glUniformMatrix4fv(wModel, 1, GL_FALSE, (float *)model);
            glUniformMatrix3fv(wNormal, 1, GL_FALSE, (float *)normals);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        /* light cubes: one per point light */
        glUseProgram(lp);
        glUniformMatrix4fv(lProj, 1, GL_FALSE, (float *)projection);
        glUniformMatrix4fv(lView, 1, GL_FALSE, (float *)view);

        glBindVertexArray(lightVao);
        for (int i = 0; i < 4; i++)
        {
            mat4 lightModel;
            glm_mat4_identity(lightModel);
            glm_translate(lightModel, pointLightPositions[i]);
            glm_scale_uni(lightModel, 0.2f);
            glUniformMatrix4fv(lModel, 1, GL_FALSE, (float *)lightModel);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteVertexArrays(1, &lightVao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(worldShaders->program);
    glDeleteProgram(lightShaders->program);
    free(worldShaders);
    free(lightShaders);
    free(camera);
    glfwTerminate();
    return 0;
}

void GLFWInit(void)
{
    glfwSetErrorCallback(glfw_error_callback); /* before glfwInit so init errors are reported */
    if (!glfwInit())
    {
        fprintf(stderr, "glfwInit failed\n");
        exit(EXIT_FAILURE);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
}

void glfw_framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    (void)window;
    g_fbWidth = width;
    g_fbHeight = height;
    glViewport(0, 0, width, height);
}

void glfw_error_callback(int error, const char *description)
{
    fprintf(stderr, "GLFW ERROR %d: %s\n", error, description);
}

Camera *createCamera(GLFWwindow *window)
{
    Camera *camera = calloc(1, sizeof(Camera));
    if (!camera)
    {
        perror("Camera memory");
        return NULL;
    }

    camera->firstMouse = true;
    camera->zoom = 45.0f;
    camera->lastX = WINDOW_WIDTH / 2.0;
    camera->lastY = WINDOW_HEIGHT / 2.0;
    camera->yaw = -90.0f;
    camera->pitch = 0.0f;

    glm_vec3_copy((vec3){0.0f, 0.0f, 3.0f}, camera->position); /* +Z so the cube at the origin is in front */
    glm_vec3_copy((vec3){0.0f, 0.0f, -1.0f}, camera->front);
    glm_vec3_copy((vec3){0.0f, 1.0f, 0.0f}, camera->up);

    glfwSetWindowUserPointer(window, camera);
    return camera;
}

char *readFile(const char *filePath)
{
    FILE *file = fopen(filePath, "rb");
    if (!file)
    {
        perror(filePath);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = calloc(fileSize + 1, sizeof(char));
    if (!buffer)
    {
        perror("File read buffer");
        fclose(file);
        return NULL;
    }

    size_t read = fread(buffer, 1, fileSize, file); /* was sizeof buffer (8 bytes!) */
    buffer[read] = '\0';

    fclose(file);
    return buffer;
}

static unsigned int compileShader(GLenum type, const char *source, const char *label)
{
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL); /* needs const char *const *, i.e. &source */
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char log[BUFSIZ];
        glGetShaderInfoLog(shader, BUFSIZ, NULL, log);
        fprintf(stderr, "%s shader error: %s\n", label, log);
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

Shaders *createShaders(const char *vertexFilePath, const char *fragmentFilePath)
{
    char *vertexSource = readFile(vertexFilePath);
    char *fragmentSource = readFile(fragmentFilePath);
    if (!vertexSource || !fragmentSource)
    {
        free(vertexSource);
        free(fragmentSource);
        return NULL;
    }

    unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexSource, "Vertex");
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentSource, "Fragment");
    free(vertexSource);
    free(fragmentSource);

    if (!vs || !fs)
    {
        if (vs)
            glDeleteShader(vs);
        if (fs)
            glDeleteShader(fs);
        return NULL;
    }

    unsigned int program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        char log[BUFSIZ];
        glGetProgramInfoLog(program, BUFSIZ, NULL, log); /* was printing the fragment log */
        fprintf(stderr, "Shader program error: %s\n", log);
        glDeleteProgram(program);
        return NULL;
    }

    Shaders *shaders = calloc(1, sizeof(Shaders));
    if (!shaders)
    {
        perror("Shader memory");
        glDeleteProgram(program);
        return NULL;
    }
    shaders->program = program;
    return shaders;
}

unsigned int loadTexture(const char *path)
{
    unsigned int id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int w, h, ch;
    unsigned char *data = stbi_load(path, &w, &h, &ch, 0);
    if (!data)
    {
        fprintf(stderr, "Texture failed: %s\n", path);
        glDeleteTextures(1, &id);
        return 0;
    }

    GLenum fmt = ch == 1 ? GL_RED : ch == 3 ? GL_RGB
                                            : GL_RGBA;
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
    return id;
}

void KeyboardInput(GLFWwindow *window, float deltaTime)
{
    Camera *camera = glfwGetWindowUserPointer(window);
    if (!camera)
        return;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    float speed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
        speed *= 2;

    vec3 velocity, right;
    glm_vec3_cross(camera->front, camera->up, right); /* front x up = right (was up x front = left) */
    glm_vec3_normalize(right);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        glm_vec3_scale(camera->front, speed, velocity); /* move along front, not along position */
        glm_vec3_add(camera->position, velocity, camera->position);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        glm_vec3_scale(camera->front, speed, velocity);
        glm_vec3_sub(camera->position, velocity, camera->position);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        glm_vec3_scale(right, speed, velocity);
        glm_vec3_sub(camera->position, velocity, camera->position);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        glm_vec3_scale(right, speed, velocity);
        glm_vec3_add(camera->position, velocity, camera->position);
    }
}

void MouseInput(GLFWwindow *window, double xPosition, double yPosition)
{
    Camera *camera = glfwGetWindowUserPointer(window);
    if (!camera)
        return;

    if (camera->firstMouse)
    {
        camera->lastX = xPosition;
        camera->lastY = yPosition;
        camera->firstMouse = false;
        return;
    }

    double xOffset = xPosition - camera->lastX;
    double yOffset = camera->lastY - yPosition; /* reversed: window y grows downward */
    camera->lastX = xPosition;
    camera->lastY = yPosition;

    const float sensitivity = 0.1f;
    camera->yaw += (float)(xOffset * sensitivity);
    camera->pitch += (float)(yOffset * sensitivity); /* was += xOffset */

    if (camera->pitch > 89.0f)
        camera->pitch = 89.0f;
    if (camera->pitch < -89.0f)
        camera->pitch = -89.0f;

    vec3 direction = {
        cosf(glm_rad(camera->yaw)) * cosf(glm_rad(camera->pitch)),
        sinf(glm_rad(camera->pitch)),
        sinf(glm_rad(camera->yaw)) * cosf(glm_rad(camera->pitch))};
    glm_vec3_normalize_to(direction, camera->front);
}

void ScrollInput(GLFWwindow *window, double xScroll, double yScroll)
{
    (void)xScroll;
    Camera *camera = glfwGetWindowUserPointer(window);
    if (!camera)
        return;

    camera->zoom -= (float)yScroll;
    if (camera->zoom > 45.0f)
        camera->zoom = 45.0f;
    if (camera->zoom < 1.0f)
        camera->zoom = 1.0f;
}

static GLint uniformLoc(GLuint program, const char *name)
{
    GLint loc = glGetUniformLocation(program, name);
    if (loc < 0)
        fprintf(stderr, "uniform not found: %s\n", name);
    return loc;
}

static GLint pointLoc(GLuint program, int i, const char *field)
{
    char name[64];
    snprintf(name, sizeof name, "pointLights[%d].%s", i, field);
    return uniformLoc(program, name);
}

void GetViewMatrix(Camera *camera, mat4 view)
{
    vec3 center;
    glm_vec3_add(camera->position, camera->front, center);
    glm_lookat(camera->position, center, camera->up, view);
}