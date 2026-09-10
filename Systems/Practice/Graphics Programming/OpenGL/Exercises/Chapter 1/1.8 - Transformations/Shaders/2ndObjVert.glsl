#version 330 core
layout (location = 0) in vec3 objCoords;
layout (location = 1) in vec3 objColors;
layout (location = 2) in vec2 texCoords;

out vec3 fragObjColors;
out vec2 fragTexMapping;

uniform mat4 transform;

void main(){
    gl_Position = transform * vec4(objCoords,1.0);
    fragObjColors = objColors;
    fragTexMapping = texCoords;
}