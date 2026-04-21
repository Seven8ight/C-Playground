#version 330 core
in vec3 colors;
in vec2 textureCoords;

out vec4 FragColors;

uniform sampler2D ourTexture;
uniform sampler2D ourTexture2;

void main(){
    // FragColors = texture(ourTexture,textureCoords);
    FragColors = mix(texture(ourTexture, textureCoords),texture(ourTexture2, textureCoords),0.5);
}