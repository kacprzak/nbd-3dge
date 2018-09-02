#version 330

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec3 in_tangent;
layout(location = 3) in vec2 in_texCoord_0;

out vec4 position;
out vec2 texCoord_0;
out mat3 TBN;

void main()
{
    vec3 N   = normalize(vec3(modelMatrix * vec4(in_normal, 0.0)));
    vec3 T   = normalize(vec3(modelMatrix * vec4(in_tangent, 0.0)));
    vec3 B   = cross(N, T);
    TBN = mat3(T, B, N);

    texCoord_0 = vec2(in_texCoord_0.s, 1.0 - in_texCoord_0.t);

    position    = modelMatrix * vec4(in_position, 1.0);
    gl_Position = projectionMatrix * viewMatrix * position;
}
