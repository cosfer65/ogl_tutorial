// we use the same vertex shader for all the shaders in this chapter
#version 330 core
// these are set in the array buffer
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

// the transformation matrices
uniform mat4 camera;
uniform mat4 model;

// data we pass to the fragment shader
out vec2 texCoord;
out vec3 Normal;
out vec3 pos;

void main() {
    // some code optimization
    // this product appears twice in our code
    // so we calculate it once
    vec4 p = model * vec4(aPos, 1);
    // first occurence in gl_Position calculation
    gl_Position = camera * p; // model* vec4(aPos, 1);
    texCoord = aTexCoord;
    // vertex position in space (no view)
    // second occurence in vertex position in 3D
    pos = vec3(p); //  model* vec4(aPos, 1));
    Normal = mat3(transpose(inverse(model))) * aNormal;
}