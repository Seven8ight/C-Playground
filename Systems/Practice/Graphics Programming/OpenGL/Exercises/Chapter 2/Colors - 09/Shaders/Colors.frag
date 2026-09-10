#version 330 core
out vec4 FragColors;

in vec3 ObjColors;
in vec3 ObjNormals;
in vec3 FragPosition;
in vec3 vertexColor;

uniform vec3 ObjectColor;
uniform vec3 LightColor;
uniform vec3 LightPosition;
uniform vec3 CameraPosition;

void main(){
    // Ambient Lighting
    float ambientStrength = 0.2;
    vec3 ambientLighting = (ambientStrength * LightColor);
    
    // Diffuse Lighting
    // Calculate Light direction Vector, Normalize since we only care for direction and direction requires unit vectors alone
    vec3 normal = normalize(ObjNormals);
    vec3 lightDir = normalize(LightPosition - FragPosition);

    float diffuseFactor = max(dot(normal,lightDir),0.0);
    vec3 diffuseLighting = diffuseFactor * LightColor;

    // Specular Lighting
    float specularStrength = 0.5;

    vec3 viewDirection = normalize(CameraPosition - FragPosition);
    vec3 reflectDirection = reflect(-lightDir,normal);

    float specularFactor = pow(max(dot(viewDirection,reflectDirection),0.0),1);
    vec3 specularLighting = specularStrength * specularFactor * LightColor;

    vec3 Lighting = (ambientLighting + diffuseLighting + specularLighting) * ObjColors;

    FragColors = vec4(Lighting,1.0);
}