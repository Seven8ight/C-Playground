#version 330 core
layout (location = 0) in vec3 ObjPos;
layout (location = 1) in vec3 ObjColors;
layout (location = 2) in vec3 ObjNormals;

out vec3 ModelColors;
out vec3 ModelNormals;
out vec3 FragPosition;

uniform mat4 modelMatrix;
uniform mat3 modelNormals;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main(){
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(ObjPos,1.0);
    ModelColors = ObjColors;
    ModelNormals = modelNormals * ObjNormals;
    FragPosition = vec3(modelMatrix * vec4(ObjPos,1.0));
}