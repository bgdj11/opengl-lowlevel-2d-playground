#pragma once
#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <string>
#include <map>
#include <GL/glew.h>
#include <deque>

// Struktura za karakter
struct Character {
    float x;       // X koordinata u teksturi
    float y;       // Y koordinata u teksturi
    float width;   // Širina karaktera
    float height;  // Visina karaktera
    float xOffset; // Horizontalni pomak
    float yOffset; // Vertikalni pomak
    float xAdvance;// Razmak do sledećeg karaktera
};

extern std::map<char, Character> Characters;

void loadFont(const std::string& fontFile);
void renderText(std::string text, float x, float y, float scale, float r, float g, float b, unsigned int textShader, unsigned int fontTexture, unsigned int textVAO, unsigned int textVBO);

void renderScrollingText(
    std::deque<std::string>& newsQueue, // Lista vesti (FIFO)
    float& scrollOffset,                // Offset za animaciju
    float xStart,                       // Početna x-pozicija (desna ivica ekrana)
    float y,                            // Y-pozicija teksta
    float scale,                        // Skala teksta
    float speed,                        // Brzina skrolovanja
    float r, float g, float b,          // Boja teksta
    unsigned int textShader,
    unsigned int fontTexture,
    unsigned int textVAO,
    unsigned int textVBO);


#endif