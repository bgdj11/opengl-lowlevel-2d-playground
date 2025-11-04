#version 330 core

layout(location = 0) in vec2 inPos;  // Pozicija verteksa
layout(location = 1) in vec2 inTex;  // Teksturne koordinate

out vec2 TexCoords;  // Prosledi teksturne koordinate u fragment šejder

uniform mat4 model;      // Model matrica
uniform mat4 projection; // Projekcija

void main() {
    gl_Position = projection * model * vec4(inPos, 0.0, 1.0);
    TexCoords = inTex;
}