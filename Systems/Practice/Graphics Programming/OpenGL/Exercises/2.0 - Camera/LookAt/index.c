#include "../../../Header.h"

mat4 *lookAt(vec3 cameraPosition, vec3 cameraDirection, vec3 cameraUp, vec3 cameraRight)
{
    static mat4 lookat = GLM_MAT4_IDENTITY_INIT;
    mat4 cameraTranslation = GLM_MAT4_IDENTITY_INIT;
    mat4 cameraAxes = GLM_MAT4_IDENTITY_INIT;

    cameraTranslation[0][3] = -cameraPosition[0];
    cameraTranslation[1][3] = -cameraPosition[1];
    cameraTranslation[2][3] = -cameraPosition[2];

    cameraAxes[0][0] = cameraRight[0];
    cameraAxes[1][0] = cameraRight[1];
    cameraAxes[2][0] = cameraRight[2];

    cameraAxes[0][1] = cameraUp[0];
    cameraAxes[1][1] = cameraUp[1];
    cameraAxes[2][1] = cameraUp[2];

    cameraAxes[0][2] = cameraDirection[0];
    cameraAxes[1][2] = cameraDirection[1];
    cameraAxes[2][2] = cameraDirection[2];

    glm_mat4_mul(cameraAxes, cameraTranslation, lookat);

    return &lookat;
}

int main(void)
{
    return 0;
}