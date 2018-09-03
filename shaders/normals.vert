#version 330

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent;

out VS_OUT
{
    vec3 normal;
    vec3 tangent;
}
vs_out;

void main()
{
    gl_Position    = vec4(position, 1.0);
    vs_out.normal  = normal;
    vs_out.tangent = tangent;
}
