#version 330 core

// Ulaz iz vertex shader-a
in vec2 TexCoords;

// Izlazna boja
out vec4 FragColor;

// Uniform za teksturu
uniform sampler2D texture1;

void main() {
    FragColor = texture(texture1, TexCoords); // Dohvata boju iz teksture na osnovu koordinata
}
