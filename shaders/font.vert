#version 330

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

out vec2 texCoord;

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 in_texCoord;
void main()
{
    gl_Position = modelMatrix * vec4(position, 0.0, 1.0);
    texCoord = in_texCoord;
    texCoord.t = 1.0 - texCoord.t;
}
