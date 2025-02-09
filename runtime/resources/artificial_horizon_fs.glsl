#version 330 core

uniform vec3 objectColor;   // object color to use
uniform vec3 cameraPos;     // viewer location

in vec3 Normal;             // surface normal
in vec3 pos;                // drawing position

out vec4 color;             // resulting drawing color

void main() {
        color = vec4(objectColor, 1);
}
