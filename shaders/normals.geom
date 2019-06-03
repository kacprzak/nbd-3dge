#version 330 core

uniform mat4 projectionMatrix;
uniform vec3 lengths = vec3(1.0, 1.0, 1.0);

layout(triangles) in;
layout(line_strip, max_vertices = 18) out;

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
        generateVector(gl_in[i].gl_Position, gs_in[i].normal, lengths[0], vec3(0.0, 0.0, 1.0));
        generateVector(gl_in[i].gl_Position, gs_in[i].tangent, lengths[1], vec3(1.0, 0.0, 0.0));
        generateVector(gl_in[i].gl_Position, gs_in[i].bitangent, lengths[2], vec3(0.0, 1.0, 0.0));
    }
}
