#pragma once
#ifndef CARTOON_CHARACTER_H
#define CARTOON_CHARACTER_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

class CartoonCharacter {
private:
    GLuint textureID;
    GLuint VAO, VBO, EBO;
    glm::vec3 position;
    glm::vec2 size;
    float speed;

public:
    CartoonCharacter(const std::string& texturePath, float initialX, float initialY, float width, float height, float speed);
    ~CartoonCharacter();

    void moveLeft(float leftBoundary);
    void moveRight(float rightBoundary);
    void render(GLuint shaderProgram, const glm::mat4& projection);
    float getX() const;
    float getY() const;
    void setX(float x);
    float getWidth() const;
};

#endif