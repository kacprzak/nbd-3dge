#version 330

uniform sampler2D sampler0;
uniform sampler2D sampler1;
uniform sampler2D sampler2;
uniform sampler2DShadow shadowSampler;

in vec3 position_shadowMap;
in vec3 ambient;
in vec3 diffuse;
in vec3 specular;
in vec2 texCoord;
in vec3 position_w;
in vec3 normal_w;

out vec4 fragColor;

struct Light
{
    vec4 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light lights[8];

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Material material;

float calcShadowRev(vec3 position_shadowMap, float cosTheta)
{
    float bias = 0.005 * tan(acos(cosTheta)); // cosTheta is dot( n,l ), clamped between 0 and 1
    bias       = clamp(bias, 0, 0.01);

    position_shadowMap.z -= bias;

    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    return texture(shadowSampler, position_shadowMap);
}

void main()
{
    Light sun = lights[0];

    vec3 surfaceToLight;
    if (sun.position.w == 0.0) {
        surfaceToLight = normalize(-sun.position.xyz);
    } else {
        surfaceToLight = normalize(sun.position.xyz - position_w.xyz);
    }

    const float snowLevel            = 30.0;
    const float snowTransitionHeight = 5.0;

    // float normalImpact = dot(normal, vec3(0,1,0)) * 80;
    // fragColor = vec4(1.0 - dot(normal, vec3(0,1,0)), 0, 0, 1);return;

    float snowFactor = position_w.y - snowLevel; // + normalImpact;
    snowFactor       = clamp(snowFactor / snowTransitionHeight, 0.0, 1.0);

    float grassHeight          = 0.0;
    float grassTrasitionHeight = 5.0;
    float grassFactor          = position_w.y - grassHeight;
    grassFactor                = clamp(grassFactor / grassTrasitionHeight, 0.0, 1.0);

    vec4 texColor;
    vec4 rocks = texture2D(sampler2, texCoord);
    vec4 grass = texture2D(sampler0, texCoord);
    vec4 snow  = texture2D(sampler1, texCoord);

    texColor = mix(mix(rocks, grass, grassFactor), snow, snowFactor);

    float cosTheta = clamp(dot(normal_w, surfaceToLight), 0.0, 1.0);
    // fragColor = vec4(cosTheta, 0, 0, 1); return;

    float shadow = calcShadowRev(position_shadowMap, cosTheta);

    fragColor.rgb = texColor.xyz * material.ambient * ambient;
    fragColor.rgb += texColor.xyz * material.diffuse * diffuse * shadow;
    fragColor.rgb += material.specular * specular * shadow;
    fragColor.a = 1.0;
}
