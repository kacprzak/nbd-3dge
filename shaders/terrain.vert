#version 330 core

const int MAX_LIGHTS   = 8;
const int MAX_CASCADES = 4;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 lightMVP[MAX_LIGHTS];
uniform mat4 cascadeVP[MAX_CASCADES];

out VS_OUT
{
    float clipZ;
    vec3 position_shadowMap[MAX_CASCADES];
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec2 texCoord;
    vec3 position_w;
    vec3 normal_w;
}
vs_out;

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

uniform Light lights[MAX_LIGHTS];

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

    for (int i = 0; i < MAX_CASCADES; ++i) {
        vs_out.position_shadowMap[i] =
            vec3(biasMatrix * cascadeVP[i] * modelMatrix * vec4(position, 1.0));
    }

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

    vs_out.ambient = sun.ambient;

    vs_out.diffuse = sun.diffuse * max(dot(normal_world, surfaceToLight), 0.0);

    vec3 reflection  = -reflect(surfaceToLight, normal_world);
    vec3 vertexToEye = normalize(-position_eye.xyz);
    vs_out.specular =
        sun.specular * pow(max(dot(vertexToEye, reflection), 0.0), material.shininess);

    gl_Position       = projectionMatrix * position_eye;
    vs_out.texCoord   = in_texCoord;
    vs_out.position_w = position_world.xyz;
    vs_out.normal_w   = normal_world;
    vs_out.clipZ      = gl_Position.z;
}
