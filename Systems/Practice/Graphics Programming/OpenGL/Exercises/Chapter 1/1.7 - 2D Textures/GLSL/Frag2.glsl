#version 330 core
out vec4 FragColor;

in vec2 texCoords1;
in vec2 texCoords2;
in vec3 mixColors;

uniform float visibility;
uniform sampler2D ourTexture;
uniform sampler2D ourTexture2;

void main(){
    FragColor = mix(texture(ourTexture,texCoords1),texture(ourTexture2,texCoords2),visibility);
}