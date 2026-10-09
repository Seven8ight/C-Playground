#version 330 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 ObjectColors;
in vec3 FragPosition;
in vec3 ObjectNormals;

uniform vec3 viewPosition;

uniform Material material;
uniform Light light;

out vec4 FragColors;

void main(){
    // 1. Ambient
    // Added a 0.2 multiplier so the shadowed sides stay dark
    vec3 ambientLighting = light.ambient * material.ambient;

    // 2. Diffuse
    vec3 normals = normalize(ObjectNormals);
    vec3 lightDirection = normalize(light.position - FragPosition);
    float diffuseFactor = max(dot(normals, lightDirection), 0.0);
    
    // Note: No more ObjectColors multiplication
    vec3 diffuseLighting = light.diffuse * (diffuseFactor * material.diffuse);

    // 3. Specular
    vec3 LookingAtDirection = normalize(viewPosition - FragPosition);
    vec3 reflectionDirection = reflect(-lightDirection, normals);
    float specularFactor = pow(max(dot(LookingAtDirection, reflectionDirection), 0.0), material.shininess);
    
    vec3 specularLighting = light.specular * (specularFactor * material.specular);

    // Final composition: Just add them up!
    vec3 Lighting = ambientLighting + diffuseLighting + specularLighting;
    FragColors = vec4(Lighting, 1.0);
}