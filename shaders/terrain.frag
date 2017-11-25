#version 330

uniform sampler2D sampler0;
uniform sampler2D sampler1;
uniform sampler2D sampler2;
uniform sampler2DShadow shadowSampler;

in vec4 position_lightSpace;
in vec3 ambient;
in vec3 diffuse;
in vec3 specular;
in vec2 texCoord;
in vec3 normal;
in float height;

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

    return 1.0f - shadow;
}

void main()
{
    const float snowLevel            = 30.0f;
    const float snowTransitionHeight = 5.0f;

    // float normalImpact = dot(normal, vec3(0,1,0)) * 80;
    // fragColor = vec4(1.0f - dot(normal, vec3(0,1,0)), 0, 0, 1);return;

    float snowFactor = height - snowLevel; // + normalImpact;
    snowFactor       = clamp(snowFactor / snowTransitionHeight, 0.0f, 1.0f);

    float grassHeight          = 0.0f;
    float grassTrasitionHeight = 5.0f;
    float grassFactor          = height - grassHeight;
    grassFactor                = clamp(grassFactor / grassTrasitionHeight, 0.0f, 1.0f);

    vec4 texColor;
    vec4 rocks = texture2D(sampler2, texCoord);
    vec4 grass = texture2D(sampler0, texCoord);
    vec4 snow  = texture2D(sampler1, texCoord);

    texColor = mix(mix(rocks, grass, grassFactor), snow, snowFactor);

    float shadow = calcShadow(position_lightSpace);

    fragColor.rgb = texColor.xyz * material.ambient * ambient;
    fragColor.rgb += texColor.xyz * material.diffuse * diffuse * (1.0f - shadow);
    fragColor.rgb += material.specular * specular * (1.0f - shadow);
    fragColor.a = 1.0;
}
