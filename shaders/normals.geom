#version 330 core

uniform mat4 projectionMatrix;
uniform vec4 lengths = vec4(1.0, 1.0, 1.0, 1.0); // TBN, vertN

layout(triangles) in;
layout(line_strip, max_vertices = 12) out;

in VS_OUT
{
    vec3 normal;
    vec3 tangent;
    vec3 bitangent;
}
gs_in[];

out GS_OUT { vec3 color; }
gs_out;

const float mag_scale = 0.05;

void generateVector(vec4 position, vec3 vector, float len, vec3 color)
{
    gl_Position  = projectionMatrix * position;
    gs_out.color = color;
    EmitVertex();
    gl_Position  = projectionMatrix * (position + vec4(vector * len * mag_scale, 0.0));
    gs_out.color = color;
    EmitVertex();
    EndPrimitive();
}

void main()
{
    for (int i = 0; i < gl_in.length(); ++i) {
        generateVector(gl_in[i].gl_Position, gs_in[i].normal, lengths[3], vec3(0.0, 1.0, 1.0));
    }

    vec4 p = (gl_in[0].gl_Position + gl_in[1].gl_Position + gl_in[2].gl_Position) / 3.0;
    vec3 n = (gs_in[0].normal + gs_in[1].normal + gs_in[2].normal) / 3.0;
    vec3 t = (gs_in[0].tangent + gs_in[1].tangent + gs_in[2].tangent) / 3.0;
    vec3 b = (gs_in[0].bitangent + gs_in[1].bitangent + gs_in[2].bitangent) / 3.0;

    generateVector(p, n, lengths[0], vec3(0.0, 0.0, 1.0));
    generateVector(p, t, lengths[1], vec3(1.0, 0.0, 0.0));
    generateVector(p, b, lengths[2], vec3(0.0, 1.0, 0.0));
}
