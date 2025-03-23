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
uniform vec4 object_color;    // object color

in vec4 vertex_color;        // vertex/object coloring
in vec3 Normal;           // surface normal
in vec3 pos;              // drawing position

out vec4 color;           // resulting drawing color

void n_main() {
color = object_color;
}

void main() {
    vec3 ambient = light.ambient;
    vec3 norm = normalize(Normal);

    vec3 lightDir;
    if (light.type == 1) // SPOTLIGHT
        lightDir = normalize(light.pos_or_dir - pos);
    else                 // DIRLIGHT
        lightDir = normalize(-light.pos_or_dir);

    vec3 viewDir = normalize(cameraPos - pos);

    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128);
    vec3 specular = spec * light.specular;

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.diffuse;

	color = vec4((ambient + diffuse + specular),1) * vertex_color;
}
