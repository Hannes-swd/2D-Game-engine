#include "player.h"
#include "Cam.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

// ── Inventory-Methoden ────────────────────────────────────────────────────────

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
            if (it->anzahl <= 0) inventar.erase(it);
            return;
        }
    }
}

bool player::hasItem(const std::string& itemId) const {
    for (const auto& slot : inventar)
        if (slot.itemId == itemId) return true;
    return false;
}

// ── Speichern / Laden ─────────────────────────────────────────────────────────

void loadPlayer(player& p) {
    std::ifstream f("assets/json/player/player.json");
    if (!f.is_open()) f.open("assets/json/player.json");

    if (f.is_open()) {
        std::cout << "[loadPlayer] player.json gefunden" << std::endl;
        json data;
        f >> data;
        p.Set_position(data.value("posX", 0), data.value("posY", 0));
        p.Change_Name(data.value("name", std::string("Spieler")));

        if (data.contains("inventory") && data["inventory"].is_array()) {
            for (const auto& slot : data["inventory"]) {
                std::string id  = slot.value("id",     "");
                int         anz = slot.value("anzahl", 1);
                if (!id.empty())
                    p.addToInventory(id, anz);
            }
        }
    } else {
        std::cout << "[loadPlayer] player.json NICHT gefunden -> Fallback" << std::endl;
        p.Set_position(0, 0);
        p.Change_Name("Spieler");
    }

    std::cout << "[loadPlayer] Inventar: " << p.getInventory().size() << " slots" << std::endl;
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

    std::ofstream f("assets/json/player/player.json");
    if (!f.is_open()) f.open("assets/json/player.json");
    if (f.is_open())
        f << data.dump(4);
}

// ── Bewegung ──────────────────────────────────────────────────────────────────

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

// ── Zeichnen ──────────────────────────────────────────────────────────────────

void DrawPlayer(player& p) {
    Vector2 pos = p.Get_position();
    DrawCircle((int)pos.x, (int)pos.y, 10, ORANGE);
}

// ── Inventar-UI (Hotbar unten mittig, 10 Slots) ───────────────────────────────

void DrawInventar(player& p) {
    const int SLOT_SIZE    = 48;
    const int SLOT_PADDING = 6;
    const int SLOTS        = 10;
    const int BAR_W        = SLOTS * (SLOT_SIZE + SLOT_PADDING) - SLOT_PADDING;
    const int BAR_H        = SLOT_SIZE + 16;
    const int MARGIN_BOT   = 16;

    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    int barX = (sw - BAR_W) / 2 - 8;
    int barY = sh - BAR_H - MARGIN_BOT;

    // Hintergrund-Panel
    DrawRectangleRounded(
        { (float)barX, (float)barY, (float)(BAR_W + 16), (float)BAR_H },
        0.2f, 8, { 20, 20, 20, 200 }
    );
    DrawRectangleRoundedLines(
        { (float)barX, (float)barY, (float)(BAR_W + 16), (float)BAR_H },
        0.2f, 8, { 180, 180, 180, 220 }
    );

    const auto& inv = p.getInventory();

    for (int i = 0; i < SLOTS; i++) {
        int sx = barX + 8 + i * (SLOT_SIZE + SLOT_PADDING);
        int sy = barY + 8;

        bool belegt = (i < (int)inv.size());
        Color slotBg = belegt ? Color{60, 60, 60, 220} : Color{40, 40, 40, 180};

        DrawRectangleRounded(
            { (float)sx, (float)sy, (float)SLOT_SIZE, (float)SLOT_SIZE },
            0.15f, 6, slotBg
        );
        DrawRectangleRoundedLines(
            { (float)sx, (float)sy, (float)SLOT_SIZE, (float)SLOT_SIZE },
            0.15f, 6, { 120, 120, 120, 200 }
        );

        // Slot-Nummer
        DrawText(std::to_string(i + 1).c_str(), sx + 3, sy + 2, 8, { 150, 150, 150, 200 });

        if (!belegt) continue;

        const InventarSlot& slot = inv[i];

        // Item-Textur
        Item* item = g_itemManager.getItem(slot.itemId);
        if (item && item->textur.id != 0) {
            int   drawSize = SLOT_SIZE - 8;
            float scaleX   = (float)drawSize / item->textur.width;
            float scaleY   = (float)drawSize / item->textur.height;
            float scale    = (scaleX < scaleY) ? scaleX : scaleY;
            DrawTextureEx(item->textur, { (float)(sx + 4), (float)(sy + 4) }, 0.0f, scale, WHITE);
        } else {
            // Fallback: ID-Text
            int fontSize = 9;
            const char* label = slot.itemId.c_str();
            int tw = MeasureText(label, fontSize);
            DrawText(label, sx + (SLOT_SIZE - tw) / 2,
                     sy + SLOT_SIZE / 2 - fontSize / 2, fontSize, LIGHTGRAY);
        }

        // Anzahl
        if (slot.anzahl > 1) {
            std::string anzStr = std::to_string(slot.anzahl);
            int fontSize = 10;
            int tw = MeasureText(anzStr.c_str(), fontSize);
            DrawText(anzStr.c_str(), sx + SLOT_SIZE - tw - 3 + 1,
                     sy + SLOT_SIZE - fontSize - 2 + 1, fontSize, BLACK);
            DrawText(anzStr.c_str(), sx + SLOT_SIZE - tw - 3,
                     sy + SLOT_SIZE - fontSize - 2,     fontSize, WHITE);
        }
    }
}