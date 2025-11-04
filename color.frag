#version 330 core

in vec4 chCol;       // Boja iz verteks šejdera
out vec4 outCol;     // Završna boja piksela

uniform vec4 color; // Uniform za boju kruga

void main() {
    outCol = color; 
}
