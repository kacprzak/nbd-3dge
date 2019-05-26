#version 330 core

layout(triangles) in;
layout(line_strip, max_vertices = 12) out;

in VS_OUT
{
    vec3 normal;
    vec3 tangent;
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

void generateNormal(int index)
{
    vec4 pos  = gl_in[index].gl_Position;
    vec4 norm = (modelMatrix * pos) +
                normalize(modelMatrix * vec4(gs_in[index].normal, 0.0)) * lengths[0] * mag_scale;

    gl_Position  = MVP * pos;
    gs_out.color = vec3(0.0, 0.0, 1.0);
    EmitVertex();
    gl_Position  = VP * norm;
    gs_out.color = vec3(0.0, 0.0, 1.0);
    EmitVertex();
    EndPrimitive();
}

void generateTangent(int index)
{
    vec4 pos  = gl_in[index].gl_Position;
    vec4 tang = (modelMatrix * pos) +
                normalize(modelMatrix * vec4(gs_in[index].tangent, 0.0)) * lengths[1] * mag_scale;

    gl_Position  = MVP * pos;
    gs_out.color = vec3(1.0, 0.0, 0.0);
    EmitVertex();
    gl_Position  = VP * tang;
    gs_out.color = vec3(1.0, 0.0, 0.0);
    EmitVertex();
    EndPrimitive();
}

void main()
{
    VP  = projectionMatrix * viewMatrix;
    MVP = VP * modelMatrix;

    for (int i = 0; i < gl_in.length(); ++i) {
        generateNormal(i);
        generateTangent(i);
    }
}
