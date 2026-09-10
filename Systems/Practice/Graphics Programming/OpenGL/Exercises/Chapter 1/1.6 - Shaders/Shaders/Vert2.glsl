#version 330 core
layout (location = 0) in vec3 aPos;

uniform float horizontalOffset;

void main(){
    gl_Position=vec4(aPos.x + horizontalOffset,aPos.y,0.0,1.0);
}