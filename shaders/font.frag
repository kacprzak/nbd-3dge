#version 330

uniform sampler2D colorTexture;

in vec2 texCoord;
out vec4 fragColor;

void main()
{
    vec4 outputColor = texture2D(colorTexture, texCoord);
    //    if (outputColor.a < 0.5)
    //        discard;
    fragColor = outputColor;
}
