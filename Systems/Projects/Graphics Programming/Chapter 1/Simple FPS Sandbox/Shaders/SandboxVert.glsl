#version 330 core
layout(location = 0) in vec3 ObjCoords;
layout(location = 1) in vec3 ObjColors;

out vec3 Colors;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main(){
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(ObjCoords,1.0);
    Colors = ObjColors;
}