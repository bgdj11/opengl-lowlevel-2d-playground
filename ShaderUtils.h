#pragma once
#ifndef SHADER_UTILS_H
#define SHADER_UTILS_H

#include <string>
#include <GL/glew.h>

// Funkcije za sejder
unsigned int compileShader(GLenum type, const char* source);
unsigned int createShader(const char* vsSource, const char* fsSource);

#endif