#version 330 core

struct lightsource {
    int type;             // SPOTLIGHT=1, DIRLIGHT=2
    vec3 pos_or_dir;      // light location
    vec3 ambient;         // the ambience property of the light
    vec3 diffuse;         // the diffuse property of the light
    vec3 specular;        // the specular property of the light
};
uniform lightsource light;
uniform vec3 cameraPos;   // viewer location
uniform vec3 objectColor; // object color

in vec3 Normal;           // surface normal
in vec3 pos;              // drawing position

out vec3 color;           // resulting drawing color

void main() {
    // ambient color
    vec3 ambient = light.ambient;
    // normalize is a built in function in GLSL
    // surface normal
    vec3 norm = normalize(Normal);

    // light direction
    vec3 lightDir;
    if (light.type == 1) // SPOTLIGHT
        lightDir = normalize(light.pos_or_dir - pos);
    else                 // DIRLIGHT
        lightDir = normalize(-light.pos_or_dir);

    // view direction
    vec3 viewDir = normalize(cameraPos - pos);

    // reflection vector, reflect is a built in function in GLSL
    vec3 reflectDir = reflect(-lightDir, norm);

    // pow, max, dot are built in functions in GLSL
    // specular strength
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128);
    // specular color
    vec3 specular = spec * light.specular;

    // diffusion factor (calculated by the reflection angle)
    float diff = max(dot(norm, lightDir), 0.0);

    // and diffusion color
    vec3 diffuse = diff * light.diffuse;

    // light emitted from object is (ambient + diffuse + specular)
    color = (ambient + diffuse + specular) * objectColor;
}
