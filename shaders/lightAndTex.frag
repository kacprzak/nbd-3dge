#version 330

uniform sampler2D sampler0;
uniform samplerCube environmentCube;
uniform vec3 cameraPosition;

in vec3 ambient;
in vec3 diffuse;
in vec3 specular;
in vec2 texCoord;
in vec3 position_w;
in vec3 normal_w;

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
    vec3 fragToCamera    = normalize(position_w - cameraPosition);
    vec3 cubeCoord       = reflect(fragToCamera, normalize(normal_w));
    vec3 reflectionColor = texture(environmentCube, cubeCoord).rgb;

    vec4 texColor = texture2D(sampler0, texCoord);
    fragColor.rgb = texColor.xyz * material.ambient * ambient +
                    texColor.xyz * material.diffuse * diffuse + material.specular * specular;
    fragColor.rgb = mix(fragColor.rgb, reflectionColor, 0.05);
    fragColor.a   = 1.0;
}
