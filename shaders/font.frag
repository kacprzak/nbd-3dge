#version 330

uniform sampler2D colorTexture;

in vec2 texCoord;
out vec4 fragColor;

void main()
{
    vec4 outputColor = texture2D(colorTexture, texCoord);
    fragColor = outputColor;
    //fragColor.ra = vec2(1,1);
    //fragColor = vec4(1,1,1,1);
}
