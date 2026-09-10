#version 330 core
out vec4 FragColor;

in vec2 texCoords1;
in vec2 texCoords2;
in vec3 mixColors;

uniform sampler2D ourTexture;
uniform sampler2D ourTexture2;

void main(){
    // FragColor = texture(ourTexture,texCoords1) * vec4(mixColors,1.0);
    FragColor = mix(texture(ourTexture,texCoords1),texture(ourTexture2,texCoords2),0.75);
}