#version 330 core

struct Material{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
    // Qn 4: Emission Map
    sampler2D emission;
};

struct Light{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec2 TexCoords;
in vec3 ModelNormals;
in vec3 FragPosition;

out vec4 FragColors;

uniform Material material;
uniform Light light;
uniform vec3 CameraPosition;

void main(){
    vec3 albedo = vec3(texture(material.diffuse, TexCoords));
    vec3 ambientLighting = light.ambient * albedo;

    vec3 normals = normalize(ModelNormals);
    vec3 lightDirection = normalize(light.position - FragPosition);
    float diffuseFactor = max(dot(normals,lightDirection),0.0);
    vec3 diffuseLighting = light.diffuse * diffuseFactor * vec3(texture(material.diffuse,TexCoords));

    
    vec3 viewDirection = normalize(CameraPosition - FragPosition);
    vec3 halfway = normalize(lightDirection + viewDirection);
    float specularFactor = pow(max(dot(normals,halfway),0.0),material.shininess);
    /* Qn 2:
    vec3 specularMap = vec3(1.0) - vec3(texture(material.specular,TexCoords));
    vec3 specularLighting = light.specular * specularFactor * specularMap;
    */
    // vec3 specularMap = vec3(1.0) - vec3(texture(material.specular,TexCoords));
    // vec3 specularLighting = light.specular * specularFactor * specularMap;
    vec3 specularLighting = light.specular * specularFactor * vec3(texture(material.specular,TexCoords));

    //Qn 4: Addision of an emission map
    vec3 emissionMap = vec3(texture(material.emission,TexCoords));
    FragColors = vec4(ambientLighting + diffuseLighting + specularLighting + emissionMap,1.0);
}