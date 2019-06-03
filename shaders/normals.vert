#version 330

uniform mat4 modelViewMatrix;
uniform mat3 normalMatrix;

uniform mat4 jointMatrices[20];

uniform vec3 weights;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec4 in_tangent;

layout(location = 5) in vec4 in_joints_0;
layout(location = 6) in vec4 in_weights_0;

layout(location = 7) in vec3 in_morph_0[3];
layout(location = 10) in vec3 in_morph_1[3];
layout(location = 13) in vec3 in_morph_2[3];

out VS_OUT
{
    vec3 normal;
    vec3 tangent;
    vec3 bitangent;
}
vs_out;

void main()
{
    vec3 pos = in_position + in_morph_0[0] * weights[0] + in_morph_1[0] * weights[1] +
               in_morph_2[0] * weights[2];

    vec3 normal = in_normal + in_morph_0[1] * weights[0] + in_morph_1[1] * weights[1] +
                  in_morph_2[1] * weights[2];

    vec3 tangent = in_tangent.xyz + in_morph_0[2] * weights[0] + in_morph_1[2] * weights[1] +
                   in_morph_2[2] * weights[2];

    mat4 skinMatrix = in_weights_0.x * jointMatrices[int(in_joints_0.x)] +
                      in_weights_0.y * jointMatrices[int(in_joints_0.y)] +
                      in_weights_0.z * jointMatrices[int(in_joints_0.z)] +
                      in_weights_0.w * jointMatrices[int(in_joints_0.w)];

    vs_out.normal    = normalize(normalMatrix * mat3(skinMatrix) * normal);
    vs_out.tangent   = normalize(normalMatrix * mat3(skinMatrix) * tangent);
    vs_out.bitangent = normalize(cross(vs_out.normal, vs_out.tangent) * in_tangent.w);
    gl_Position      = modelViewMatrix * skinMatrix * vec4(pos, 1.0);
}
