// we use the same vertex shader for all the shaders in this chapter
#version 330 core
// these are set in the array buffer
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aVertexCol;

// the transformation matrices
uniform mat4 camera;
uniform mat4 model;
uniform int use_vertex_color; // object color
uniform vec3 objectColor; // object color


// data we pass to the fragment shader
out vec3 vertexCol;
out vec3 Normal;
out vec3 pos;

void main() {
    vec4 p = model * vec4(aPos, 1);
    gl_Position = camera * p;
	if (use_vertex_color == 1)
		vertexCol = aVertexCol; // per vertex coloring
	else 
		vertexCol = objectColor;
    pos = vec3(p);
    Normal = mat3(transpose(inverse(model))) * aNormal;
}