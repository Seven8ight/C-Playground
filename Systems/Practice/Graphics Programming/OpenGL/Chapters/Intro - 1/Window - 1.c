#include "../../Header.h"

void GLFWInit();
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}
void processInput(GLFWwindow *window);

int main()
{
    // GLFW initialization
    GLFWInit();

    // Window creation
    char title[] = "Window";

    GLFWwindow *window = glfwCreateWindow(800, 600, title, NULL, NULL);
    if (!window)
        return -1;

    // Handle window resizing
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // Thread initialization, making the program execution the main thread
    glfwMakeContextCurrent(window);

    // Glad initialization - for opengl pointers necessary for OS specialization
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        perror("GLAD");
        return -1;
    }

    glViewport(0, 0, 800, 600);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.5f, 0.4f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void GLFWInit()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
}
void processInput(GLFWwindow *window)
{
    if (!window)
        return;

    // If not pressed returns GLFW_RELEASE
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    static int count = 0;

    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
        printf("Y key pressed: %d", count++);
}