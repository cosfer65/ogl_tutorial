#version 440 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

uniform mat4 camera;
uniform mat4 model;

out vec2 texCoord;
out vec3 Normal;
out vec3 pos;

void main()
{
    vec4 p = model * vec4(aPos, 1);
    gl_Position = camera * p;
    
    texCoord = aTexCoord;
    pos = vec3(p);
    Normal = mat3(transpose(inverse(model))) * aNormal;
}
