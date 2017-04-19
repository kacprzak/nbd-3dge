#version 330

uniform samplerCube colorCube;

in vec3 texCoord;

out vec4 fragColor;

void main()
{
    fragColor = texture(colorCube, texCoord);
}
