#pragma once
#include <string>
#include <vector>
#include "raylib.h"

// provisorisch, später durch echte Liste ersetzt
struct Inventar_Items {
    std::string name;
    int anzahl;
};

enum direction {
    Up,
    Down,
    Right,
    Left
};

class player {
private:
    float posX;
    float posY;
    float speed = 100.0f;   // Pixel pro Sekunde (bei zoom=4 wirkt es langsamer)
    std::vector<Inventar_Items> Inventar;
    std::string name;

public:
    void Set_position(int posx, int posy) {
        posX = (float)posx;
        posY = (float)posy;
    }

    void Move(direction dir, float deltaTime) {
        switch (dir) {
            case Right: posX += speed * deltaTime; break;
            case Left:  posX -= speed * deltaTime; break;
            case Up:    posY -= speed * deltaTime; break;
            case Down:  posY += speed * deltaTime; break;
        }
    }

    Vector2 Get_position() const { return { posX, posY }; }

    void        Change_Name(std::string Name) { name = Name; }
    std::string Get_Name()       { return name; }
    std::string Get_Name() const { return name; }
};

void loadPlayer(player& p);
void savePlayer(const player& p);
void moovePlayer(player& p);
void DrawPlayer(player& p);