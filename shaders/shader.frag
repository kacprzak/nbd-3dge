#version 120

uniform sampler2D colorTexture;

varying vec2 texCoord;

void main()
{
    gl_FragColor = texture2D(colorTexture, texCoord);
}
