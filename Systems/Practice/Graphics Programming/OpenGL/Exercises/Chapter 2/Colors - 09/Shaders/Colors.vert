#version 330 core
layout (location = 0) in vec3 ModelPosition;
layout (location = 1) in vec3 ModelColors;
layout (location = 2) in vec3 ModelNormals;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalInverse;

out vec3 ObjColors;
out vec3 ObjNormals;
out vec3 FragPosition;
out vec3 vertexColor;

uniform vec3 ObjectColor;
uniform vec3 LightColor;
uniform vec3 LightPosition;
uniform vec3 CameraPosition;

void main(){
    gl_Position = projection * view * model * vec4(ModelPosition,1.0);
    ObjColors = ModelColors;
    ObjNormals = (normalInverse * ModelNormals);
    FragPosition = vec3(model * vec4(ModelPosition,1.0));

    // Ambient Lighting
    float ambientStrength = 0.2;
    vec3 ambientLighting = vec3(1.0,1.0,1.0) * ambientStrength;

    // Diffuse
    vec3 normal = normalize(ObjNormals);
    vec3 lightDirection = normalize(LightPosition - FragPosition);

    float diffuseFactor = max(dot(lightDirection,normal),0.0);
    vec3 diffuseLighting = diffuseFactor * LightColor;

    //Specular
    float specularStrength = 0.5;
    vec3 viewDirection = normalize(CameraPosition - FragPosition);

    vec3 reflectDir = reflect(-lightDirection,normal);

    float specularFactor = pow(max(dot(viewDirection,reflectDir),0.0),32);
    vec3 specularLighting = specularStrength * specularFactor * LightColor;

    vec3 Lighting = (ambientLighting + diffuseLighting + specularLighting) * ObjectColor;
    vertexColor = Lighting;
}