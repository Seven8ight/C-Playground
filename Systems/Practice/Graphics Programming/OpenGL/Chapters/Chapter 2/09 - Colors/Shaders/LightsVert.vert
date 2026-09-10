#version 330 core
layout (location = 0) in vec3 modelPosition;
layout (location = 1) in vec3 modelColors;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec3 ObjColors;

void main(){
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(modelPosition,1.0);
    ObjColors = modelColors;
}