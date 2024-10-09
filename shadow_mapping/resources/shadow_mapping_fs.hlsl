#version 330 core

in vec3 pos;
in vec3 Normal;
in vec2 TexCoords;
in vec4 posLightSpace;

struct lightsource {
    int type;          // SPOTLIGHT=1, DIRLIGHT=2
    vec3 pos_or_dir;   // light location
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform lightsource light;
uniform sampler2D shadowMap;
uniform vec3 cameraPos;
uniform vec3 objectColor; // object color

out vec4 color;

float shadow_calculation()
{
    // perform perspective divide
    vec3 projCoords = posLightSpace.xyz / posLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.499;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float shadow = currentDepth > closestDepth ? 1.0 : 0.0;

    return shadow;
}

void main()
{
    vec3 normal = normalize(Normal);

    vec3 ambient = light.ambient;

    vec3 lightDir = normalize(light.pos_or_dir);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * light.diffuse;

    vec3 viewDir = normalize(cameraPos - pos);
    vec3 reflectDir = reflect(-light.pos_or_dir, normal);
    vec3 halfwayDir = normalize(light.pos_or_dir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 128.0);
    vec3 specular = spec * light.specular;

    float shadow = shadow_calculation();

    vec3 result = (ambient + (1.0 - shadow) * (diffuse + specular)) * objectColor;

    color = vec4(result, 1);
}