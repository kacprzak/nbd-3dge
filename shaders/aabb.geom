#version 330 core

layout(points) in;
layout(line_strip, max_vertices = 24) out;

out GS_OUT { vec3 color; }
gs_out;

uniform float magnitude = 1.0;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

uniform vec3 minimum;
uniform vec3 maximum;

mat4 MVP;

void generateLine(vec4 from, vec4 to, vec3 color)
{
    gs_out.color = color;
    gl_Position  = MVP * from;
    EmitVertex();
    gs_out.color = color;
    gl_Position  = MVP * to;
    EmitVertex();
    EndPrimitive();
}

void main()
{
    MVP = projectionMatrix * viewMatrix * modelMatrix;

    vec3 color = vec3(1.0, 0.0, 1.0);

    vec4 lbn = vec4(minimum, 1.0);
    vec4 rtf = vec4(maximum, 1.0);

    vec4 rbn = vec4(rtf.x, lbn.y, lbn.z, 1.0);
    vec4 rtn = vec4(rtf.x, rtf.y, lbn.z, 1.0);
    vec4 ltn = vec4(lbn.x, rtf.y, lbn.z, 1.0);

    // Near plane
    generateLine(lbn, rbn, color);
    generateLine(rbn, rtn, color);
    generateLine(rtn, ltn, color);
    generateLine(ltn, lbn, color);

    vec4 rbf = vec4(rtf.x, lbn.y, rtf.z, 1.0);
    vec4 lbf = vec4(lbn.x, lbn.y, rtf.z, 1.0);
    vec4 ltf = vec4(lbn.x, rtf.y, rtf.z, 1.0);

    // Far plane
    generateLine(rtf, rbf, color);
    generateLine(rbf, lbf, color);
    generateLine(lbf, ltf, color);
    generateLine(ltf, rtf, color);

    // Connections
    generateLine(lbn, lbf, color);
    generateLine(rbn, rbf, color);
    generateLine(rtn, rtf, color);
    generateLine(ltn, ltf, color);
}
