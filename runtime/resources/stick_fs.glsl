#version 330 core
uniform vec4 objectColor; // object color
out vec4 color;           // resulting drawing color

void main() {
    color = objectColor;
}
