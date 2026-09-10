#version 330 core

in vec3 mixColors;
in vec2 TexCoords;
out vec4 FragColors;

uniform sampler2D ourTexture;

void main(){
    FragColors = texture(ourTexture,TexCoords);
}