#version 330 core

in vec3 ModelColors;
in vec3 ModelNormals;
in vec3 FragPosition;

out vec4 FragColors;

uniform vec3 LightColor;
uniform vec3 LightPosition;
uniform vec3 ViewPosition;

void main(){
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * LightColor;

    vec3 lightDirection = normalize(LightPosition - FragPosition);
    vec3 normal = normalize(ModelNormals);
    float diffuseFactor = max(dot(normal, lightDirection), 0.0);
    vec3 diffuse = diffuseFactor * LightColor;

    float specularStrength = 0.3;
    vec3 viewDirection = normalize(ViewPosition - FragPosition);
    vec3 halfway = normalize(lightDirection + viewDirection);
    float specularFactor = pow(max(dot(viewDirection, halfway), 0.0), 32.0);
    vec3 specular = specularStrength * specularFactor * LightColor;

    vec3 lighting = (ambient + diffuse + specular) * ModelColors;
    FragColors = vec4(lighting, 1.0);
}