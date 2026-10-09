#version 330 core
layout (location = 0) in vec3 ObjPosition;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat3 modelNormals;

void main(){
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(ObjPosition,1.0);
}