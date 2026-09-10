#version 330 core
layout (location = 0) in vec3 modelPosition;
layout (location = 1) in vec3 modelColors;
layout (location = 2) in vec3 modelNormals;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat3 normalsInverse;

out vec3 ObjColors;
out vec3 ObjNormals;
out vec3 FragPosition;

void main(){
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(modelPosition,1.0);
    ObjColors = modelColors;
    // ObjNormals = modelNormals;
    ObjNormals = normalsInverse * modelNormals; // costly operation, ensure to calculate from CPU before passing it to GPU
    FragPosition = vec3(modelMatrix * vec4(modelPosition,1.0));
}