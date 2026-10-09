#version 330 core

struct Material{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Lighting{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 ModelColors;
in vec3 ModelNormals;
in vec3 FragPosition;

out vec4 FragColors;

uniform vec3 LightColor;

uniform vec3 ViewPosition;
uniform Material material;
uniform Lighting lighting;

void main(){
    vec3 ambient = lighting.ambient * material.ambient;

    vec3 normals = normalize(ModelNormals);
    vec3 lightDirection = normalize(lighting.position - FragPosition);
    float diffuseFactor = max(dot(normals,lightDirection),0.0);
    vec3 diffuse = lighting.diffuse * (diffuseFactor * material.diffuse);

    vec3 viewDirection = normalize(ViewPosition - FragPosition);
    vec3 halfway = normalize(lightDirection + viewDirection);
    float specularFactor = pow(max(dot(normals,halfway),0.0),material.shininess);
    vec3 specular = lighting.specular * (specularFactor * material.specular);

    vec3 FullLighting = ambient + diffuse + specular;
    FragColors = vec4(FullLighting,1.0);
}