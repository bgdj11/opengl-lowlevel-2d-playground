#include "CircleUtils.h"
#include <cmath>
#include <vector>
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

std::vector<float> generateCircle(const CircleButton& circle, int segments) {
    std::vector<float> circleVertices;

    // Dodaj centar kruga
    circleVertices.push_back(circle.centerX);
    circleVertices.push_back(circle.centerY);
    circleVertices.push_back(circle.color[0]); // R
    circleVertices.push_back(circle.color[1]); // G
    circleVertices.push_back(circle.color[2]); // B
    circleVertices.push_back(circle.color[3]); // A

    // Dodaj verteks podatke za obod kruga
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * M_PI * i / segments;
        float x = circle.centerX + circle.radius * cos(angle);
        float y = circle.centerY + circle.radius * sin(angle);

        circleVertices.push_back(x);
        circleVertices.push_back(y);
        circleVertices.push_back(circle.color[0]); // R
        circleVertices.push_back(circle.color[1]); // G
        circleVertices.push_back(circle.color[2]); // B
        circleVertices.push_back(circle.color[3]); // A
    }

    return circleVertices;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods, CircleButton& circle) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        // Preuzmi koordinate miša u pikselima
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        // Dimenzije prozora
        int windowWidth, windowHeight;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);

        // Transformacija Y ose u OpenGL koordinatni prostor
        float normalizedMouseX = static_cast<float>(mouseX);
        float normalizedMouseY = static_cast<float>(windowHeight - mouseY); // Obrnuta Y-os zbog OpenGL-a

        // Proveri da li miš klikne unutar prosleđenog kruga
        float dx = normalizedMouseX - circle.centerX;
        float dy = normalizedMouseY - circle.centerY;

        if (dx * dx + dy * dy <= circle.radius * circle.radius) {
            // Promeni stanje kruga
            circle.isClicked = !circle.isClicked;
            std::cout << "Circle clicked! New state: " << circle.isClicked << std::endl;

            // Ažuriraj boju kruga
            if (circle.isClicked) {
                circle.color[0] = 1.0f; // Roze
                circle.color[1] = 0.0f;
                circle.color[2] = 1.0f;
                circle.color[3] = 1.0f;
            }
            else {
                circle.color[0] = 1.0f; // Bela
                circle.color[1] = 1.0f;
                circle.color[2] = 1.0f;
                circle.color[3] = 1.0f;
            }
        }
        else {
            std::cout << "Click outside of circle." << std::endl;
        }
    }
}
