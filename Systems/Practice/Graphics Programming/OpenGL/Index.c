#include "Header.h"

void GLFWInit();
GLFWwindow *createWindow(int width, int height, char *windowTitle);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

int main()
{
    // GLFW initialization
    GLFWInit();

    // Window creation
    char title[] = "Window";

    GLFWwindow *window = createWindow(800, 600, title);
    if (!window)
        return -1;

    // Handle window resizing
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // Thread intialization, making the program execution the main thread
    glfwMakeContextCurrent(window);

    // Glad initialization - for opengl pointers necessary for OS specialization
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        perror("GLAD");
        return -1;
    }

    // Coordinates for origin within the window to start placing items in the window
    glViewport(0, 0, 800, 600);

    // Ensures the window is kept open unless requested to close(pressing close button) - render loop
    // Each iteration loop for rendering is called a frame(frame update)
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        // Swaps color buffer used in rendering and show its output to the screen during state update
        glfwSwapBuffers(window);
        // Ensures all triggers are recorded and state is updated(keyboard and mouse and anything else) and callbacks are called
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
void processInput(GLFWwindow *window)
{
    // If not pressed returns GLFW_RELEASE
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
GLFWwindow *createWindow(int width, int height, char *windowTitle)
{
    GLFWwindow *window = glfwCreateWindow(width, height, windowTitle, NULL, NULL);

    if (!window)
    {
        perror("Memory");
        return NULL;
    }

    return window;
}