#version 330 core

out vec4 FragColors;

in vec2 TexCoords;
in vec3 FragPosition;
in vec3 ModelNormals;

struct DirectionalLight{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight{
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

uniform vec3 viewPosition;
uniform Material material;

#define NR_POINT_LIGHTS 4
uniform DirectionalLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];

vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir){
    vec3 LightDirection = normalize(light.direction);

    float diff = max(dot(normal,LightDirection),0.0);
    
    vec3 reflectDir = reflect(-LightDirection,normal);
    float spec = pow(max(dot(reflectDir,viewDir),0.0),material.shininess);

    vec3 ambient = light.ambient * vec3(texture(material.diffuse,TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse,TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular,TexCoords));

    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light,vec3 normal, vec3 FragPosition,vec3 viewDirection){
    vec3 LightDirection = normalize(light.position - FragPosition);

    float diff = max(dot(normal,LightDirection),0.0);
    
    vec3 reflectDir = reflect(-LightDirection,normal);
    float spec = pow(max(dot(reflectDir,viewDirection),0.0),material.shininess);

    float distance = length(light.position - FragPosition);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec3 ambient = light.ambient * vec3(texture(material.diffuse,TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse,TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular,TexCoords));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

void main(){
    vec3 norm = normalize(ModelNormals);
    vec3 viewDirection = normalize(viewPosition - FragPosition);

    vec3 result = CalcDirLight(dirLight,norm,viewDirection);
    
    for(int i = 0;i < NR_POINT_LIGHTS;i++)
        result += CalcPointLight(pointLights[i],norm,FragPosition,viewDirection);
    
    FragColors = vec4(result,1.0);
}

