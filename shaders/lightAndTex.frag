#version 330

uniform sampler2D sampler0;
uniform samplerCube environmentCube;
uniform vec3 cameraPosition;
uniform sampler2DShadow shadowSampler;

in vec4 position_lightSpace;
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

float calcShadow(vec4 position_lightSpace)
{
    // perform perspective divide
    vec3 projCoords = position_lightSpace.xyz / position_lightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float shadow = texture(shadowSampler, projCoords.xyz);

    return 1.0 - shadow;
}

void main()
{
    vec3 fragToCamera    = normalize(position_w - cameraPosition);
    vec3 cubeCoord       = reflect(fragToCamera, normalize(normal_w));
    vec3 reflectionColor = texture(environmentCube, cubeCoord).rgb;

    float shadow = calcShadow(position_lightSpace);

    vec4 texColor = texture2D(sampler0, texCoord);
    fragColor.rgb = texColor.xyz * material.ambient * ambient;
    fragColor.rgb += texColor.xyz * material.diffuse * diffuse * (1.0 - shadow);
    fragColor.rgb += material.specular * specular * (1.0 - shadow);
    fragColor.rgb = mix(fragColor.rgb, reflectionColor, 0.05);
    fragColor.a   = 1.0;
}
