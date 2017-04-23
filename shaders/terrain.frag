#version 330

uniform sampler2D colorTexture;

struct material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

in vec3 ambient;
in vec3 diffuse;
in vec3 specular;
in vec2 texCoord;

out vec4 fragColor;

const material mtl = material(
    vec3(1, 1, 1) * 0.1,
    vec3(1, 1, 1) * 0.9,
    vec3(1, 1, 1) * 0.4,
    32
);

void main()
{
    vec4 texColor = texture2D(colorTexture, texCoord);
    fragColor.rgb = texColor.xyz * mtl.ambient * ambient
        + texColor.xyz * mtl.diffuse * diffuse
        + mtl.specular * specular;
    fragColor.a = 1.0;
}
