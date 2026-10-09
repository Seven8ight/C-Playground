#version 330 core
layout(location = 0) in vec3 ObjPosition;
layout(location = 1) in vec3 ObjColors;
layout(location = 2) in vec3 ObjNormals;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat3 normalsInverse;

out vec3 FragPosition;
out vec3 ObjectNormals;
out vec3 ObjectColors;

void main(){
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(ObjPosition,1.0);

    ObjectNormals = (ObjNormals * normalsInverse);
    ObjectColors = ObjColors;

    FragPosition = vec3(modelMatrix * vec4(ObjPosition,1.0));
}