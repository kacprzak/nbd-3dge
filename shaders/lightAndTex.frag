#version 330

//uniform sampler2D colorTexture;

in vec3 color;
in vec2 texCoord;
out vec4 fragColor;

void main()
{
    //outputColor = texture2D(colorTexture, texCoord);
    fragColor = vec4(color, 1);
}
