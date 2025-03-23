#version 330 core

uniform vec4 object_color; // object color
uniform int use_color_or_texture; // 1: color, 0: texture

uniform sampler2D textureSampler;

in vec3 Normal;           // surface normal
in vec3 pos;              // drawing position
in vec2 texCoord;         // texture coordinate

// drawing color for OpenGL to use
out vec4 color;
void main() {
	if (use_color_or_texture == 1)
		color = object_color; //vec4(1, 1, 1, 1);
	else
		color = vec4(texture(textureSampler, texCoord).rgb, 1);
}