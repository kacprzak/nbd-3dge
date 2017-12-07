#version 330

const int MAX_CASCADES = 4;

uniform sampler2D sampler0;
uniform sampler2D sampler1;
uniform sampler2D sampler2;
uniform sampler2DArrayShadow shadowSampler;
uniform float cascadeFar[MAX_CASCADES];

in VS_OUT
{
    float clipZ;
    vec3 position_shadowMap[MAX_CASCADES];
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec2 texCoord;
    vec3 position_w;
    vec3 normal_w;
}
fs_in;

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

int cascadeIndex(float z)
{
    for (int i = 0; i < MAX_CASCADES; ++i) {
        if (z < cascadeFar[i]) {
            return i;
        }
    }
    return MAX_CASCADES - 1;
}

float calcShadowRev(vec3 position_shadowMap, int cascadeIdx, float cosTheta)
{
    vec4 coord = vec4(position_shadowMap.x, position_shadowMap.y, cascadeIdx, position_shadowMap.z);

    float bias = 0.005 * tan(acos(cosTheta)); // cosTheta is dot( n,l ), clamped between 0 and 1
    bias       = clamp(bias, 0, 0.01);

    coord.w -= bias;

    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    return texture(shadowSampler, coord);
}

void main()
{
    Light sun = lights[0];

    vec3 surfaceToLight;
    if (sun.position.w == 0.0) {
        surfaceToLight = normalize(-sun.position.xyz);
    } else {
        surfaceToLight = normalize(sun.position.xyz - fs_in.position_w.xyz);
    }

    const float snowLevel            = 30.0;
    const float snowTransitionHeight = 5.0;

    // float normalImpact = dot(normal, vec3(0,1,0)) * 80;
    // fragColor = vec4(1.0 - dot(normal, vec3(0,1,0)), 0, 0, 1);return;

    float snowFactor = fs_in.position_w.y - snowLevel; // + normalImpact;
    snowFactor       = clamp(snowFactor / snowTransitionHeight, 0.0, 1.0);

    float grassHeight          = 0.0;
    float grassTrasitionHeight = 5.0;
    float grassFactor          = fs_in.position_w.y - grassHeight;
    grassFactor                = clamp(grassFactor / grassTrasitionHeight, 0.0, 1.0);

    vec4 texColor;
    vec4 rocks = texture2D(sampler2, fs_in.texCoord);
    vec4 grass = texture2D(sampler0, fs_in.texCoord);
    vec4 snow  = texture2D(sampler1, fs_in.texCoord);

    texColor = mix(mix(rocks, grass, grassFactor), snow, snowFactor);

    float cosTheta = clamp(dot(fs_in.normal_w, surfaceToLight), 0.0, 1.0);
    // fragColor = vec4(cosTheta, 0, 0, 1); return;

    int cascadeIdx = cascadeIndex(fs_in.clipZ);
    // fragColor = vec4(0); fragColor[cascadeIdx] = 1.0; return;
    float shadow = calcShadowRev(fs_in.position_shadowMap[cascadeIdx], cascadeIdx, cosTheta);

    fragColor.rgb = texColor.xyz * material.ambient * fs_in.ambient;
    fragColor.rgb += texColor.xyz * material.diffuse * fs_in.diffuse * shadow;
    fragColor.rgb += material.specular * fs_in.specular * shadow;
    fragColor.a = 1.0;
}
