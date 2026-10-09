#version 330 core
layout (location = 0) in vec3 ObjectPosition;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main(){
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(ObjectPosition,1.0);
}