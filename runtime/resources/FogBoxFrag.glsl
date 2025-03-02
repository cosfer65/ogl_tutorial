#version 330 core

out vec4 FragColor;
in vec3 TexCoords;
uniform samplerCube c_skybox;

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
	FragColor = texture(c_skybox, TexCoords);
	// Apply fog calculation only if fog is enabled
	if(fogParams.isEnabled == 1)	{
	 	float fogCoordinate = fogParams.linearEnd*0.75; // not completely hidden
	 	FragColor = mix(FragColor, vec4(fogParams.color, 1.0), getFogFactor(fogParams, fogCoordinate));
	}
}
