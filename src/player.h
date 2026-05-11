#pragma once
#include <string>
#include <vector>
#include "raylib.h"
#include "Items.h"

struct InventarSlot {
    std::string itemId;
    int anzahl;
};

enum direction { Up, Down, Right, Left };

class player {
private:
    float posX  = 0.0f;
    float posY  = 0.0f;
    float speed = 100.0f;
    std::vector<InventarSlot> inventar;
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
    void Change_Name(std::string Name) { name = Name; }
    std::string Get_Name() const { return name; }

    void addToInventory(const std::string& itemId, int anzahl);
    void removeFromInventory(const std::string& itemId, int anzahl);
    bool hasItem(const std::string& itemId) const;
    const std::vector<InventarSlot>& getInventory() const { return inventar; }
};

void loadPlayer(player& p);
void savePlayer(const player& p);
void moovePlayer(player& p);
void DrawPlayer(player& p);
void DrawInventar(player& p);