#version 330

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

uniform vec3 weights;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec3 in_tangent;

layout(location = 7) in vec3 in_morph_0[3];
layout(location = 10) in vec3 in_morph_1[3];
layout(location = 13) in vec3 in_morph_2[3];

out VS_OUT
{
    vec3 normal;
    vec3 tangent;
}
vs_out;

void main()
{
    vec3 pos = in_position + in_morph_0[0] * weights[0] + in_morph_1[0] * weights[1] +
               in_morph_2[0] * weights[2];

    vec3 normal = in_normal + in_morph_0[1] * weights[0] + in_morph_1[1] * weights[1] +
                  in_morph_2[1] * weights[2];

    vec3 tangent = in_tangent + in_morph_0[2] * weights[0] + in_morph_1[2] * weights[1] +
                   in_morph_2[2] * weights[2];

    vs_out.normal  = normal;
    vs_out.tangent = tangent;
    gl_Position    = vec4(pos, 1.0);
}
