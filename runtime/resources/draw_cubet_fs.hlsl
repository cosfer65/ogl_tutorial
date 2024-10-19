#version 330 core
in vec2 texCoord;
uniform sampler2D textureSampler;
// drawing color for OpenGL to use
out vec4 color;
void main() {
    color = vec4(texture(textureSampler, texCoord).rgb, 1);
}