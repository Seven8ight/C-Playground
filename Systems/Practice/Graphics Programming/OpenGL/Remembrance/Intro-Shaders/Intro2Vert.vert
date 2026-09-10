#version 330 core

layout (location = 0) in vec3 objPosition;
layout (location = 1) in vec3 objColors;
layout (location = 2) in vec2 objTexture;

uniform vec3 blendColors;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec3 mixColors;
out vec2 TexCoords;

void main(){
    gl_Position =  projectionMatrix * viewMatrix * modelMatrix * vec4(objPosition,1.0);
    mixColors = objColors + blendColors;
    TexCoords = objTexture;
}