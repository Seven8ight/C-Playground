#version 330 core
layout (location = 0) in vec3 objPosition;
layout (location = 1) in vec3 mixColors;

out vec3 ourColor;
out vec3 colorMix;

uniform vec3 objColor;

void main(){
    gl_Position = vec4(objPosition,1.0);
    ourColor = objColor;
    colorMix = mixColors;
}