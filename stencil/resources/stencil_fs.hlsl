#version 330 core

struct lightsource {
    int type;          // SPOTLIGHT=1, DIRLIGHT=2
    vec3 pos_or_dir;   // light location
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform lightsource light;

uniform vec3 objectColor; // object color

in vec3 Normal;           // surface normal
in vec3 pos;              // drawing position

uniform vec3 cameraPos;   // viewer location
in vec2 texCoord;

out vec4 color;           // resulting drawing color

void main() {
    // ambient color 
    vec3 ambient = light.ambient * objectColor;

    // surface normal
    vec3 norm = normalize(Normal);
    // light direction
    vec3 lightDir;
    if (light.type == 1)
        lightDir = normalize(light.pos_or_dir - pos);
    else
        lightDir = normalize(-light.pos_or_dir);

    // diffusion factor (calculated by the reflection angle)
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * objectColor;

    // specular
    vec3 viewDir = normalize(cameraPos - pos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128);
    vec3 specular = light.specular * spec * objectColor;

    // light emited from object is (ambient + diffuse + specular)
    vec3 result = ambient + diffuse + specular;
   
    color = vec4(result, 1);
}
