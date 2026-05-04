#version 330 core

in vec3 fragObjColors;
in vec2 fragTexMapping;

out vec4 FragColors;

uniform sampler2D textureMapping;

void main(){
    FragColors = texture(textureMapping,fragTexMapping);
}