#version 330

uniform sampler2D sampler0;
uniform sampler2D sampler1;
uniform sampler2D sampler2;

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
in vec3 normal;
in vec3 tangent;

out vec4 fragColor;

const material mtl = material(
    vec3(1, 1, 1) * 0.1,
    vec3(1, 1, 1) * 0.9,
    vec3(1, 1, 1) * 0.4,
    32
);

void main()
{
    vec4 texColor = texture2D(sampler0, texCoord);
    vec4 normalColor = texture2D(sampler1, texCoord);
    vec4 specularColor = texture2D(sampler2, texCoord);

    fragColor.rgb = texColor.xyz * mtl.ambient * ambient
        + texColor.xyz * mtl.diffuse * diffuse
        + mtl.specular * specular;

    //fragColor.rgb = texColor.rgb;
    //fragColor.rgb = normalColor.rgb;

    if (texColor.a < 0.6)
        discard;
    else
        fragColor.a = texColor.a;
}
