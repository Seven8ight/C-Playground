#include "../../Header.h"

void Translation();
void Rotation();

int main()
{
    Translation();
    return 0;
}

void Translation()
{
    vec4 originalPosition = {1.0f, 0.0f, 0.0f, 1.0f};
    vec3 translationPositions = {1.0f, 1.0f, 0.0f};
    mat4 trans = GLM_MAT4_IDENTITY_INIT;

    glm_translate(trans, translationPositions);

    vec4 result;
    glm_mat4_mulv(trans, originalPosition, result);

    printf("%f %f %f %f\n", result[0], result[1], result[2], result[3]);
}
void Rotation()
{
    mat4 trans = GLM_MAT4_IDENTITY_INIT;
    vec3 positions = {0.0, 0.0, 1.0};

    glm_rotate(trans, glm_rad(90.0f), positions);

    vec3 scaleFactor = {0.5, 0.5, 0.5};
    glm_scale(trans, scaleFactor);
}