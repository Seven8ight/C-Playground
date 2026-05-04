#version 330 core
out vec4 FragColor;

uniform vec4 RenderColor;

void main(){
    FragColor = RenderColor;
}