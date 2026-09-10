#version 330 core 
in vec3 ourColor;
in vec3 colorMix;

out vec4 FragColor;

void main(){
    FragColor = vec4(ourColor + colorMix,1.0);
}