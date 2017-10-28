#version 330

uniform sampler2D sampler0;
uniform sampler2D sampler1;
uniform sampler2D sampler2;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;


//in vec3 ambient;
//in vec3 diffuse;
//in vec3 specular;
//in vec2 texCoord;
//in vec3 normal;
//in vec3 tangent;

in VS_OUT
{
    vec3 pos_world;
    vec2 texCoord;
    mat3 TBN;
} fs_in;

out vec4 fragColor;

struct light
{
    vec4 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

const light sun = light(
    vec4(1, -1, -1, 0),
    vec3(1.0, 0.8863, 0.8078),
    vec3(1.0, 0.8863, 0.8078),
    vec3(1, 1, 1)
);

struct material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

const material mtl = material(
    vec3(1, 1, 1) * 0.1,
    vec3(1, 1, 1) * 0.9,
    vec3(1, 1, 1) * 0.4,
    32
);


void main()
{
    vec4 texColor = texture2D(sampler0, fs_in.texCoord);
    vec4 normalColor = normalize(texture2D(sampler1, fs_in.texCoord) * 2 - 0.5);
    vec4 specularColor = texture2D(sampler2, fs_in.texCoord);

    //vec3 normal_world = normalize(modelMatrix * vec4(in_normal, 0)).xyz;
    
    vec4 position_world = vec4(fs_in.pos_world, 1.0);
    vec4 position_eye = viewMatrix * position_world;

    vec3 surfaceToLight;
    if (sun.position.w == 0.0) {
        surfaceToLight = normalize(-sun.position.xyz);
    }
    else {
        surfaceToLight = normalize(sun.position.xyz - position_world.xyz);
    }

    vec3 ambient = sun.ambient;

    vec3 normal_world = normalize(fs_in.TBN * normalColor.xyz);
    vec3 diffuse = sun.diffuse * max(dot(normal_world, surfaceToLight), 0.0);

    
    vec3 specular;
    if (dot(normal_world, surfaceToLight) < 0.0) {
        specular = vec3(0.0, 0.0, 0.0);
    }
    else { 
        vec3 reflection = -reflect(surfaceToLight, normal_world);
        vec3 vertexToEye = normalize(-position_eye.xyz);
        specular = sun.specular * pow(max(dot(vertexToEye, reflection), 0.0), mtl.shininess);
    }
       

    fragColor.rgb = texColor.xyz * mtl.ambient * ambient
        + texColor.xyz * mtl.diffuse * diffuse
        + specularColor.xyz * specular;

    //fragColor.rgb = texColor.rgb;
    //fragColor.rgb = specularColor.rgb * specular;
    //fragColor.rgb = normal_world.rgb * 0.5 + 0.5;
    //fragColor.rgb = normalColor.rgb * 0.5 + 0.5;
    //fragColor.rgb = diffuse;
    //fragColor.rgb = specular;
    

    if (texColor.a < 0.6)
        discard;
    else
        fragColor.a = texColor.a;
}
