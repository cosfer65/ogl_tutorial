#version 330 core

struct lightsource {
    int type;          // SPOTLIGHT=1, DIRLIGHT=2
    vec3 pos_or_dir;   // light location
    vec3 ambient;     // the ambience property of the light
    vec3 diffuse;     // the ambience property of the light
    vec3 specular;     // the ambience property of the light
};
uniform lightsource light;

struct material {
    // how the material reacts to light
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shine;  // surface shine
};
uniform material mat;

in vec3 Normal;           // surface normal
in vec3 pos;              // drawing position

uniform vec3 cameraPos;   // viewer location

out vec4 color;           // resulting drawing color

void main() {
    // ambient color based on metrial properties as well
    vec3 ambient = light.ambient * mat.ambient;

    // surface normal
    vec3 norm = normalize(Normal);

    // light direction
    vec3 lightDir;
    if (light.type == 1)
        lightDir = normalize(light.pos_or_dir - pos);
    else
        lightDir = normalize(-light.pos_or_dir);

    // view direction
    vec3 viewDir = normalize(cameraPos - pos);
    // reflection vector
    vec3 reflectDir = reflect(-lightDir, norm);
    // specular strength
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.shine);
    // at last, specular color
    vec3 specular = (spec * mat.specular) * light.specular;

    // diffusion factor (calculated by the reflection angle)
    float diff = max(dot(norm, lightDir), 0.0);
    // and diffusion color
    vec3 diffuse = light.diffuse * (diff * mat.diffuse);

    // light emitted from object is (ambient + diffuse + specular)
    vec3 result = (ambient + diffuse + specular);

    color = vec4(result, 1);
}
