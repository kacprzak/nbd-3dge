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
in vec3 normal;
in vec2 texCoord_0;

const float PI = 3.14159265359;

vec3 lightColor = vec3(1.0);
vec3 lightPos   = vec3(0.5, 0.5, -0.5);
int numLights   = 1;

float distributionGGX(vec3 N, vec3 H, float roughness);
float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);

void main()
{
    vec3 N = normalize(normal);
    vec3 V = normalize(cameraPosition - position.xyz);

    vec3 albedo = material.baseColorFactor.rgb * texture2D(baseColorSampler, texCoord_0).rgb;
    // fragColor = vec4(albedo, 1.0); return;
    float occlusion = texture2D(occlusionSampler, texCoord_0).r;
    float roughness = material.roughnessFactor * texture2D(metallicRoughnessSampler, texCoord_0).g;
    float metallic  = material.metallicFactor * texture2D(metallicRoughnessSampler, texCoord_0).b;
    vec3 emissive   = material.emissiveFactor * texture2D(emissiveSampler, texCoord_0).rgb;

    vec3 F0 = vec3(0.04);
    F0      = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    for (int i = 0; i < numLights; ++i) {
        vec3 L = normalize(lightPos - position.xyz);
        vec3 H = normalize(V + L);

        float distance    = length(lightPos - position.xyz);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance     = lightColor * attenuation;

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
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }

    vec3 ambient = vec3(0.03) * albedo * occlusion;
    vec3 color   = ambient + Lo;

    // HDR tonemapping
    // color = color / (color + vec3(1.0));
    // Gamma
    // color = pow(color, vec3(1.0 / 2.2));

    fragColor = vec4(color + emissive, 1.0f);
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
