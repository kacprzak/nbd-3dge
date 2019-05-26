#version 330 core

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

uniform vec3 lengths = vec3(1.0, 1.0, 1.0);

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

mat4 VP;
mat4 MVP;

const float mag_scale = 0.025;

void generateVector(vec4 position, vec3 vector, float length, vec3 color)
{
    vec4 v =
        (modelMatrix * position) + normalize(modelMatrix * vec4(vector, 0.0)) * length * mag_scale;

    gl_Position  = MVP * position;
    gs_out.color = color;
    EmitVertex();
    gl_Position  = VP * v;
    gs_out.color = color;
    EmitVertex();
    EndPrimitive();
}

void main()
{
    VP  = projectionMatrix * viewMatrix;
    MVP = VP * modelMatrix;

    for (int i = 0; i < gl_in.length(); ++i) {
        generateVector(gl_in[i].gl_Position, gs_in[i].normal, lengths[0], vec3(0.0, 0.0, 1.0));
        generateVector(gl_in[i].gl_Position, gs_in[i].tangent, lengths[1], vec3(1.0, 0.0, 0.0));
        generateVector(gl_in[i].gl_Position, gs_in[i].bitangent, lengths[2], vec3(0.0, 1.0, 0.0));
    }
}
