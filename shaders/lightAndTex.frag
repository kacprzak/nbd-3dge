#version 330

uniform sampler2D sampler0;
uniform samplerCube environmentCube;
uniform vec3 cameraPosition;
uniform sampler2DShadow shadowSampler;

in VS_OUT
{
    vec3 position_shadowMap;
    vec2 texCoord;
    vec3 position_v;
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
        surfaceToLight = normalize(sun.position.xyz - fs_in.position_w);
    }

    vec3 ambient = sun.ambient;

    vec3 diffuse = sun.diffuse * max(dot(fs_in.normal_w, surfaceToLight), 0.0);

    vec3 specular;
    if (dot(fs_in.normal_w, surfaceToLight) < 0.0) {
        specular = vec3(0.0, 0.0, 0.0);
    } else {
        vec3 reflection  = -reflect(surfaceToLight, fs_in.normal_w);
        vec3 vertexToEye = normalize(-fs_in.position_v);
        specular = sun.specular * pow(max(dot(vertexToEye, reflection), 0.0), material.shininess);
    }

    // Reflections

    vec3 fragToCamera    = normalize(fs_in.position_w - cameraPosition);
    vec3 cubeCoord       = reflect(fragToCamera, normalize(fs_in.normal_w));
    vec3 reflectionColor = texture(environmentCube, cubeCoord).rgb;

    // Shadows

    float cosTheta = clamp(dot(fs_in.normal_w, surfaceToLight), 0.0, 1.0);
    // fragColor = vec4(cosTheta, 0, 0, 1); return;

    float shadow = calcShadowRev(fs_in.position_shadowMap, cosTheta);

    vec4 texColor = texture2D(sampler0, fs_in.texCoord);
    fragColor.rgb = mix(texColor.xyz, reflectionColor, 0.5) * material.ambient * ambient;
    fragColor.rgb += texColor.xyz * material.diffuse * diffuse * shadow;
    fragColor.rgb += material.specular * specular * shadow;
    fragColor.a = 1.0;
}
