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

float calcShadow(vec4 position_lightSpace, float cosTheta)
{
    const mat4 biasMatrix =
        mat4(0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.5, 0.5, 0.5, 1.0);

    position_lightSpace = biasMatrix * position_lightSpace;

    float bias = 0.005 * tan(acos(cosTheta)); // cosTheta is dot( n,l ), clamped between 0 and 1
    bias       = clamp(bias, 0, 0.01);

    // perform perspective divide
    vec3 projCoords = position_lightSpace.xyz; // / position_lightSpace.w;
    projCoords.z -= bias;
    // transform to [0,1] range
    // projCoords.xy = projCoords.xy * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float shadow = texture(shadowSampler, projCoords.xyz);

    return 1.0 - shadow;
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

    vec3 fragToCamera    = normalize(position_w - cameraPosition);
    vec3 cubeCoord       = reflect(fragToCamera, normalize(normal_w));
    vec3 reflectionColor = texture(environmentCube, cubeCoord).rgb;

    float cosTheta = clamp(dot(normal_w, surfaceToLight), 0.0, 1.0);

    // fragColor = vec4(cosTheta, 0, 0, 1); return;

    float shadow = calcShadow(position_lightSpace, cosTheta);

    vec4 texColor = texture2D(sampler0, texCoord);
    fragColor.rgb = mix(texColor.xyz, reflectionColor, 0.5) * material.ambient * ambient;
    fragColor.rgb += texColor.xyz * material.diffuse * diffuse * (1.0 - shadow);
    fragColor.rgb += material.specular * specular * (1.0 - shadow);
    fragColor.a = 1.0;
}
