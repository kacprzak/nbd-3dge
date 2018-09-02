#version 330

out vec4 fragColor;

uniform vec3 cameraPosition;

struct Material
{
    vec4 baseColorFactor;
    float normalScale;
    float metallicFactor;
    float roughnessFactor;
    vec3 emissiveFactor;
};

uniform Material material;
uniform sampler2D baseColorSampler;
uniform sampler2D normalSampler;
uniform sampler2D metallicRoughnessSampler;
uniform sampler2D occlusionSampler;
uniform sampler2D emissiveSampler;

in vec4 position;
in vec2 texCoord_0;
in mat3 TBN;

const float PI    = 3.14159265359;
const float GAMMA = 2.2;

const int numLights = 2;
vec3 lightColor[2];
vec3 lightPos[2];

float distributionGGX(vec3 N, vec3 H, float roughness);
float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);
vec4 srgb2linear(vec4 srgb);

void main()
{
    lightColor[0] = vec3(100.0);
    lightColor[1] = vec3(100.0);
    lightPos[0]   = vec3(3.0, 3.0, 3.0);
    lightPos[1]   = vec3(3.0, -3.0, -3.0);

    vec3 N = texture(normalSampler, texCoord_0).rgb;
    N      = (2.0 * N - 1.0) * vec3(material.normalScale, material.normalScale, 1.0);
    N      = normalize(TBN * N);
    //fragColor = vec4(cameraPosition * 150.f, 1.0); return;

    vec3 V = normalize(cameraPosition - position.xyz);

    vec4 albedo     = material.baseColorFactor * srgb2linear(texture(baseColorSampler, texCoord_0));
    float occlusion = texture(occlusionSampler, texCoord_0).r;
    float roughness = material.roughnessFactor * texture(metallicRoughnessSampler, texCoord_0).g;
    float metallic  = material.metallicFactor * texture(metallicRoughnessSampler, texCoord_0).b;
    vec3 emissive   = material.emissiveFactor * texture(emissiveSampler, texCoord_0).rgb;

    vec3 F0 = vec3(0.04);
    F0      = mix(F0, albedo.rgb, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    for (int i = 0; i < numLights; ++i) {
        vec3 L = normalize(lightPos[i] - position.xyz);
        vec3 H = normalize(V + L);

        float distance    = length(lightPos[i] - position.xyz);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance     = lightColor[i] * attenuation;

        // cook-torrance brdf
        float NDF = distributionGGX(N, H, roughness);
        float G   = geometrySmith(N, V, L, roughness);
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;

        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
        vec3 specular     = numerator / max(denominator, 0.001);

        // add to outgoing radiance Lo
        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * albedo.rgb / PI + specular) * radiance * NdotL;
    }

    vec3 ambient = vec3(0.03) * albedo.rgb * occlusion;
    vec3 color   = ambient + Lo;

    // HDR tonemapping
    // color = color / (color + vec3(1.0));
    // Gamma
    color = pow(color, vec3(1.0 / GAMMA));

    fragColor = vec4(color + emissive, albedo.a);
}

float distributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness * roughness;
    float a2     = a * a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom       = PI * denom * denom;

    return num / denom;
}

float geometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = geometrySchlickGGX(NdotV, roughness);
    float ggx1  = geometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) { return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0); }

vec4 srgb2linear(vec4 srgb) { return vec4(pow(srgb.xyz, vec3(GAMMA)), srgb.a); }
