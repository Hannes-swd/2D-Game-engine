#pragma once
#include <string>
#include <vector>
#include "raylib.h"
#include "Items.h"

struct InventorySlot {
    std::string itemId;
    int amount;
};

enum direction { Up, Down, Right, Left };

class player {
private:
    float posX  = 0.0f;
    float posY  = 0.0f;
    float speed = 100.0f;
    std::vector<InventorySlot> inventory;
    std::string name;

    int  currentSlot  = 0;   // aktuell ausgewählter Hotbar-Slot (0–9)
    int  dragSlot       = -1;  // Slot der gerade gezogen wird (-1 = keiner)
    bool inventoryOpen  = false; // erweitertes Inventar offen?
    bool mouseOnUI      = false; // true when mouse is over hotbar/inventory UI
    bool buildMode       = false; // Baumodus: zeigt Raster + Platzierungs-Vorschau

public:
    void Set_position(int posx, int posy) {
        posX = (float)posx;
        posY = (float)posy;
    }

    void setPositionF(float x, float y) { posX = x; posY = y; }

    void Move(direction dir, float deltaTime) {
        switch (dir) {
            case Right: posX += speed * deltaTime; break;
            case Left:  posX -= speed * deltaTime; break;
            case Up:    posY -= speed * deltaTime; break;
            case Down:  posY += speed * deltaTime; break;
        }
    }

    float getSpeed() const { return speed; }
    Vector2 Get_position() const { return { posX, posY }; }
    void Change_Name(std::string Name) { name = Name; }
    std::string Get_Name() const { return name; }

    // Slot-Steuerung
    int  getCurrentSlot() const         { return currentSlot; }
    void setCurrentSlot(int s)          {
        int neu = (s < 0) ? 0 : (s > 9 ? 9 : s);
        currentSlot = neu;
        // buildMode wird NICHT hier zurückgesetzt.
        // onHand() des neuen Items setzt ihn korrekt (z.B. Gras → true, Beton → false).
        // Das geschieht automatisch jeden Frame in updatePlayer().
    }
    int  getDragSlot() const              { return dragSlot; }
    void setDragSlot(int s)               { dragSlot = s; }
    bool isInventoryOpen() const          { return inventoryOpen; }
    void toggleInventory()                 { inventoryOpen = !inventoryOpen; }
    bool IsMouseOnUi() const              { return mouseOnUI; }
    void setMouseOnUI(bool b)             { mouseOnUI = b; }
    bool isBuildMode() const               { return buildMode; }
    void setBuildMode(bool b)              { buildMode = b; }
    void toggleBuildMode()                 { buildMode = !buildMode; }

    // Aktives Item (in der Hand)
    Item* getHandItem() const;

    // Inventar
    void addToInventory(const std::string& itemId, int amount);
    void removeFromInventory(const std::string& itemId, int amount);
    bool hasItem(const std::string& itemId) const;
    const std::vector<InventorySlot>& getInventory() const { return inventory; }
    std::vector<InventorySlot>&       getInventoryMut()    { return inventory; }

    // Slots tauschen
    void swapSlots(int a, int b);
};

void loadPlayer(player& p);
void savePlayer(const player& p);
void updatePlayer(player& p);
void drawPlayer(player& p);