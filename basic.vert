#version 330 core

layout(location = 0) in vec2 inPos; // Pozicija tjemena
layout(location = 1) in vec4 inCol; // Boja tjemena - ovo saljemo u fragment sejder
out vec4 chCol; // Izlazni kanal kroz koji saljemo boju do fragment sejdera

uniform mat4 projection; // Projekcija za transformaciju koordinata

void main() {
    gl_Position = projection * vec4(inPos.xy, 0.0, 1.0); // Koristi projekciju
    chCol = inCol;
}