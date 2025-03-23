// fragment shader
#version 330 core

uniform vec3 cameraPos;     // viewer location
uniform samplerCube skybox; // skybox texture

in vec3 Normal;             // surface normal
in vec3 pos;                // drawing position

out vec4 color;             // resulting drawing color

void main() {
    // calculate reflection and not direct view
    vec3 view = normalize(pos - cameraPos);
    // reflect is a builtin function in GLSL
    vec3 reflection = reflect(view, normalize(Normal));
    // the reflection vector determines the output color
    color = vec4(texture(skybox, reflection).rgb, 1.0);
}
