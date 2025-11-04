#define _CRT_SECURE_NO_WARNINGS

#include "TextRenderer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <map>

std::map<char, Character> Characters;

using namespace std;

void loadFont(const std::string& fontFile) {
    std::ifstream file(fontFile);
    if (!file.is_open()) {
        std::cerr << "Failed to open font file: " << fontFile << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.substr(0, 4) == "char") { // Parsiraj linije sa "char" informacijama
            int id, x, y, width, height, xOffset, yOffset, xAdvance;
            sscanf(line.c_str(),
                "char id=%d    x=%d    y=%d    width=%d    height=%d    xoffset=%d    yoffset=%d    xadvance=%d",
                &id, &x, &y, &width, &height, &xOffset, &yOffset, &xAdvance);

            Characters[id] = {
                (float)x, (float)y, (float)width, (float)height,
                (float)xOffset, (float)yOffset, (float)xAdvance
            };

            // provera
            std::cout << "Loaded character: " << id << ", x=" << x << ", y=" << y
                << ", width=" << width << ", height=" << height
                << ", xOffset=" << xOffset << ", yOffset=" << yOffset
                << ", xAdvance=" << xAdvance << std::endl;

        }
    }
}

void renderText(std::string text, float x, float y, float scale, float r, float g, float b, unsigned int textShader, unsigned int fontTexture, unsigned int textVAO, unsigned int textVBO) {
    glUseProgram(textShader);

    // Postavi uniform za boju teksta
    glUniform3f(glGetUniformLocation(textShader, "textColor"), r, g, b);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fontTexture);
    glBindVertexArray(textVAO);

    // Renderuj svaki karakter
    float cursorX = x; // Početna x-pozicija
    float cursorY = y; // Početna y-pozicija

    for (char c : text) {
        if (Characters.find(c) == Characters.end()) continue; // Preskoči karakter ako ne postoji

        Character ch = Characters[c];

        // Računaj pozicije na osnovu trenutnog kursora i offseta karaktera
        float xpos = cursorX + ch.xOffset * scale;
        float ypos = cursorY - (ch.height - ch.yOffset) * scale;
        float w = ch.width * scale;
        float h = ch.height * scale;

        // Poboljšano skaliranje teksturnih koordinata
        float texLeft = (ch.x + 0.5f) / 256.0f;
        float texRight = (ch.x + ch.width - 0.5f) / 256.0f;
        float texTop = (ch.y + 0.5f) / 256.0f;
        float texBottom = (ch.y + ch.height - 0.5f) / 256.0f;

        // Vertices za trenutni kvadrat
        float vertices[6][4] = {
            { xpos,     ypos + h,   texLeft,  texTop    },
            { xpos,     ypos,       texLeft,  texBottom },
            { xpos + w, ypos,       texRight, texBottom },

            { xpos,     ypos + h,   texLeft,  texTop    },
            { xpos + w, ypos,       texRight, texBottom },
            { xpos + w, ypos + h,   texRight, texTop    }
        };

        // Napuni VBO
        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        // Iscrtavanje kvadrata
        glBindTexture(GL_TEXTURE_2D, fontTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Povećaj x-poziciju za sledeći karakter
        cursorX += ch.xAdvance * scale;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

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
    unsigned int textVBO)
{
    // Ako nema vesti, izađi iz funkcije
    if (newsQueue.empty()) return;

    // Definiši granice pravougaonika za prikaz teksta
    const int clipX = 70;        // Leva ivica pravougaonika
    const int clipY = 120;       // Donja ivica pravougaonika
    const int clipWidth = 730 - 70;  // Širina pravougaonika
    const int clipHeight = 500 - 120; // Visina pravougaonika

    // Ukloni prvu vest ako je nestala sa leve strane
    if (scrollOffset + xStart < -clipWidth) { // Ako je tekst u potpunosti izašao
        scrollOffset = 0.0f; // Resetuj offset
        newsQueue.push_back(newsQueue.front()); // Vrati prvu vest na kraj
        newsQueue.pop_front(); // Ukloni prvu vest
    }

    // Pomeri offset u zavisnosti od brzine
    scrollOffset -= speed;

    // Aktiviraj "scissoring" da ograničiš prikaz na pravougaonik
    glEnable(GL_SCISSOR_TEST);
    glScissor(clipX, clipY, clipWidth, clipHeight); // Postavi granice pravougaonika

    // Renderuj trenutno aktivnu vest unutar granica
    renderText(newsQueue.front(), xStart + scrollOffset, y, scale, r, g, b, textShader, fontTexture, textVAO, textVBO);

    // Isključi "scissoring" nakon renderovanja
    glDisable(GL_SCISSOR_TEST);
}
