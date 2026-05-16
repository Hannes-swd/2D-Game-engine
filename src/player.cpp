#include "player.h"
#include "inventar.h"
#include "Cam.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

// ── Inventory-Methoden ────────────────────────────────────────────────────────

Item* player::getHandItem() const {
    if (aktuellerSlot < (int)inventar.size()) {
        const std::string& id = inventar[aktuellerSlot].itemId;
        if (!id.empty()) return g_itemManager.getItem(id);
    }
    return nullptr;
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

void player::swapSlots(int a, int b) {
    auto& inv = inventar;
    while ((int)inv.size() <= std::max(a, b))
        inv.push_back({"", 0});
    std::swap(inv[a], inv[b]);
    while (!inv.empty() && inv.back().itemId.empty())
        inv.pop_back();
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
        p.setAktuellerSlot(data.value("aktuellerSlot", 0));

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
    Vector2 pos        = p.Get_position();
    data["posX"]       = pos.x;
    data["posY"]       = pos.y;
    data["name"]       = p.Get_Name();
    data["aktuellerSlot"] = p.getAktuellerSlot();

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

    // Slot-Auswahl per Tastatur (1–0)
    for (int i = 0; i < 10; i++) {
        if (IsKeyPressed(KEY_ONE + i))
            p.setAktuellerSlot(i);
    }

    // Mausrad
    float rad = GetMouseWheelMove();
    if (rad != 0.0f) {
        int neuer = p.getAktuellerSlot() - (int)rad;
        if (neuer < 0) neuer = 9;
        if (neuer > 9) neuer = 0;
        p.setAktuellerSlot(neuer);
    }

    // Tab: Inventar öffnen/schließen
    if (IsKeyPressed(KEY_TAB)) p.toggleInventar();

    // ── onHand: jeden Frame für das aktive Item ────────────────────────────
    Item* hand = p.getHandItem();
    if (hand && hand->onHand) hand->onHand();

    // ── onInventar: jeden Frame für alle Items im Inventar ────────────────
    for (const auto& slot : p.getInventory()) {
        if (slot.itemId.empty()) continue;
        Item* item = g_itemManager.getItem(slot.itemId);
        if (item && item->onInventar) item->onInventar();
    }

    // ── onKlick: registrierte Taste ODER linke Maustaste → aktives Item ───
    if (hand && hand->onKlick) {
        bool ausloesen = false;

        // Wenn eine spezifische Taste registriert ist, nur diese prüfen
        if (hand->klickTaste >= 0) {
            ausloesen = IsKeyDown(hand->klickTaste);
        }

        // Linke Maustaste löst immer aus (für Platzieren/Interaktion)
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) ||
            IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            ausloesen = true;
        }

        if (ausloesen) {
            hand->onKlick();
        }
    }

    Vector2 pos     = p.Get_position();
    camera.target.x = pos.x;
    camera.target.y = pos.y;
}

// ── Zeichnen ──────────────────────────────────────────────────────────────────

void DrawPlayer(player& p) {
    Vector2 pos = p.Get_position();
    DrawCircle((int)pos.x, (int)pos.y, 10, ORANGE);

    Item* hand = p.getHandItem();
    if (hand && hand->textur.id != 0) {
        const int HAND_SIZE = 14;
        float scaleX = (float)HAND_SIZE / hand->textur.width;
        float scaleY = (float)HAND_SIZE / hand->textur.height;
        float scale  = (scaleX < scaleY) ? scaleX : scaleY;
        Vector2 drawPos = { pos.x + 10, pos.y - 18 };
        DrawTextureEx(hand->textur, drawPos, 0.0f, scale, WHITE);
    }
}