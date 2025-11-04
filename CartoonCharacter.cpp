#include "CartoonCharacter.h"
#include "stb_image.h"
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

// Constructor
CartoonCharacter::CartoonCharacter(const std::string& texturePath, float initialX, float initialY, float width, float height, float speed)
    : position(initialX, initialY, 0.0f), speed(speed), size(width, height) {
    // Load texture
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);


    if (textureID == 0) {
        std::cerr << "Error: Texture ID is 0 for " << texturePath << std::endl;
    }

    int texWidth, texHeight, nrChannels;
    stbi_set_flip_vertically_on_load(true); // okrece teksturu
    unsigned char* data = stbi_load(texturePath.c_str(), &texWidth, &texHeight, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, texWidth, texHeight, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cerr << "Failed to load texture: " << texturePath << std::endl;
    }
    stbi_image_free(data);

    if (data) {
        std::cout << "Loaded texture: " << texturePath << " (Width: " << texWidth << ", Height: " << texHeight << ")" << std::endl;
    }

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Set up VAO and VBO
    float vertices[] = {
        // Positions         // Texture Coords
        -width / 2,  height / 2,  0.0f, 1.0f, // Top-left
         width / 2,  height / 2,  1.0f, 1.0f, // Top-right
         width / 2, -height / 2,  1.0f, 0.0f, // Bottom-right
        -width / 2, -height / 2,  0.0f, 0.0f  // Bottom-left
    };
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);



    std::cout << "Generated VAO: " << VAO << ", VBO: " << VBO << ", EBO: " << EBO << std::endl;

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


CartoonCharacter::~CartoonCharacter() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteTextures(1, &textureID);
}

void CartoonCharacter::moveLeft(float leftBoundary) {
    if (position.x - size.x / 2 > leftBoundary) {
        position.x -= speed;
    }
}

void CartoonCharacter::moveRight(float rightBoundary) {
    if (position.x + size.x / 2 < rightBoundary) {
        position.x += speed;
    }
}
void CartoonCharacter::render(unsigned int shaderProgram, const glm::mat4& projection) {
    glUseProgram(shaderProgram);

    // Postavljanje matrice modela
    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    model = glm::scale(model, glm::vec3(size.x, size.y, 1.0f));

    // Povezivanje matrice modela i projekcije
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // Aktiviranje teksture i povezivanje
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0); // Povezivanje sa uniformom texture1

    // Nacrtaj lik
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // Crtanje likova

    glBindVertexArray(0);
    glUseProgram(0);
}

float CartoonCharacter::getX() const {
    return position.x;
}

float CartoonCharacter::getY() const {
    return position.y;
}

void CartoonCharacter::setX(float x) {
    position.x = x;
}

float CartoonCharacter::getWidth() const {
    return 0.2f; // Width of the character in OpenGL space
}