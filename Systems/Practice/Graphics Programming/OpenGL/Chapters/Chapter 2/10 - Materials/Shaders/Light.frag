#version 330 core

out vec4 FragColors;
uniform vec3 LightColor;

void main(){
    FragColors = vec4(LightColor,1.0);
}