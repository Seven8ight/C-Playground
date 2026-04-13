#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 colors;
layout (location = 2) in vec2 texture;

out vec3 objColors;
out vec2 objTexture;

void main(){
    gl_Position = vec4(position,1.0);

    objColors = colors;
    objTexture = texture;
}