#pragma once
#ifndef CLOCK_H
#define CLOCK_H

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Deklaracija funkcije za crtanje sata
void drawClock(unsigned int shaderProgram, const glm::mat4& projection, const float clockColor[4]);

#endif // CLOCK_H