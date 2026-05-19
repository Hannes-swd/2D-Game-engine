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

    int  aktuellerSlot  = 0;   // aktuell ausgewählter Hotbar-Slot (0–9)
    int  dragSlot       = -1;  // Slot der gerade gezogen wird (-1 = keiner)
    bool inventarOffen  = false; // erweitertes Inventar offen?
    bool mausAufUI      = false; // true wenn Maus über Hotbar/Inventar-UI liegt
    bool bauModus       = false; // Baumodus: zeigt Raster + Platzierungs-Vorschau

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

    // Slot-Steuerung
    int  getAktuellerSlot() const         { return aktuellerSlot; }
    void setAktuellerSlot(int s)          {
        int neu = (s < 0) ? 0 : (s > 9 ? 9 : s);
        if (neu != aktuellerSlot) bauModus = false; // Baumodus aus beim Slot-Wechsel
        aktuellerSlot = neu;
    }
    int  getDragSlot() const              { return dragSlot; }
    void setDragSlot(int s)               { dragSlot = s; }
    bool isInventarOffen() const          { return inventarOffen; }
    void toggleInventar()                 { inventarOffen = !inventarOffen; }
    bool isMausAufUI() const              { return mausAufUI; }
    void setMausAufUI(bool b)             { mausAufUI = b; }
    bool isBauModus() const               { return bauModus; }
    void setBauModus(bool b)              { bauModus = b; }
    void toggleBauModus()                 { bauModus = !bauModus; }

    // Aktives Item (in der Hand)
    Item* getHandItem() const;

    // Inventar
    void addToInventory(const std::string& itemId, int anzahl);
    void removeFromInventory(const std::string& itemId, int anzahl);
    bool hasItem(const std::string& itemId) const;
    const std::vector<InventarSlot>& getInventory() const { return inventar; }
    std::vector<InventarSlot>&       getInventoryMut()    { return inventar; }

    // Slots tauschen
    void swapSlots(int a, int b);
};

void loadPlayer(player& p);
void savePlayer(const player& p);
void moovePlayer(player& p);
void DrawPlayer(player& p);