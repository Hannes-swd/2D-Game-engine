#include "player.h"
#include "Cam.h"
#include <fstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

void loadPlayer(player& p) {
    std::ifstream f("assets/json/player.json");
    if (f.is_open()) {
        json data;
        f >> data;
        p.Set_position(data.value("posX", 0), data.value("posY", 0));
        p.Change_Name(data.value("name", std::string("Spieler")));

        if (data.contains("inventory") && data["inventory"].is_array()) {
            for (const auto& item : data["inventory"]) {
                std::string id  = item.value("id",     "");
                int         anz = item.value("anzahl", 0);
                if (!id.empty()) {
                    p.addToInventory(id, anz);
                }
            }
        }
    } else {
        p.Set_position(0, 0);
        p.Change_Name("Spieler");
        // Fallback: Test-Item wenn JSON nicht gefunden
        p.addToInventory("grasItem", 10);
    }
}

void savePlayer(const player& p) {
    json data;
    Vector2 pos  = p.Get_position();
    data["posX"] = pos.x;
    data["posY"] = pos.y;
    data["name"] = p.Get_Name();

    json inventarArray = json::array();
    for (const auto& slot : p.getInventory()) {
        inventarArray.push_back({
            {"id",     slot.itemId},
            {"anzahl", slot.anzahl}
        });
    }
    data["inventory"] = inventarArray;

    std::ofstream f("assets/json/player.json");
    if (f.is_open())
        f << data.dump(4);
}

void moovePlayer(player& p) {
    float delta = GetFrameTime();

    if (IsKeyDown(KEY_W)) p.Move(Up,    delta);
    if (IsKeyDown(KEY_S)) p.Move(Down,  delta);
    if (IsKeyDown(KEY_A)) p.Move(Left,  delta);
    if (IsKeyDown(KEY_D)) p.Move(Right, delta);

    Vector2 pos     = p.Get_position();
    camera.target.x = pos.x;
    camera.target.y = pos.y;
}

void DrawPlayer(player& p) {
    Vector2 pos = p.Get_position();
    DrawCircle((int)pos.x, (int)pos.y, 10, ORANGE);
}

void DrawInventar(player& p) {
    const int slotSize = 50;
    const int padding  = 5;
    const int startX   = 10;
    const int startY   = 10;

    const auto& inventar = p.getInventory();
    int slotCount = (int)inventar.size();

    // DEBUG-Zeile: zeigt ob Items geladen wurden
    DrawText(TextFormat("Inv: %d items", slotCount), startX, startY + slotSize + 10, 14, RED);

    if (slotCount == 0) {
        // Leeres Panel
        DrawRectangle(startX - padding, startY - padding,
                      slotSize + padding * 2, slotSize + padding * 2,
                      Fade(BLACK, 0.5f));
        DrawText("leer", startX + 8, startY + 17, 16, WHITE);
        return;
    }

    // Hintergrund-Panel
    int panelW = slotCount * (slotSize + padding) + padding;
    int panelH = slotSize + padding * 2 + 4;
    DrawRectangle(startX - padding, startY - padding, panelW, panelH, Fade(BLACK, 0.5f));

    for (int i = 0; i < slotCount; i++) {
        int x = startX + i * (slotSize + padding);
        int y = startY;

        // Slot-Hintergrund
        DrawRectangle(x, y, slotSize, slotSize, LIGHTGRAY);
        DrawRectangleLines(x, y, slotSize, slotSize, DARKGRAY);

        const auto& slot = inventar[i];
        Item* item = g_itemManager.getItem(slot.itemId);

        if (item && item->textur.id != 0) {
            float scale = (float)(slotSize - 8) / (float)item->textur.width;
            DrawTextureEx(item->textur,
                          { (float)(x + 4), (float)(y + 4) },
                          0.0f, scale, WHITE);
        } else {
            // Fallback: Item-ID als Text
            DrawText(slot.itemId.c_str(), x + 3, y + 16, 9, DARKGRAY);
        }

        // Anzahl mit Schatten fuer bessere Lesbarkeit
        const char* anzStr = TextFormat("%d", slot.anzahl);
        int tx = x + slotSize - 22;
        int ty = y + slotSize - 18;
        DrawText(anzStr, tx + 1, ty + 1, 15, BLACK);  // Schatten
        DrawText(anzStr, tx,     ty,     15, WHITE);   // Text
    }
}

void player::addToInventory(const std::string& itemId, int anzahl) {
    for (auto& slot : inventar) {
        if (slot.itemId == itemId) {
            slot.anzahl += anzahl;
            return;
        }
    }
    inventar.push_back({ itemId, anzahl });
}

void player::removeFromInventory(const std::string& itemId, int anzahl) {
    for (auto it = inventar.begin(); it != inventar.end(); ++it) {
        if (it->itemId == itemId) {
            it->anzahl -= anzahl;
            if (it->anzahl <= 0) {
                inventar.erase(it);
            }
            return;
        }
    }
}

bool player::hasItem(const std::string& itemId) const {
    for (const auto& slot : inventar) {
        if (slot.itemId == itemId && slot.anzahl > 0)
            return true;
    }
    return false;
}