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

uniform float magnitude = 1.0;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

mat4 VP;
mat4 MVP;

void GenerateNormal(int index)
{
    vec4 pos = gl_in[index].gl_Position;
    vec4 norm = (modelMatrix * pos) + normalize(modelMatrix * vec4(gs_in[index].normal, 0.0)) * magnitude * 0.25;

    gl_Position = MVP * pos;
    gs_out.color = vec3(0.0, 0.0, 1.0);
    EmitVertex();
    gl_Position = VP * norm;
    gs_out.color = vec3(0.0, 0.0, 1.0);
    EmitVertex();
    EndPrimitive();
}

void GenerateTangent(int index)
{
    vec4 pos = gl_in[index].gl_Position;
    vec4 tang = (modelMatrix * pos) + normalize(modelMatrix * vec4(gs_in[index].tangent, 0.0)) * magnitude * 0.25;
    
    gl_Position = MVP * pos;
    gs_out.color = vec3(1.0, 0.0, 0.0);
    EmitVertex();
    gl_Position = VP * tang;
    gs_out.color = vec3(1.0, 0.0, 0.0);
    EmitVertex();
    EndPrimitive();
}

void main()
{
    VP = projectionMatrix * viewMatrix;
    MVP = VP * modelMatrix;

    int i;
    for(i = 0; i < gl_in.length(); ++i) {
        GenerateNormal(i);
        GenerateTangent(i);
    }
}  
