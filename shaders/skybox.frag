#version 330

uniform sampler2D colorTexture;

in vec2 texCoord;

out vec4 fragColor;

void main()
{
    fragColor = texture2D(colorTexture, texCoord);
}
