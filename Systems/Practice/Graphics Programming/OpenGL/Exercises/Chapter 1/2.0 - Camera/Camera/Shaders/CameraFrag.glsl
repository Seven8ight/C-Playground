#version 330 core
out vec4 FragColors;

in vec2 texCoords;

uniform sampler2D texture1;

void main(){
    FragColors = texture(texture1,texCoords);
}