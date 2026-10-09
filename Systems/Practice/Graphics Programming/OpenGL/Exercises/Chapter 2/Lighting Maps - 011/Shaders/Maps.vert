#version 330 core
layout (location = 0) in vec3 ObjPosition;
layout (location = 1) in vec2 ObjTexCoords;
layout (location = 2) in vec3 ObjNormals;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat3 normalsMatrix;
uniform mat4 projectionMatrix;

out vec2 TexCoords;
out vec3 ModelNormals;
out vec3 FragPosition;

void main(){
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(ObjPosition,1.0);
    ModelNormals = ObjNormals * normalsMatrix;
    TexCoords = ObjTexCoords;
    FragPosition = vec3(modelMatrix * vec4(ObjPosition,1.0));
}