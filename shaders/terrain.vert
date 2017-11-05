#version 330 core

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

out vec3 ambient;
out vec3 diffuse;
out vec3 specular;
out vec2 texCoord;
out vec3 normal;
out float height;

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 in_texCoord;
layout(location = 2) in vec3 in_normal;

struct Light
{
    vec4 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light lights[8];

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Material material;

void main()
{
    Light sun = lights[0];
    
    vec3 normal_world = normalize(modelMatrix * vec4(in_normal, 0)).xyz;
    
    vec4 position_world = modelMatrix * vec4(position, 1.0);
    vec4 position_eye = viewMatrix * position_world;

    vec3 surfaceToLight;
    if (sun.position.w == 0.0) {
        surfaceToLight = normalize(-sun.position.xyz);
    }
    else {
        surfaceToLight = normalize(sun.position.xyz - position_world.xyz);
    }

    ambient = sun.ambient;
    
    diffuse = sun.diffuse * max(dot(normal_world, surfaceToLight), 0.0);

    vec3 reflection = -reflect(surfaceToLight, normal_world);
    vec3 vertexToEye = normalize(-position_eye.xyz);
    specular = sun.specular * pow(max(dot(vertexToEye, reflection), 0.0), material.shininess);

    gl_Position = projectionMatrix * position_eye;
    texCoord = in_texCoord;
    normal = normal_world;
    height = position_world.y;
}
