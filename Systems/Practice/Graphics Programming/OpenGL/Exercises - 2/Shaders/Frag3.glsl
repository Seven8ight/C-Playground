#version 330 core
in vec3 fragColors;

out vec4 FragColor;

void main(){
    FragColor = vec4(fragColors,1.0);
}