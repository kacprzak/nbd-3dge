#version 330

out vec4 fragColor;

struct Material
{
    vec4 baseColorFactor;
    float normalScale;
};

uniform Material material;
uniform sampler2D baseColorSampler;

in vec2 texCoord_0;

void main() { fragColor = material.baseColorFactor * texture2D(baseColorSampler, texCoord_0); }
