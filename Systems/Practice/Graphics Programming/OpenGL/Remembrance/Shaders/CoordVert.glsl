#version 330 core

layout(location = 0) in vec3 ObjCoords;
layout(location = 1) in vec3 ObjColors;
layout(location = 2) in vec2 ObjTexCoords;

out vec2 texCoords;
out vec3 fragColors;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main(){
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(ObjCoords,1.0);
    texCoords = ObjTexCoords;
    fragColors = ObjColors;
}