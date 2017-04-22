#version 330

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

layout(location = 0) in vec3 position;
layout(location = 2) in vec3 normal;

out VS_OUT {
    vec3 normal;
} vs_out;

void main()
{
    mat4 mvp =  viewMatrix * modelMatrix;
    gl_Position = projectionMatrix * mvp * vec4(position, 1.0);
    vs_out.normal = normalize(vec3(mvp * vec4(normal, 0.0)));
}
