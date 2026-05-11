#include "player.h"
#include "Cam.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

void loadPlayer(player& p) {
    std::ifstream f("assets/json/player.json");
    if (f.is_open()) {
        std::cout << "[loadPlayer] player.json gefunden" << std::endl;
        json data;
        f >> data;
        p.Set_position(data.value("posX", 0), data.value("posY", 0));
        p.Change_Name(data.value("name", std::string("Spieler")));

        if (data.contains("inventory") && data["inventory"].is_array()) {
            std::cout << "[loadPlayer] inventory Array gefunden, groesse: "
                      << data["inventory"].size() << std::endl;
            for (const auto& item : data["inventory"]) {
                std::string id  = item.value("id",     "");
                int         anz = item.value("anzahl", 0);
                std::cout << "[loadPlayer] Item: " << id << " x" << anz << std::endl;
                if (!id.empty()) {
                    p.addToInventory(id, anz);
                }
            }
        } else {
            std::cout << "[loadPlayer] KEIN inventory Array in JSON!" << std::endl;
        }
    } else {
        std::cout << "[loadPlayer] player.json NICHT gefunden -> Fallback" << std::endl;
        p.Set_position(0, 0);
        p.Change_Name("Spieler");
        p.addToInventory("grasItem", 10);
    }

    std::cout << "[loadPlayer] Inventar nach laden: "
              << p.getInventory().size() << " slots" << std::endl;
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
    const int TOTAL_SLOTS = 10;
    const int slotSize    = 50;
    const int padding     = 5;
    const int margin      = 10;

    const auto& inventar = p.getInventory();
    int slotCount = (int)inventar.size();

    int panelW = TOTAL_SLOTS * (slotSize + padding) + padding;
    int panelH = slotSize + padding * 2;

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();
    int panelX  = (screenW - panelW) / 2;
    int panelY  = screenH - panelH - margin;

    DrawRectangle(panelX, panelY, panelW, panelH, Fade(BLACK, 0.6f));
    DrawRectangleLines(panelX, panelY, panelW, panelH, Fade(WHITE, 0.3f));

    // DEBUG: Slot-Anzahl über dem Panel
    DrawText(TextFormat("Slots: %d", slotCount), panelX, panelY - 20, 16, RED);

    for (int i = 0; i < TOTAL_SLOTS; i++) {
        int x = panelX + padding + i * (slotSize + padding);
        int y = panelY + padding;

        DrawRectangle(x, y, slotSize, slotSize, DARKGRAY);
        DrawRectangleLines(x, y, slotSize, slotSize, GRAY);

        if (i < slotCount) {
            const auto& slot = inventar[i];
            Item* item = g_itemManager.getItem(slot.itemId);

            if (item && item->textur.id != 0) {
                float scale = (float)(slotSize - 8) / (float)item->textur.width;
                DrawTextureEx(item->textur,
                              { (float)(x + 4), (float)(y + 4) },
                              0.0f, scale, WHITE);
            } else {
                // Zeigt Item-ID auch wenn Textur fehlt (gelb = Item da, Textur fehlt)
                DrawText(slot.itemId.c_str(), x + 3, y + 16, 9, YELLOW);
            }

            const char* anzStr = TextFormat("%d", slot.anzahl);
            int tx = x + slotSize - 22;
            int ty = y + slotSize - 18;
            DrawText(anzStr, tx + 1, ty + 1, 15, BLACK);
            DrawText(anzStr, tx,     ty,     15, WHITE);
        }
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