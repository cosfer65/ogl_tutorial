#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightSpaceMatrix;

out vec3 pos;
out vec3 Normal;
out vec2 TexCoords;
out vec4 posLightSpace;

void main()
{
    TexCoords = aTexCoords;
    pos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    posLightSpace = lightSpaceMatrix * vec4(pos, 1.0);
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
