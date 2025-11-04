#version 330 core
layout(location = 0) in vec2 inPos;  // Pozicija verteksa
layout(location = 1) in vec4 inCol;  // Boja verteksa

uniform mat4 model;
uniform mat4 projection;

out vec4 chCol; 

void main() {
    gl_Position = projection * model * vec4(inPos, 0.0, 1.0);
    chCol = inCol;
}
