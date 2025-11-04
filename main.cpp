// Autor: Nedeljko Tesanovic
// Opis: Zestoko iskomentarisan program koji crta sareni trougao u OpenGL-u

#define _CRT_SECURE_NO_WARNINGS


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

 //Biblioteke za stvari iz C++-a (unos, ispis, fajlovi itd) 
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ShaderUtils.h"
#include "CircleUtils.h"
#include "TextRenderer.h"
#include <map>
#include "CartoonCharacter.h"
#include "Clock.h"
#include <thread>
#include <chrono>


//Biblioteke OpenGL-a
#include <GL/glew.h>   //Omogucava upotrebu OpenGL naredbi
#include <GLFW/glfw3.h>//Olaksava pravljenje i otvaranje prozora (konteksta) sa OpenGL sadrzajem

unsigned int loadTexture(const char* path);

// temena za generisanje kruga -> dugmeta
CircleButton button = { 100.0f, 95.0f, 17.0f, {1.0f, 1.0f, 1.0f, 1.0f}, false }; // Dugme
std::vector<float> circleVertices = generateCircle(button, 36);

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    ::mouse_button_callback(window, button, action, mods, ::button); // Callback samo za dugme
}

std::deque<std::string> newsQueue = {
    "BREAKING NEWS BOSTON CELTICS RETURN TO NO1 SPOT!",
    "KIA MVP LADDER ANTETOKOUNMPO STORMS INTO TOP 5 !!!"
};
float scrollOffset = 0.0f; // Početni offset


bool isSettingsMode = false; // Da li je mod podešavanja aktivan
int selectedOption = 0;      // Trenutna selektovana opcija
float brightness = 1.0f;     // Osvetljenje
float clockColor[4] = { 0.0f, 1.0f, 1.0f, 1.0f };
double lastActionTime = 0.0; // Vreme poslednje akcije

void renderOptions(unsigned int textShader, unsigned int fontTexture, unsigned int textVAO, unsigned int textVBO);
void handleSettingsInput(GLFWwindow* window, bool& isSettingsMode, float& brightness, float clockColor[4]);


int main(void)
{

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ INICIJALIZACIJA ++++++++++++++++++++++++++++++++++++++++++++++++++++++

    bool isChannel0 = true;
    bool isChannel1 = false;
    bool isChennel2 = false;
    bool isChannel3 = false;

    // Pokretanje GLFW biblioteke
    // Nju koristimo za stvaranje okvira prozora / konteksta
    if (!glfwInit()) // !0 == 1  | glfwInit inicijalizuje GLFW i vrati 1 ako je inicijalizovana uspjesno, a 0 ako nije
    {
        std::cout<<"GLFW Biblioteka se nije ucitala! :(\n";
        return 1;
    }

    //Odredjivanje OpenGL verzije i profila (3.3, programabilni pajplajn)
    //bez ovoga, koristi se najnoviji moguci OpenGL koji hardver podrzava
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //Stvaranje prozora
    GLFWwindow* window; //Mjesto u memoriji za prozor
    unsigned int wWidth = 800;
    unsigned int wHeight = 600;
    const char wTitle[] = "[Generic Title]";
    window = glfwCreateWindow(wWidth, wHeight, wTitle, NULL, NULL); // Napravi novi prozor
    // glfwCreateWindow( sirina, visina, naslov, monitor na koji ovaj prozor ide preko citavog ekrana (u tom slucaju umjesto NULL ide glfwGetPrimaryMonitor() ), i prozori sa kojima ce dijeliti resurse )
    if (window == NULL) //Ako prozor nije napravljen
    {
        std::cout << "Prozor nije napravljen! :(\n";
        glfwTerminate(); //Gasi GLFW
        return 2; //Vrati kod za gresku
    }
    // Postavljanje novopecenog prozora kao aktivni (sa kojim cemo da radimo)
    glfwMakeContextCurrent(window);

    // Inicijalizacija GLEW biblioteke
    if (glewInit() != GLEW_OK) //Slicno kao glfwInit. GLEW_OK je predefinisani kod za uspjesnu inicijalizaciju sadrzan unutar biblioteke
    {
        std::cout << "GLEW nije mogao da se ucita! :'(\n";
        return 3;
    }

    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ PROMJENLJIVE I BAFERI +++++++++++++++++++++++++++++++++++++++++++++++++

    unsigned int basicShader = createShader("basic.vert", "basic.frag"); // Napravi objedinjeni sejder program
    unsigned int basicShader2 = createShader("basic2.vert", "basic.frag"); 
    unsigned int pulsatingShader = createShader("basic.vert", "pulsating.frag");
    unsigned int textShader = createShader("text.vert", "text.frag");
    unsigned int colorShader = createShader("basic.vert", "color.frag");
    unsigned int characterShader = createShader("character.vert", "character.frag");
    unsigned int overlayShader = createShader("overlay.vert", "overlay.frag");

    //teksture za karaktere
    CartoonCharacter hero1("fica.png", 200.0f, 300.0f, 12.0f, 12.0f, 2.0f); 
    CartoonCharacter hero2("fedja.png", 600.0f, 300.0f, 12.0f, 12.0f, 2.0f);
    CartoonCharacter reporter("reporter.png", 400.0f, 340.0f, 16.0f, 16.0f, 2.0f);

    // Pravougaonik za sloj osvetljenja
    float overlayVertices[] = {
        70.0f, 500.0f,  // Levo gore
        730.0f, 500.0f, // Desno gore
        730.0f, 120.0f,  // Desno dole
        70.0f, 120.0f    // Levo dole
    };

    float vertices[] =
    {
        // Veći sivi pravougaonik (pozicije u pikselima, boje ostaju iste)
        // Pozicija         | Boja
        50.0f, 530.0f,     0.34f, 0.23f, 0.15f, 1.0f, // Levo gore
        750.0f, 530.0f,     0.34f, 0.23f, 0.15f, 1.0f, // Desno gore
        750.0f, 70.0f,     0.34f, 0.23f, 0.15f, 1.0f, // Desno dole
        50.0f, 70.0f,     0.34f, 0.23f, 0.15f, 1.0f, // Levo dole

        // Manji crni pravougaonik
        70.0f, 500.0f,     0.0f, 0.0f, 0.0f, 1.0f,   // Levo gore
        730.0f, 500.0f,     0.0f, 0.0f, 0.0f, 1.0f,   // Desno gore
        730.0f, 120.0f,     0.0f, 0.0f, 0.0f, 1.0f,   // Desno dole
        70.0f, 120.0f,     0.0f, 0.0f, 0.0f, 1.0f    // Levo dole
    };


    unsigned int indices[] = {
        // Veći pravougaonik
        0, 1, 2,   // Prvi trougao
        2, 3, 0,   // Drugi trougao

        // Manji pravougaonik
        4, 5, 6,   // Prvi trougao
        6, 7, 4    // Drugi trougao
    };

    unsigned int stride = (2 + 4) * sizeof(float); //Korak pri kretanju po podacima o tjemenima = Koliko mjesta u memoriji izmedju istih komponenti susjednih tjemena

    //Vertex Array Object - Sadrzi bar 16 pokazivaca na atribute koji opusuju sadrzaje bafer objekata
    unsigned int VAO;
    glGenVertexArrays(1, &VAO); //Generisi 1 VAO na adresi datoj adresi
    glBindVertexArray(VAO); //Povezi VAO za aktivni kontekst - sva naknadna podesavanja ce se odnositi na taj VAO

    //Vertex Buffer Object - Bafer objekat, u ovom slucaju za tjemena trougla koji crtamo
    unsigned int VBO;
    glGenBuffers(1, &VBO); //Generisi 1 bafer na datoj adresi 
    glBindBuffer(GL_ARRAY_BUFFER, VBO); //Povezi dati bafer za aktivni kontekst. Array buffer se koristi za tjemena figura.
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //Slanje podataka na memoriju graficke karte
    //glBufferData(koji bafer, koliko podataka ima, adresa podataka, nacin upotrebe podataka (GL_STATIC_DRAW, GL_DYNAMIC_DRAW, GL_STREAM_DRAW;)

    // Element Buffer Object - Bafer za indekse
    unsigned int EBO;
    glGenBuffers(1, &EBO); // Generiši EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // Poveži EBO
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); // Slanje podataka za indekse

    // Podešavanje pokazivača na atribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0); // Pozicija (x, y)
    glEnableVertexAttribArray(0); // Aktiviraj pokazivač na poziciju
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float))); // Boja (r, g, b, a)
    glEnableVertexAttribArray(1); // Aktiviraj pokazivač na boju

    //Postavili smo sta treba, pa te stvari iskljucujemo, da se naknadna podesavanja ne bi odnosila na njih i nesto poremetila
    //To radimo tako sto bindujemo 0, pa kada treba da nacrtamo nase stvari, samo ih ponovo bindujemo
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    //-------------------- DUGME ---------------
    unsigned int circleVBO, circleVAO;
    glGenVertexArrays(1, &circleVAO);
    glBindVertexArray(circleVAO);

    glGenBuffers(1, &circleVBO);
    glBindBuffer(GL_ARRAY_BUFFER, circleVBO);
    glBufferData(GL_ARRAY_BUFFER, circleVertices.size() * sizeof(float), circleVertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // Pozicija
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float))); // Boja
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


   // ------------- TEKST ----------
    loadFont("myFont.fnt");
    unsigned int fontTexture = loadTexture("myFont_0.tga");

    std::cout << "Loaded characters: " << Characters.size() << std::endl;


    if (fontTexture == 0) {
        std::cout << "Font texture failed to load!" << std::endl;
    }

    unsigned int textVAO, textVBO;
    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBO);

    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW); // 6 verteksa sa 4 vrednosti (x, y, s, t)
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    // osvetljenje

    unsigned int overlayVAO, overlayVBO;
    glGenVertexArrays(1, &overlayVAO);
    glGenBuffers(1, &overlayVBO);

    glBindVertexArray(overlayVAO);
    glBindBuffer(GL_ARRAY_BUFFER, overlayVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(overlayVertices), overlayVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ RENDER LOOP - PETLJA ZA CRTANJE +++++++++++++++++++++++++++++++++++++++++++++++++

    glClearColor(1.0, 1.0, 1.0, 1.0); //Podesavanje boje pozadine (RGBA format);

    const double targetFPS = 60.0; // Ciljani broj FPS
    const double frameDuration = 1.0 / targetFPS; // Trajanje jednog okvira u sekundama
    double lastFrameTime = glfwGetTime(); // Vreme poslednjeg renderovanja

    while (!glfwWindowShouldClose(window)) //Beskonacna petlja iz koje izlazimo tek kada prozor treba da se zatvori
    {
        double currentTime = glfwGetTime();
        double elapsedTime = currentTime - lastFrameTime;

        if (elapsedTime < frameDuration) {
            // Pauza da bismo dostigli ciljani FPS
            double sleepTime = frameDuration - elapsedTime;
            if (sleepTime > 0) {
                std::this_thread::sleep_for(std::chrono::duration<double>(sleepTime));
            }
            continue;
        }

        lastFrameTime = glfwGetTime();

        glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f); // Levo, desno, dole, gore
        //Unos od korisnika bez callback funckcije. GLFW_PRESS = Dugme je trenutno pritisnuto. GLFW_RELEASE = Dugme trenutno nije pritisnuto
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
            isChannel0 = false;
            isChannel1 = true;
            isChennel2 = false;
            isChannel3 = false;
        }
        if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
            isChannel0 = true; 
            isChannel1 = false;
            isChennel2 = false;
            isChannel3 = false;
        }
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
            isChannel0 = false;
            isChannel1 = false;
            isChennel2 = true;
            isChannel3 = false;
        }
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
            isChannel0 = false;
            isChannel1 = false;
            isChennel2 = false;
            isChannel3 = true;
        }

        //Brisanje ekrana
        glClear(GL_COLOR_BUFFER_BIT);

        // [KOD ZA CRTANJE]
        glUseProgram(basicShader); //Izaberi nas sejder program za crtanje i koristi ga za svo naknadno crtanje (Ukoliko ne aktiviramo neke druge sejder programe)
        glUniformMatrix4fv(glGetUniformLocation(basicShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glBindVertexArray(VAO); //Izaberemo sta zelimo da crtamo
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // Crtanje prvog pravouganoika
        //void glDrawElements(	GLenum mode, GLsizei count, GLenum type,const void* indices);

        //glDrawArrays(GL_TRIANGLES, 0, 3); //To i nacrtamo
        //glDrawArrays(tip primitive, indeks pocetnog tjemena, koliko narednih tjemena crtamo)

        if (button.isClicked) {
            glUseProgram(colorShader);
            glUniformMatrix4fv(glGetUniformLocation(colorShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            if (isChannel1) {  
                glUniform4f(glGetUniformLocation(colorShader, "color"), 0.0f, 0.03f, 0.3f, 1.0f);
            }
            else if (isChennel2) {   
                glUniform4f(glGetUniformLocation(colorShader, "color"), 0.76f, 0.92f, 0.92f, 1.0f);
            }
            else if (isChannel3) {
                glUniform4f(glGetUniformLocation(colorShader, "color"), 0.0f, 0.0f, 0.0f, 1.0f);
            }
            else {  
                glUniform4f(glGetUniformLocation(colorShader, "color"), 1.0f, 1.0f, 1.0f, 1.0f); // Beli pravougaonik
            }
        }
        else {
            glUseProgram(basicShader); // Ostavi ga crnim u osnovnom stanju
        }
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(6 * sizeof(unsigned int))); // Renderuj drugi pravougaonik
        glBindVertexArray(0);
        glUseProgram(0);

        // crtanje dugmeta
        if (button.isClicked) {
            glUseProgram(colorShader);
            glUniformMatrix4fv(glGetUniformLocation(colorShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            glUniform4f(glGetUniformLocation(colorShader, "color"), button.color[0], button.color[1], button.color[2], button.color[3]);
        }
        else {
            glUseProgram(pulsatingShader); // Ako nije kliknuto, koristi pulsirajući šejder
            glUniformMatrix4fv(glGetUniformLocation(pulsatingShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            float time = glfwGetTime();
            float intensity = (sin(time * 3.0f) + 1.0f) / 2.0f;
            glUniform1f(glGetUniformLocation(pulsatingShader, "intensity"), intensity);
        }
        glBindVertexArray(circleVAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, circleVertices.size() / 6); // Koristite broj tačaka iz circleVertices
        glBindVertexArray(0);
        glUseProgram(0);

        glBindVertexArray(circleVAO);
        //glDrawArrays(GL_TRIANGLE_FAN, 0, 38); // 37 = 1 centar + 36 tačaka oboda + pocetna tacka ponovo
        glDrawArrays(GL_TRIANGLE_FAN, 0, circleVertices.size() / 6);
        glBindVertexArray(0);
        glUseProgram(0);

       
        glUseProgram(textShader);
        glUniformMatrix4fv(glGetUniformLocation(textShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        // Renderuj tekst "RA 98/2021" desno od dugmeta
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        renderText("RA 98/2021", 330.0f, 95.0f, 1.0f, 0.38f, 0.68f, 0.68f, textShader, fontTexture, textVAO, textVBO);
        glUseProgram(0);

        if (button.isClicked) {
            if (isChannel1) {
                drawClock(basicShader2, projection, clockColor);
            }
            else if (isChennel2) {
                reporter.render(characterShader, projection);
                renderScrollingText(newsQueue, scrollOffset, 740.0f, 180.0f, 1.3f, 1.4f, 1.0f, 0.0f, 0.0f, textShader, fontTexture, textVAO, textVBO);
            }
            else if (isChannel3) {

                isSettingsMode = true;
                renderOptions(textShader, fontTexture, textVAO, textVBO);
                handleSettingsInput(window, isSettingsMode, brightness, clockColor);
            }
            else if (isChannel0) {
                // crtanje karaktera na ekranu

                    // Granice pravougaonika
                float leftBoundary = 115.0f;
                float rightBoundary = 695.0f;

                // Kretanje hero1
                if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
                    hero1.moveLeft(leftBoundary + 20.0);
                }
                if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
                    hero1.moveRight(rightBoundary / 2 - 10.0); // Ne sme preći na desnu polovinu
                }

                // Kretanje hero2
                if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
                    hero2.moveLeft(rightBoundary / 2 + 90.0); // Ne sme preći na levu polovinu
                }
                if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
                    hero2.moveRight(rightBoundary - 20.0);
                }

                // Renderovanje
                hero1.render(characterShader, projection);
                hero2.render(characterShader, projection);
            }
            glUseProgram(overlayShader);
            glUniformMatrix4fv(glGetUniformLocation(overlayShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            glUniform4f(glGetUniformLocation(overlayShader, "overlayColor"), 0.0f, 0.0f, 0.0f, 1.0f - brightness); // Transparentnost zavisi od osvetljenja

            glBindVertexArray(overlayVAO);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4); // Pravougaonik
            glBindVertexArray(0);
            glUseProgram(0);
        }
        //Zamjena vidljivog bafera sa pozadinskim
        glfwSwapBuffers(window);

        //Hvatanje dogadjaja koji se ticu okvira prozora (promjena velicine, pomjeranje itd)
        glfwPollEvents();
    }
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ POSPREMANJE +++++++++++++++++++++++++++++++++++++++++++++++++
        //Brisanje bafera i sejdera
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(basicShader);
    //Sve OK - batali program
    glfwTerminate();
    return 0;
}

unsigned int loadTexture(const char* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(false);
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = GL_RED; // Pošto je .tga verovatno crno-bela tekstura
        if (nrChannels == 3) format = GL_RGB;
        else if (nrChannels == 4) format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else {
        std::cout << "Failed to load texture: " << path << std::endl;
    }
    stbi_image_free(data);

    std::cout << "Loaded texture: " << path << " (" << width << "x" << height << ")\n";

    return textureID;
}


void renderOptions(unsigned int textShader, unsigned int fontTexture, unsigned int textVAO, unsigned int textVBO) {
    std::vector<std::string> options = {
        "VOLUME UP",
        "VOLUME DOWN",
        "BRIGHTNESS UP",
        "BRIGHTNESS DOWN",
        "CHANGE CLOCK COLOR"
    };

    float startY = 400.0f; // Početna Y pozicija za tekst
    float spacing = 40.0f; // Razmak između linija
    float x = 200.0f;      // X pozicija za opcije

    for (size_t i = 0; i < options.size(); ++i) {
        if (i == selectedOption) {
            // Selektovana opcija: renderuj pozadinu
            glEnable(GL_SCISSOR_TEST);
            glScissor(x - 10, startY - i * spacing - 20, 400, 40);
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glDisable(GL_SCISSOR_TEST);

            // Crni tekst na beloj pozadini
            renderText(options[i], x, startY - i * spacing, 1.0f, 0.0f, 0.0f, 0.0f, textShader, fontTexture, textVAO, textVBO);
        }
        else {
            // Belo slovo na crnoj pozadini
            renderText(options[i], x, startY - i * spacing, 1.0f, 1.0f, 1.0f, 1.0f, textShader, fontTexture, textVAO, textVBO);
        }
    }
}

void handleSettingsInput(GLFWwindow* window, bool& isSettingsMode, float& brightness, float clockColor[4]) {
    double currentTime = glfwGetTime(); // Trenutno vreme

    // Proveri da li je prošlo dovoljno vremena od poslednje akcije
    if (currentTime - lastActionTime < 0.2) {
        return; // Izbegavaj prebrze akcije
    }

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        selectedOption = (selectedOption - 1 + 5) % 5; // Kreće se gore, pet opcija
        lastActionTime = currentTime;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        selectedOption = (selectedOption + 1) % 5; // Kreće se dole
        lastActionTime = currentTime;
    }

    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
        lastActionTime = currentTime;
        switch (selectedOption) {
        case 0:
            std::cout << "Povećavanje jačine zvuka\n";
            break;
        case 1:
            std::cout << "Smanjivanje jačine zvuka\n";
            break;
        case 2:
            brightness = std::min(brightness + 0.1f, 1.0f);
            break;
        case 3:
            brightness = std::max(brightness - 0.1f, 0.0f);
            break;
        case 4: {
            // Promeni boju sata
            static int colorIndex = 0;
            const float colors[][4] = {
                {0.0f, 1.0f, 0.0f, 1.0f}, // Zelena
                {0.0f, 0.0f, 1.0f, 1.0f}, // Plava
                {1.0f, 1.0f, 0.0f, 1.0f}, // Žuta
                {1.0f, 0.0f, 1.0f, 1.0f}, // Ljubičasta
                {0.0f, 1.0f, 1.0f, 1.0f}  // Tirkizna
            };
            colorIndex = (colorIndex + 1) % 5;
            for (int i = 0; i < 4; i++) {
                clockColor[i] = colors[colorIndex][i];
            }
            break;
        }
        }
    }
}
