#version 330 core
layout (location = 0) in vec3 objCoords;

void main(){
    gl_Position = vec4(objCoords,1.0);
}