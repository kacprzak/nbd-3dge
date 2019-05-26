#version 330

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

uniform vec3 weights;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec3 in_tangent;
layout(location = 3) in vec2 in_texCoord_0;
layout(location = 4) in vec2 in_texCoord_1;
layout(location = 5) in vec2 in_texCoord_2;
layout(location = 6) in vec2 in_texCoord_3;

layout(location = 7) in vec3 in_position_morph_0;
layout(location = 8) in vec3 in_normal_morph_0;
layout(location = 9) in vec3 in_tangent_morph_0;

layout(location = 10) in vec3 in_position_morph_1;
layout(location = 11) in vec3 in_normal_morph_1;
layout(location = 12) in vec3 in_tangent_morph_1;

layout(location = 13) in vec3 in_position_morph_2;
layout(location = 14) in vec3 in_normal_morph_2;
layout(location = 15) in vec3 in_tangent_morph_2;

out vec4 position;
out vec2 texCoord_0;
out mat3 TBN;

void main()
{
    vec3 pos = in_position + in_position_morph_0 * weights[0] + in_position_morph_1 * weights[1] +
               in_position_morph_2 * weights[2];

    vec3 normal = in_normal + in_normal_morph_0 * weights[0] + in_normal_morph_1 * weights[1] +
                  in_normal_morph_2 * weights[2];

    vec3 tangent = in_tangent + in_tangent_morph_0 * weights[0] + in_tangent_morph_1 * weights[1] +
                   in_tangent_morph_2 * weights[2];

    vec3 N = normalize(vec3(modelMatrix * vec4(normal, 0.0)));
    vec3 T = normalize(vec3(modelMatrix * vec4(tangent, 0.0)));
    vec3 B = cross(N, T);
    TBN    = mat3(T, B, N);

    texCoord_0 = in_texCoord_0;
    // texCoord_0 = vec2(in_texCoord_0.s, 1.0 - in_texCoord_0.t);

    position    = modelMatrix * vec4(pos, 1.0);
    gl_Position = projectionMatrix * viewMatrix * position;
}
