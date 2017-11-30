#version 330 core

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 lightMVP;

out vec3 position_shadowMap;
out vec3 ambient;
out vec3 diffuse;
out vec3 specular;
out vec2 texCoord;
out vec3 position_w;
out vec3 normal_w;

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
    const mat4 biasMatrix =
        mat4(0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.5, 0.5, 0.5, 1.0);

    position_shadowMap = vec3(biasMatrix * lightMVP * vec4(position, 1.0));

    Light sun = lights[0];

    vec3 normal_world = normalize(modelMatrix * vec4(in_normal, 0)).xyz;

    vec4 position_world = modelMatrix * vec4(position, 1.0);
    vec4 position_eye   = viewMatrix * position_world;

    vec3 surfaceToLight;
    if (sun.position.w == 0.0) {
        surfaceToLight = normalize(-sun.position.xyz);
    } else {
        surfaceToLight = normalize(sun.position.xyz - position_world.xyz);
    }

    ambient = sun.ambient;

    diffuse = sun.diffuse * max(dot(normal_world, surfaceToLight), 0.0);

    vec3 reflection  = -reflect(surfaceToLight, normal_world);
    vec3 vertexToEye = normalize(-position_eye.xyz);
    specular = sun.specular * pow(max(dot(vertexToEye, reflection), 0.0), material.shininess);

    gl_Position = projectionMatrix * position_eye;
    texCoord    = in_texCoord;
    position_w  = position_world.xyz;
    normal_w    = normal_world;
}
