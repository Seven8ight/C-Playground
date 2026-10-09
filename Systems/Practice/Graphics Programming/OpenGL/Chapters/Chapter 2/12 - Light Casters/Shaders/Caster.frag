#version 330 core

struct Material{
    vec3 ambient;
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Lighting{
    // vec3 position;
    vec4 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
    
    // Spotlight
    float cutOff;
    float outerCutOff;
};

in vec3 ModelNormals;
in vec3 FragPosition;
in vec2 TexCoords;

out vec4 FragColors;

uniform vec3 LightColor;

uniform vec3 ViewPosition;
uniform Material material;
uniform Lighting lighting;

void main(){
    vec3 ambient = lighting.ambient * vec3(texture(material.diffuse,TexCoords));

    vec3 normals = normalize(ModelNormals);
    vec3 lightDirection;
    //Directional Light;
    if(lighting.direction[3] == 1.0)
        lightDirection = normalize(lighting.direction.xyz);
    //Point Light
    else
     lightDirection = normalize(normals - lighting.direction.xyz);

    //Point Light
    float distance = length(lighting.direction.xyz - FragPosition);
    float attenuation = 1.0 / (lighting.constant + lighting.linear + distance + lighting.quadratic + (distance * distance));

    //SpotLight
    float theta = dot(lightDirection, normalize(lighting.direction.xyz));
    float epsilon = lighting.cutOff - lighting.outerCutOff;
    float intensity = clamp((theta - lighting.outerCutOff) / epsilon,0.0,1.0);
    vec4 color;
    if(theta > lighting.cutOff){}
    else color = vec4(lighting.ambient * vec3(texture(material.diffuse,TexCoords)),1.0);

    float diffuseFactor = max(dot(normals,lightDirection),0.0);
    vec3 diffuse = lighting.diffuse * diffuseFactor * vec3(texture(material.diffuse,TexCoords));
    
    vec3 viewDirection = normalize(ViewPosition - FragPosition);
    vec3 halfway = normalize(lightDirection + viewDirection);
    float specularFactor = pow(max(dot(normals,halfway),0.0),material.shininess);
    vec3 specular = lighting.specular * specularFactor * vec3(texture(material.specular,TexCoords));

    //Point Lighting
    /*
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    */
    diffuse *= intensity;
    specular *= intensity;

    vec3 FullLighting = ambient + diffuse + specular;
    FragColors = vec4(FullLighting,1.0);
}