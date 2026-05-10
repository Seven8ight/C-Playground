#include "../../Header.h"

// Best for 2D, architectural or engineering applications where we'd rather not have vertices distorted by perspective
void orthographicProjection()
{
    glm_ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f, (void *)0);
}

// Best for 3D
void perspectiveProjection()
{
    // fov,aspect ratio,near plane frustum,far plane of the frustum
    // glm_perspective(glm_rad(45.0f), 800, 600, 0.1f, (void *)0);
    glm_perspective(glm_rad(45.0f), 800 / 600, 0.1f, 100.0f, (void *)0);
}

// Aspect ratio = width / height
void fov()
{
}

int main()
{
}