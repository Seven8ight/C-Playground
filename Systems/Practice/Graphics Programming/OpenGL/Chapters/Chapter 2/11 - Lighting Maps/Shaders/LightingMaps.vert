#version 330 core

layout (location = 0) in vec3 ObjPosition;
layout (location = 1) in vec2 ObjTexCoords;
layout (location = 2) in vec3 ObjNorm;

uniform mat4 modelMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat3 modelNormals;

out vec3 ModelNormals;
out vec3 FragPosition;
out vec2 TexCoords;

void main(){
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(ObjPosition,1.0);
    FragPosition = vec3(modelMatrix * vec4(ObjPosition,1.0));
    TexCoords = ObjTexCoords;
    ModelNormals = modelNormals * ObjNorm;
}