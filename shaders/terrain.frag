#version 330

uniform sampler2D sampler0;
uniform sampler2D sampler1;
uniform sampler2D sampler2;

struct material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

in vec3 ambient;
in vec3 diffuse;
in vec3 specular;
in vec2 texCoord;
in vec3 normal;
in float height;

out vec4 fragColor;

const material mtl = material(
    vec3(1, 1, 1) * 0.1,
    vec3(1, 1, 1) * 0.9,
    vec3(1, 1, 1) * 0.02,
    1
);

void main()
{
    const float snowLevel = 80.0f;
    const float snowTransitionHeight = 30.0f;
    
    //float normalImpact = dot(normal, vec3(0,1,0)) * 80;
    //fragColor = vec4(1.0f - dot(normal, vec3(0,1,0)), 0, 0, 1);return;
    
    float snowFactor = height - snowLevel;// + normalImpact;
    snowFactor = clamp(snowFactor / snowTransitionHeight, 0.0f, 1.0f);

    float grassHeight = 0.0f;
    float grassTrasitionHeight = 5.0f;
    float grassFactor = height - grassHeight;
    grassFactor = clamp(grassFactor / grassTrasitionHeight, 0.0f, 1.0f);
    
    vec4 texColor;
    vec4 rocks = texture2D(sampler2, texCoord);
    vec4 grass = texture2D(sampler0, texCoord);
    vec4 snow = texture2D(sampler1, texCoord);
    
    texColor = mix(mix(rocks, grass, grassFactor), snow, snowFactor);
        
    fragColor.rgb = texColor.xyz * mtl.ambient * ambient;
    fragColor.rgb += texColor.xyz * mtl.diffuse * diffuse;
    fragColor.rgb += mtl.specular * specular;
    fragColor.a = 1.0;
}
