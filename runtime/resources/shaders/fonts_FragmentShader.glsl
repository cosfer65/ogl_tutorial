#version 330 core

struct lightsource {
    int type;          // SPOTLIGHT=1, DIRLIGHT=2
    vec3 pos_or_dir;   // light location
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform lightsource light;

in vec3 Normal;
in vec3 pos;

uniform vec3 cameraPos;
uniform vec4 object_color;

out vec4 color;

void main()
{
    vec3 normal = -normalize(Normal);
    vec3 ambient = light.ambient;
    vec3 lightDir = normalize(light.pos_or_dir - pos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * light.diffuse;
    vec3 viewDir = -normalize(cameraPos - pos);
    vec3 reflectDir = reflect(-light.pos_or_dir, normal);
    vec3 halfwayDir = normalize(light.pos_or_dir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 128.0);
    vec3 specular = spec * light.specular;

    color = vec4((ambient + diffuse + specular), 1) * object_color;
}
