#version 330 core

layout (location = 0) in vec3 objPos;
layout (location = 1) in vec3 objColor;
layout (location = 2) in vec2 objTexture;

out vec3 colors;
out vec2 textureCoords;

void main(){
    gl_Position = vec4(objPos,1.0);
    colors = objColor;
    textureCoords = objTexture;
}