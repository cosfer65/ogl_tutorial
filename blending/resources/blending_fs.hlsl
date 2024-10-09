#version 330 core

struct lightsource {
    int type;          // SPOTLIGHT=1, DIRLIGHT=2
    vec3 pos_or_dir;   // light location
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform lightsource light;

uniform vec4 objectColor; // object color
uniform int useColor;      // use color=1 or texture=0

in vec3 Normal;           // surface normal
in vec3 pos;              // drawing position

uniform vec3 cameraPos;   // viewer location

out vec4 color;           // resulting drawing color

uniform sampler2D textureSampler;
in vec2 texCoord;

void main() {
    // ambient color
    vec3 ambient = light.ambient;
    // surface normal
    vec3 norm = normalize(Normal);
    // light direction
    vec3 lightDir = normalize(light.pos_or_dir - pos);
    // view direction
    vec3 viewDir = normalize(cameraPos - pos);
    // reflection vector
    vec3 reflectDir = reflect(-lightDir, norm);
    // specular strength
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
    // at last, specular color
    vec3 specular = spec * light.specular;
    // diffusion factor (calculated by the reflection angle)
    float diff = max(dot(norm, lightDir), 0.0);
    // and diffusion color
    vec3 diffuse = diff * light.specular;

    // light emited from object is (ambient + diffuse + specular)
    vec4 result;
    if (useColor == 1)
        result = vec4(ambient + diffuse + specular, 1) * objectColor;
    else
        result = vec4(ambient + diffuse + specular, 1) * texture(textureSampler, texCoord);
    
    color = result;
}
