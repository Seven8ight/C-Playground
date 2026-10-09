#version 330 core
layout (location = 0) in vec3 ObjPosition;
layout (location = 1) in vec2 ObjTexture;
layout (location = 2) in vec3 ObjNormals;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat3 normalsMatrix;

out vec2 TexCoords;
out vec3 FragPosition;
out vec3 ModelNormals;

void main(){
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(ObjPosition,1.0);
    FragPosition = vec3(modelMatrix * vec4(ObjPosition,1.0));
    ModelNormals = normalsMatrix * ObjNormals;   
    TexCoords = ObjTexture;
}