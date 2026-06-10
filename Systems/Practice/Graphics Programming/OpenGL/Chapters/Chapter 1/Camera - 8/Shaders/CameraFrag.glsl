#version 330 core
in vec2 texCoords;

out vec4 FragColors;

uniform sampler2D texture1;

void main(){
    FragColors = texture(texture1,texCoords);
}