#version 330 core
out vec4 FragColors;

in vec3 ObjColors;
in vec3 ObjNormals;
in vec3 FragPosition;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPosition;
uniform vec3 viewPosition;

void main(){
    // Introducing ambient lighting multiply the lightColor with a small ambient light factor
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // Introducing Diffuse Lighting
    vec3 norm = normalize(ObjNormals);
    vec3 lightDir = normalize(lightPosition - FragPosition);

    float diff = max(dot(norm,lightDir),0.0);
    vec3 diffuse = diff * lightColor;

    // Introducing Specular Strength
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPosition - FragPosition);
    vec3 reflectDir = reflect(-lightDir,norm);
    // 32 here is the shininess value of the highlight
    // The higher the shininess value of an object, the more it properly reflects the light
    // instead of scattering it all around and thus the smaller the highlight becomes. 
    float spec = pow(max(dot(viewDir,reflectDir),0.0),32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 lighting = (diffuse + ambient + specular) * objectColor;
    FragColors = vec4(lighting,1.0);
}

