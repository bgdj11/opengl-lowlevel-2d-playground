#include "Clock.h"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "CircleUtils.h"
#include <iostream> // Za debug ispis
#include <glm/gtx/string_cast.hpp>

void drawClock(unsigned int shaderProgram, const glm::mat4& projection, const float clockColor[4]) {
    static float angle = 0.0f; // Ugao kazaljke
    angle -= 0.02f;           // Povećanje ugla kazaljke (rotacija)

    // Postavite centar i poluprečnik kruga za sat
    float clockCenterX = 400.0f; // U pikselima
    float clockCenterY = 300.0f; // U pikselima
    float clockRadius = 100.0f;  // U pikselima

    // Generišite vertekse za krug
    CircleButton clockCircle = { 400.0f, 300.0f, 100.0f, {clockColor[0], clockColor[1], clockColor[2], clockColor[3]}, false}; // Sat
    std::vector<float> clockVertices = generateCircle(clockCircle, 72);

    // Kreirajte VAO i VBO za krug
    unsigned int clockVAO, clockVBO;
    glGenVertexArrays(1, &clockVAO);
    glGenBuffers(1, &clockVBO);

    glBindVertexArray(clockVAO);

    glBindBuffer(GL_ARRAY_BUFFER, clockVBO);
    glBufferData(GL_ARRAY_BUFFER, clockVertices.size() * sizeof(float), clockVertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // Pozicija
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float))); // Boja
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

   

    // Koristite shader program
    glUseProgram(shaderProgram);

    // Provera uniforma za projekciju
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
    if (projLoc == -1) {
        std::cerr << "Projection uniform not found in shader!" << std::endl;
    }
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Crtajte krug (resetujemo `model` matricu za krug)
    glm::mat4 circleModel = glm::mat4(1.0f);
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    if (modelLoc == -1) {
        std::cerr << "Model uniform not found in shader for circle!" << std::endl;
    }
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(circleModel));


    glBindVertexArray(clockVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, clockVertices.size() / 6);
    glBindVertexArray(0);

    // Oslobodite resurse kruga
    glDeleteBuffers(1, &clockVBO);
    glDeleteVertexArrays(1, &clockVAO);

    // Crtanje kazaljke
    float handWidth = 5.0f;    // Širina kazaljke
    float handLength = 80.0f;  // Dužina kazaljke

    float handVertices[] = {
        -handWidth / 2, 0.0f, 0.8f, 0.1f, 0.1f, 1.0f, // Donja leva
         handWidth / 2, 0.0f, 0.8f, 0.1f, 0.1f, 1.0f, // Donja desna
         handWidth / 2, handLength, 0.8f, 0.1f, 0.1f, 1.0f, // Gornja desna
        -handWidth / 2, handLength, 0.8f, 0.1f, 0.1f, 1.0f  // Gornja leva
    };

    unsigned int handIndices[] = { 0, 1, 2, 2, 3, 0 };

    unsigned int handVAO, handVBO, handEBO;
    glGenVertexArrays(1, &handVAO);
    glBindVertexArray(handVAO);

    glGenBuffers(1, &handVBO);
    glBindBuffer(GL_ARRAY_BUFFER, handVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(handVertices), handVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &handEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(handIndices), handIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // Pozicija
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float))); // Boja
    glEnableVertexAttribArray(1);

    // Kreirajte `model` matricu za kazaljku
    glm::mat4 handModel = glm::translate(glm::mat4(1.0f), glm::vec3(clockCenterX, clockCenterY, 0.0f));
    handModel = glm::rotate(handModel, angle, glm::vec3(0.0f, 0.0f, 1.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(handModel));


    glBindVertexArray(handVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Oslobodite resurse kazaljke
    glDeleteBuffers(1, &handVBO);
    glDeleteBuffers(1, &handEBO);
    glDeleteVertexArrays(1, &handVAO);

    glUseProgram(0);
}
