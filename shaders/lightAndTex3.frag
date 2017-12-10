#version 330

const int MAX_CASCADES = 4;

uniform sampler2D sampler0;
uniform sampler2D sampler1;
uniform sampler2D sampler2;
uniform sampler2DArrayShadow shadowSampler;
uniform float cascadeFar[MAX_CASCADES];

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 lightMVP;

in VS_OUT
{
    vec3 position_shadowMap[MAX_CASCADES];
    vec3 position_w;
    vec3 position_v;
    vec2 texCoord;
    mat3 TBN;
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
    bias       = clamp(bias, 0, 0.001);

    coord.w -= bias;

    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    return texture(shadowSampler, coord);
}

void main()
{
    Light sun = lights[0];

    vec4 texColor      = texture2D(sampler0, fs_in.texCoord);
    vec4 normalColor   = normalize(texture2D(sampler1, fs_in.texCoord) * 2 - 0.5);
    vec4 specularColor = texture2D(sampler2, fs_in.texCoord);

    vec3 surfaceToLight;
    if (sun.position.w == 0.0) {
        surfaceToLight = normalize(-sun.position.xyz);
    } else {
        surfaceToLight = normalize(sun.position.xyz - fs_in.position_w);
    }

    vec3 ambient = sun.ambient;

    vec3 normal_world = normalize(fs_in.TBN * normalColor.xyz);
    vec3 diffuse      = sun.diffuse * max(dot(normal_world, surfaceToLight), 0.0);

    vec3 specular;
    if (dot(normal_world, surfaceToLight) < 0.0) {
        specular = vec3(0.0, 0.0, 0.0);
    } else {
        vec3 reflection  = -reflect(surfaceToLight, normal_world);
        vec3 vertexToEye = normalize(-fs_in.position_v);
        specular = sun.specular * pow(max(dot(vertexToEye, reflection), 0.0), material.shininess);
    }

    float cosTheta = clamp(dot(normal_world, surfaceToLight), 0.0, 1.0);
    // fragColor = vec4(cosTheta, 0, 0, 1); return;

    int cascadeIdx = cascadeIndex(-fs_in.position_v.z);
    // fragColor = vec4(0); fragColor[cascadeIdx] = 1.0; return;
    float shadow = calcShadowRev(fs_in.position_shadowMap[cascadeIdx], cascadeIdx, cosTheta);

    fragColor.rgb = texColor.xyz * material.ambient * ambient;
    fragColor.rgb += texColor.xyz * material.diffuse * diffuse * shadow;
    fragColor.rgb += specularColor.xyz * specular * 16 * shadow;

    // fragColor.rgb = texColor.rgb;
    // fragColor.rgb = specularColor.rgb;
    // fragColor.rgb = normal_world.rgb * 0.5 + 0.5;
    // fragColor.rgb = normalColor.rgb * 0.5 + 0.5;
    // fragColor.rgb = diffuse;
    // fragColor.rgb = specular;
    // fragColor.rgb = specularColor.rgb * specular * 32;

    if (texColor.a < 0.6)
        discard;
    else
        fragColor.a = texColor.a;
}
