#version 330 core
out vec4 FragColors;

in vec3 objColors;
in vec2 objTexture;

uniform sampler2D ourTexture;

void main(){
    FragColors = texture(ourTexture, objTexture);
}