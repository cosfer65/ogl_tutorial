#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 2) in vec3 aVertexCol;

uniform mat4 camera;
uniform mat4 model;
uniform int use_vertex_color; // per vertex color
uniform vec4 object_color;    // object color

out vec4 vertex_color;
out vec2 texCoord;
out vec3 Normal;
out vec3 pos;

void main()
{
    vec4 p = model * vec4(aPos, 1);
    gl_Position = camera * p;
    
	if (use_vertex_color == 1)
		vertex_color = vec4(aVertexCol,1); // per vertex coloring
	else 
	 	vertex_color = object_color;

    texCoord = aTexCoord;
    pos = vec3(p);
    Normal = mat3(transpose(inverse(model))) * aNormal;
}
