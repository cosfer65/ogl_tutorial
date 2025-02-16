#version 330 core
layout(location = 0) in vec3 aPos;

// the transformation matrices
uniform mat4 camera;
uniform mat4 model;

void main() {
    gl_Position = camera * model * vec4(aPos, 1);
}