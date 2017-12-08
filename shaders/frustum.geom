#version 330 core

layout(points) in;
layout(line_strip, max_vertices = 18) out;

out GS_OUT { vec3 color; }
gs_out;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

uniform vec4 corners[8];

void generateLine(const vec4 from, const vec4 to, const vec3 color)
{
    gs_out.color = color;
    gl_Position  = from;
    EmitVertex();
    gs_out.color = color;
    gl_Position  = to;
    EmitVertex();
    EndPrimitive();
}

void generateRect(const vec4 p[4], const vec3 color)
{
    for (int i = 0; i < 4; ++i) {
        gs_out.color = color;
        gl_Position  = p[i];
        EmitVertex();
    }
    gs_out.color = color;
    gl_Position  = p[0];
    EmitVertex();
    EndPrimitive();
}

void drawCube(const vec4 p[8], const vec3 color)
{
    // Near plane
    vec4 tmp[4];
    for (int i = 0; i < 4; ++i) {
        tmp[i] = p[i];
    }
    generateRect(tmp, color);

    // Far plane
    for (int i = 0; i < 4; ++i) {
        tmp[i] = p[i + 4];
    }
    generateRect(tmp, color);

    // Connections
    generateLine(p[0], p[6], color);
    generateLine(p[1], p[5], color);
    generateLine(p[2], p[4], color);
    generateLine(p[3], p[7], color);
}

void main()
{
    mat4 mvp = projectionMatrix * viewMatrix * modelMatrix;

    const vec3 color = vec3(0.0, 0.0, 1.0);

    vec4 corners_tmp[8];

    for (int i = 0; i < 8; ++i) {
        corners_tmp[i] = mvp * corners[i];
    }

    drawCube(corners_tmp, color);
}
