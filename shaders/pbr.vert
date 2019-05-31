#version 330

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

uniform mat4 jointMatrices[12];

uniform vec3 weights;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec3 in_tangent;
layout(location = 3) in vec2 in_texCoord_0;
layout(location = 4) in vec4 in_color_0;
layout(location = 5) in vec4 in_joints_0;
layout(location = 6) in vec4 in_weights_0;

layout(location = 7) in vec3 in_morph_0[3];
layout(location = 10) in vec3 in_morph_1[3];
layout(location = 13) in vec3 in_morph_2[3];

out vec4 position;
out vec2 texCoord_0;
out mat3 TBN;

void main()
{
    vec3 pos = in_position + in_morph_0[0] * weights[0] + in_morph_1[0] * weights[1] +
               in_morph_2[0] * weights[2];

    vec3 normal = in_normal + in_morph_0[1] * weights[0] + in_morph_1[1] * weights[1] +
                  in_morph_2[1] * weights[2];

    vec3 tangent = in_tangent + in_morph_0[2] * weights[0] + in_morph_1[2] * weights[1] +
                   in_morph_2[2] * weights[2];

    mat4 skinMatrix =
        in_weights_0.x * jointMatrices[int(in_joints_0.x)] +
        in_weights_0.y * jointMatrices[int(in_joints_0.y)] +
        in_weights_0.z * jointMatrices[int(in_joints_0.z)] +
        in_weights_0.w * jointMatrices[int(in_joints_0.w)];

    vec3 N = normalize(vec3(modelMatrix * skinMatrix * vec4(normal, 0.0)));
    vec3 T = normalize(vec3(modelMatrix * skinMatrix * vec4(tangent, 0.0)));
    vec3 B = cross(N, T);
    TBN    = mat3(T, B, N);

    texCoord_0 = in_texCoord_0;
    // texCoord_0 = vec2(in_texCoord_0.s, 1.0 - in_texCoord_0.t);

    position    = modelMatrix * skinMatrix * vec4(pos, 1.0);
    gl_Position = projectionMatrix * viewMatrix * position;
}
