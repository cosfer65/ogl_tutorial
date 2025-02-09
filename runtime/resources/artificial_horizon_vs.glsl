#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

// the following are mutualy exclusive
layout(location = 2) in vec2 aTexCoord;
//layout(location = 2) in vec3 aVertexCol;

uniform mat4 camera;
uniform mat4 model;
uniform int use_vertex_color; // use per vertex color
uniform vec3 objectColor; // object color

out vec3 Normal;
out vec3 pos;
out vec2 texCoord;

void main() {
    gl_Position = camera * model * vec4(aPos, 1);
    pos = vec3(model * vec4(aPos, 1));
    Normal = mat3(transpose(inverse(model))) * aNormal;
	texCoord = aTexCoord;
}
