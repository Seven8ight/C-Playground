#version 330 core
in vec3 ObjColors;

out vec4 FragColors;

void main(){
    FragColors = vec4(ObjColors,1.0);
}