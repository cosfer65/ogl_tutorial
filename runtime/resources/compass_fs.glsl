#version 330 core

uniform sampler2D textureSampler;
uniform int use_texture;
uniform vec3 objectColor; // object color

in vec3 Normal;
in vec3 pos;
in vec2 texCoord;

out vec4 color;             // resulting drawing color

void main() {
	if (use_texture == 1)
		color = vec4(texture(textureSampler, texCoord).rgb, 1);
	else
		color = vec4(objectColor, 1);
}
