#version 330

uniform sampler2D colorTexture;

in vec2 texCoord;
out vec4 outputColor;

void main()
{
    outputColor = texture2D(colorTexture, texCoord);
    //outputColor = vec4(1,0,0,1);
}
