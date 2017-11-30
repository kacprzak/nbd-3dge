#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 in_texCoord;
layout(location = 2) in vec3 in_normal;
layout(location = 3) in vec3 in_tangent;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 lightMVP;

out VS_OUT
{
    vec3 position_shadowMap;
    vec3 position_w;
    vec3 position_v;
    vec2 texCoord;
    mat3 TBN;
}
vs_out;

void main()
{
    const mat4 biasMatrix =
        mat4(0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.5, 0.5, 0.5, 1.0);

    vs_out.position_shadowMap = vec3(biasMatrix * lightMVP * vec4(position, 1.0));

    vec3 N     = normalize(vec3(modelMatrix * vec4(in_normal, 0.0)));
    vec3 T     = normalize(vec3(modelMatrix * vec4(in_tangent, 0.0)));
    vec3 B     = cross(T, N);
    vs_out.TBN = mat3(T, B, N);

    vs_out.position_w = vec3(modelMatrix * vec4(position, 1.0));
    vs_out.position_v = vec3(viewMatrix * vec4(vs_out.position_w, 1.0));
    vs_out.texCoord   = in_texCoord;
    gl_Position       = projectionMatrix * vec4(vs_out.position_v, 1.0);
    // normal = in_normal;
    // tangent = in_tangent;
}
