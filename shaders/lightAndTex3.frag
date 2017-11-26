#version 330

uniform sampler2D sampler0;
uniform sampler2D sampler1;
uniform sampler2D sampler2;
uniform sampler2DShadow shadowSampler;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 lightMVP;

in VS_OUT
{
    vec4 pos_lightSpace;
    vec3 pos_world;
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
    Light sun = lights[0];

    vec4 texColor      = texture2D(sampler0, fs_in.texCoord);
    vec4 normalColor   = normalize(texture2D(sampler1, fs_in.texCoord) * 2 - 0.5);
    vec4 specularColor = texture2D(sampler2, fs_in.texCoord);

    // vec3 normal_world = normalize(modelMatrix * vec4(in_normal, 0)).xyz;

    vec4 position_world = vec4(fs_in.pos_world, 1.0);
    vec4 position_eye   = viewMatrix * position_world;

    vec3 surfaceToLight;
    if (sun.position.w == 0.0) {
        surfaceToLight = normalize(-sun.position.xyz);
    } else {
        surfaceToLight = normalize(sun.position.xyz - position_world.xyz);
    }

    vec3 ambient = sun.ambient;

    vec3 normal_world = normalize(fs_in.TBN * normalColor.xyz);
    vec3 diffuse      = sun.diffuse * max(dot(normal_world, surfaceToLight), 0.0);

    vec3 specular;
    if (dot(normal_world, surfaceToLight) < 0.0) {
        specular = vec3(0.0, 0.0, 0.0);
    } else {
        vec3 reflection  = -reflect(surfaceToLight, normal_world);
        vec3 vertexToEye = normalize(-position_eye.xyz);
        specular = sun.specular * pow(max(dot(vertexToEye, reflection), 0.0), material.shininess);
    }

    float shadow = calcShadow(fs_in.pos_lightSpace);

    fragColor.rgb = texColor.xyz * material.ambient * ambient;
    fragColor.rgb += texColor.xyz * material.diffuse * diffuse * (1.0 - shadow);
    fragColor.rgb += specularColor.xyz * specular * 16 * (1.0 - shadow);

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
