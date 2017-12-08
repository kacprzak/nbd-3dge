#version 330 core

layout(points) in;
layout(line_strip, max_vertices = 36) out;

out GS_OUT { vec3 color; }
gs_out;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

uniform vec3 minimum;
uniform vec3 maximum;

mat4 VP;

void generateLine(vec4 from, vec4 to, vec3 color)
{
    gs_out.color = color;
    gl_Position  = VP * from;
    EmitVertex();
    gs_out.color = color;
    gl_Position  = VP * to;
    EmitVertex();
    EndPrimitive();
}

void generateRect(vec4 p[4], vec3 color)
{
    for (int i = 0; i < 4; ++i) {
        gs_out.color = color;
        gl_Position  = VP * p[i];
        EmitVertex();
    }
    gs_out.color = color;
    gl_Position  = VP * p[0];
    EmitVertex();
    EndPrimitive();
}

void drawCube(vec4 p[8], vec3 color)
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

void aabb2cube(vec4 lbn, vec4 rtf, out vec4 p[8])
{
    p[0] = lbn; // lbn
    p[4] = rtf; // rtf

    p[1] = vec4(rtf.x, lbn.y, lbn.z, 1.0); // rbn
    p[2] = vec4(rtf.x, rtf.y, lbn.z, 1.0); // rtn
    p[3] = vec4(lbn.x, rtf.y, lbn.z, 1.0); // ltn

    p[5] = vec4(rtf.x, lbn.y, rtf.z, 1.0); // rbf
    p[6] = vec4(lbn.x, lbn.y, rtf.z, 1.0); // lbf
    p[7] = vec4(lbn.x, rtf.y, rtf.z, 1.0); // ltf
}

void main()
{
    VP = projectionMatrix * viewMatrix;

    vec3 color_m = vec3(1.0, 0.0, 1.0);
    vec3 color_w = vec3(1.0, 1.0, 0.0);

    vec4 corners[8];

    aabb2cube(vec4(minimum, 1.0), vec4(maximum, 1.0), corners);

    for (int i = 0; i < 8; ++i) {
        corners[i] = modelMatrix * corners[i];
    }

    drawCube(corners, color_m);

    vec4 min_world = corners[0];
    vec4 max_world = corners[0];

    for (int i = 1; i < 8; ++i) {
        for (int j = 0; j < 4; ++j) {
            min_world[j] = min(min_world[j], corners[i][j]);
            max_world[j] = max(max_world[j], corners[i][j]);
        }
    }

    aabb2cube(min_world, max_world, corners);
    drawCube(corners, color_w);
}
