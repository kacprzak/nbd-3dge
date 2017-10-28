#version 330 core

layout (triangles) in;
layout (line_strip, max_vertices = 12) out;

in VS_OUT {
    vec3 normal;
    vec3 tangent;
} gs_in[];

out GS_OUT {
    vec3 color;
} gs_out;

uniform float magnitude = 1.0f;

void GenerateNormal(int index)
{
    gl_Position = gl_in[index].gl_Position;
    gs_out.color = vec3(0.0, 0.0, 1.0);
    EmitVertex();
    gl_Position = gl_in[index].gl_Position + vec4(gs_in[index].normal, 0.0f) * magnitude;
    gs_out.color = vec3(0.0, 0.0, 1.0);
    EmitVertex();
    EndPrimitive();
}

void GenerateTangent(int index)
{
    gl_Position = gl_in[index].gl_Position;
    gs_out.color = vec3(1.0, 0.0, 0.0);
    EmitVertex();
    gl_Position = gl_in[index].gl_Position + vec4(gs_in[index].tangent, 0.0f) * magnitude;
    gs_out.color = vec3(1.0, 0.0, 0.0);
    EmitVertex();
    EndPrimitive();
}

void main()
{
    GenerateNormal(0); // First vertex normal
    GenerateTangent(0);
    GenerateNormal(1); // Second vertex normal
    GenerateTangent(1);
    GenerateNormal(2); // Third vertex normal
    GenerateTangent(2);
}  
