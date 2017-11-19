#version 330

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

layout(location = 0) in vec3 in_position;

void main() { gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(in_position, 1.0); }
