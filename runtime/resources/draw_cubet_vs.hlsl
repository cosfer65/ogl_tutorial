#version 330 core
// vertex position in local coordinates
layout(location = 0) in vec3 aPos;
layout(location = 2) in vec2 aTexCoord;
out vec2 texCoord;
out vec3 norm;
// camera has the combined viewing matrix
uniform mat4 camera;
// model has the combined matrix of object position and orientation
uniform mat4 model;

void main() {
    // calculate final vertex position in the 3D space
    gl_Position = camera * model * vec4(aPos, 1);
    texCoord = aTexCoord;
}