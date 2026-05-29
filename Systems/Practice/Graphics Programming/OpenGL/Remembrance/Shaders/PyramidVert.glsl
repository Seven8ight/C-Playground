#version 330 core
layout (location = 0) in vec3 objCoords;
layout (location = 1) in vec3 objColors;

out vec3 ObjColors;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main(){
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(objCoords,1.0);
    ObjColors = objColors;
}