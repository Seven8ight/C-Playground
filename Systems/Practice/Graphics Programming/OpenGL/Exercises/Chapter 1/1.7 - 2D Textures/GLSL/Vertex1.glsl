#version 330 core
layout (location=0) in vec3 pos;
layout (location=1) in vec3 colors;
layout (location=2) in vec2 texture1;
layout (location=3) in vec2 texture2;

out vec2 texCoords1;
out vec2 texCoords2;
out vec3 mixColors;

void main(){
    gl_Position = vec4(pos,1.0);
    texCoords1 = texture1;
    texCoords2 = texture2;
    mixColors = colors;
}