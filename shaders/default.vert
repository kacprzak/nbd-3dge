#version 430

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

out vec4 color;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_color;

void main()
{
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(in_position, 1.0);
    color = vec4(in_color, 1.0);
}
