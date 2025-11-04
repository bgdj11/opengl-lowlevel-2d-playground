#ifndef CIRCLE_UTILS_H
#define CIRCLE_UTILS_H

#include <GLFW/glfw3.h>
#include <vector>

// Struktura za opisivanje kruga ili dugmeta
struct CircleButton {
    float centerX;
    float centerY;
    float radius;
    float color[4]; // RGBA
    bool isClicked; // Stanje kliknutosti
};

// Generisanje verteksa za krug
std::vector<float> generateCircle(const CircleButton& circle, int segments);

// Funkcija za detekciju klikova mišem (radi sa prosleđenim krugom)
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods, CircleButton& circle);

#endif
