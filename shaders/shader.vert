#version 120

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

varying vec2 texCoord;

//layout(location = 0)
attribute vec3 position;
//layout(location = 1)
attribute vec2 in_texCoord;

void main()
{
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);
    texCoord = in_texCoord;
}
