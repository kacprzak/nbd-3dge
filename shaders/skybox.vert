#version 330 core

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

out vec3 texCoord;

layout(location = 0) in vec3 position;

void main()
{
    vec4 pos    = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1);
    gl_Position = pos.xyww; // This moves Skybox far
    texCoord    = position;
}
