#version 330 core

in vec4 chCol;       // Boja iz verteks šejdera
out vec4 outCol;     // Završna boja piksela

uniform float intensity; // Uniform za intenzitet pulsiranja (0.0 do 1.0)

void main() {
    // Interpoliraj između crne i bele boje na osnovu intenziteta
    vec4 pulsatingColor = mix(vec4(0.0, 0.0, 0.0, 1.0), vec4(1.0, 1.0, 1.0, 1.0), intensity);
    outCol = pulsatingColor * chCol; // Kombinuj sa bojom iz verteks šejdera
}
