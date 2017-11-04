#version 330

uniform sampler2D colorTexture;

in vec3 ambient;
in vec3 diffuse;
in vec3 specular;
in vec2 texCoord;

out vec4 fragColor;

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Material material;

void main()
{
    vec4 texColor = texture2D(colorTexture, texCoord);
    fragColor.rgb = texColor.xyz * material.ambient * ambient +
                    texColor.xyz * material.diffuse * diffuse + material.specular * specular;
    fragColor.a = 1.0;
}
