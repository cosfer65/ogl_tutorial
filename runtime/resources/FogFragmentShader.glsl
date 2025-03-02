#version 440 core

struct lightsource
{
    int type; // SPOTLIGHT=1, DIRLIGHT=2
    vec3 pos_or_dir; // light location
    vec3 ambient; // the ambience property of the light
    vec3 diffuse; // the ambience property of the light
    vec3 specular; // the ambience property of the light
};
uniform lightsource light;

struct material
{
    // how the material reacts to light
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shine; // surface shine
};
uniform material mat;

uniform int use_texture;

in vec3 Normal; // surface normal
in vec3 pos; // drawing position
uniform vec3 cameraPos; // viewer location
out vec4 color; // resulting drawing color

in vec2 texCoord;
uniform sampler2D textureSampler;

struct FogParameters
{
	vec3 color;
	float linearStart;
	float linearEnd;
	float density;
	
	int equation;
	int isEnabled;
};
uniform FogParameters fogParams;

float getFogFactor(FogParameters params, float fogCoordinate);

float getFogFactor(FogParameters params, float fogCoordinate)
{
	float result = 0.0;
	if(params.equation == 0)
	{
		float fogLength = params.linearEnd - params.linearStart;
		result = (params.linearEnd - fogCoordinate) / fogLength;
	}
	else if(params.equation == 1) {
		result = exp(-params.density * fogCoordinate);
	}
	else if(params.equation == 2) {
		result = exp(-pow(params.density * fogCoordinate, 2.0));
	}
	
	result = 1.0 - clamp(result, 0.0, 1.0);
	return result;
}

void main()
{
	vec4 temp_color = vec4(1, 1, 1, 1);
    if (use_texture == 0)
    {
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

        temp_color = vec4(result, 1);
    }
    else
    {
        temp_color = vec4(texture(textureSampler, texCoord).rgb, 1);
    }

	// Apply fog calculation only if fog is enabled
	if(fogParams.isEnabled == 1)	{
		float fogCoordinate = abs(gl_FragCoord.z / gl_FragCoord.w);
		temp_color = mix(temp_color, vec4(fogParams.color, 1.0), getFogFactor(fogParams, fogCoordinate));
	}

	color = temp_color;
}
