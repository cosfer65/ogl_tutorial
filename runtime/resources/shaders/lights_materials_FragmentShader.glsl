#version 330 core

struct lightsource {
    int type;             // SPOTLIGHT=1, DIRLIGHT=2
    vec3 pos_or_dir;      // light location
    vec3 ambient;         // the ambience property of the light
    vec3 diffuse;         // the diffuse property of the light
    vec3 specular;        // the specular property of the light
};
uniform lightsource light;

struct material {
    // how the material reacts to light
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shine;  // surface shine
    sampler2D diffuse_map;   // the surface texture
    sampler2D specular_map;  // the specular reflection map
};
uniform material mat;

uniform vec3 cameraPos;   // viewer location
uniform vec4 object_color; // object color
uniform int coloring_type; // color based on: 0=object color, 1=material, 2=material map& light

in vec2 texCoord;
in vec3 Normal;           // surface normal
in vec3 pos;              // drawing position

out vec4 color;           // resulting drawing color

void light_main() {
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
    vec3 result = (ambient + diffuse + specular);
	color = vec4(result, 1) * object_color;
}

void material_main() {
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

void material_map_main() {
    // ambient color 
    vec3 ambient = light.ambient * texture(mat.diffuse_map, texCoord).rgb;

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
    vec3 diffuse = light.diffuse * diff * texture(mat.diffuse_map, texCoord).rgb;

    // specular
    vec3 viewDir = normalize(cameraPos - pos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.shine);
    vec3 specular = light.specular * spec * texture(mat.specular_map, texCoord).rgb;

    // light emited from object is (ambient + diffuse + specular)
    vec3 result = ambient + diffuse + specular;

    color = vec4(result, 1);
}

void main(){
	if (coloring_type == 0)
		light_main();
	else if (coloring_type == 1)
		material_main();
	else
		material_map_main();
    
}
