#version 330

#define HAS_OCCLUSIONMAP
#define HAS_EMISSIVE
#define USE_IBL
//#define USE_POINT_LIGHTS

out vec4 fragColor;

struct Material
{
    vec4 baseColorFactor;
    float normalScale;
    float metallicFactor;
    float roughnessFactor;
    float occlusionStrength;
    vec3 emissiveFactor;
};

uniform Material material;
uniform sampler2D baseColorSampler;
uniform sampler2D normalSampler;
uniform sampler2D metallicRoughnessSampler;
#ifdef HAS_OCCLUSIONMAP
uniform sampler2D occlusionSampler;
#endif
#ifdef HAS_EMISSIVE
uniform sampler2D emissiveSampler;
#endif
#ifdef USE_IBL
uniform sampler2D brdfLUT;
uniform samplerCube irradianceCube;
uniform samplerCube radianceCube;
#endif

in vec4 position;
in vec2 texCoord_0;
in mat3 TBN;

const float PI    = 3.14159265359;
const float GAMMA = 2.2;

#ifdef USE_POINT_LIGHTS
struct Light
{
    vec4 position;
    vec3 color;
};

#define MAX_LIGHTS 1

uniform Light lights[MAX_LIGHTS];
#endif

float distributionGGX(vec3 N, vec3 H, float roughness);
float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);
vec4 srgb2linear(vec4 srgb);

void main()
{
     // The TBN matrix is linearly interpolated, so we need to re-normalize
    vec3 N = normalize(TBN[2]);

    if (length(TBN[0]) > 0.0) {
        N = texture(normalSampler, texCoord_0).rgb;
        N = (2.0 * N - 1.0) * vec3(material.normalScale, material.normalScale, 1.0);
        N = normalize(TBN * N);
    }
    //fragColor = vec4(N, 1.0); return;

    vec3 V      = normalize(-position.xyz);
    float NdotV = max(dot(N, V), 0.0);
    vec3 R      = reflect(-V, N);

    vec4 baseColor = material.baseColorFactor * srgb2linear(texture(baseColorSampler, texCoord_0));
    // fragColor = baseColor; return;

    vec3 occRghMet; // occlusion, roughness, metallic
    occRghMet.gb = vec2(material.roughnessFactor, material.metallicFactor) *
                   texture(metallicRoughnessSampler, texCoord_0).gb;

    float roughness = occRghMet.g;
    float metallic  = occRghMet.b;

    vec3 F0 = vec3(0.04);
    F0      = mix(F0, baseColor.rgb, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);

#ifdef USE_POINT_LIGHTS
    for (int i = 0; i < MAX_LIGHTS; ++i) {
        vec3 L = normalize(lights[i].position - position).xyz;
        vec3 H = normalize(V + L);

        // Lambertian
        float NdotL = max(dot(N, L), 0.0);

        float distance    = length(lights[i].position - position);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance     = lights[i].color * attenuation;

        // cook-torrance brdf
        float NDF = distributionGGX(N, H, roughness);
        float G   = geometrySmith(N, V, L, roughness);
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * NdotV * NdotL;
        vec3 specular     = numerator / max(denominator, 0.001);

        // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;

        // add to outgoing radiance Lo
        Lo += (kD * baseColor.rgb / PI + specular) * radiance * NdotL;
    }
#endif

#ifdef USE_IBL
    vec3 F = fresnelSchlickRoughness(NdotV, F0, roughness);

    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;
    vec3 irradiance = srgb2linear(texture(irradianceCube, N)).rgb;
    vec3 diffuse    = irradiance * baseColor.rgb;
    // fragColor = vec4(irradiance, 1.0); return;

    // roughness = 0.0;
    const float MAX_REFLECTION_LOD = 9.0;
    vec3 radianceColor =
        srgb2linear(textureLod(radianceCube, R, roughness * MAX_REFLECTION_LOD)).rgb;
    vec2 envBRDF  = texture(brdfLUT, vec2(NdotV, roughness)).rg;
    vec3 specular = radianceColor * (F * envBRDF.x + envBRDF.y);
    // fragColor = vec4(specular, 1.0); return;

    vec3 ambient = kD * diffuse + specular;
#else
    vec3 ambient = vec3(0.003);
#endif
    // gl_FragColor = vec4(ambient, 1.0); return;
    vec3 color = ambient + Lo;

#ifdef HAS_OCCLUSIONMAP
    float ao = texture(occlusionSampler, texCoord_0).r;
    color    = mix(color, color * ao, material.occlusionStrength);
#endif

#ifdef HAS_EMISSIVE
    vec3 emissive = material.emissiveFactor * srgb2linear(texture(emissiveSampler, texCoord_0)).rgb;
    color.rgb += emissive;
#endif

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // Gamma
    color = pow(color, vec3(1.0 / GAMMA));

    fragColor = vec4(color, baseColor.a);
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

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

vec4 srgb2linear(vec4 srgb) { return vec4(pow(srgb.xyz, vec3(GAMMA)), srgb.a); }
