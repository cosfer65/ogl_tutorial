// fragment shader
#version 330 core

uniform int useColor;       // use color=1 or texture=0
uniform vec4 objectColor;   // object color to use
uniform vec3 cameraPos;     // viewer location
uniform samplerCube skybox; // skybox texture

in vec3 Normal;             // surface normal
in vec3 pos;                // drawing position

out vec4 color;             // resulting drawing color

void main() {
    // used when drawing text
    if (useColor == 1)
    {
        color = objectColor;
    }
    // used for textured objects
    else
    {
        // calculate reflection and not direct view
        vec3 view = normalize(pos - cameraPos);
        // reflect is a builtin function in GLSL
        vec3 reflection = reflect(view, normalize(Normal));
        // the reflection vector determines the output color
        color = vec4(texture(skybox, reflection).rgb, 1.0);
    }
}
