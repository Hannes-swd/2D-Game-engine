#include "player.h"
#include "inventory.h"
#include "Cam.h"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

// Asset path helper (defined in main.cpp)
extern std::string assetPath(const std::string& relativ);

// ── Inventory-Methoden ────────────────────────────────────────────────────────

Item* player::getHandItem() const {
    if (currentSlot < (int)inventory.size()) {
        const std::string& id = inventory[currentSlot].itemId;
        if (!id.empty()) return g_itemManager.getItem(id);
    }
    return nullptr;
}

void player::addToInventory(const std::string& itemId, int amount) {
    for (auto& slot : inventory) {
        if (slot.itemId == itemId) {
            slot.amount += amount;
            return;
        }
    }
    inventory.push_back({ itemId, amount });
}

void player::removeFromInventory(const std::string& itemId, int amount) {
    for (auto it = inventory.begin(); it != inventory.end(); ++it) {
        if (it->itemId == itemId) {
            it->amount -= amount;
            if (it->amount <= 0) inventory.erase(it);
            return;
        }
    }
}

bool player::hasItem(const std::string& itemId) const {
    for (const auto& slot : inventory)
        if (slot.itemId == itemId) return true;
    return false;
}

void player::swapSlots(int a, int b) {
    auto& inv = inventory;
    while ((int)inv.size() <= std::max(a, b))
        inv.push_back({"", 0});
    std::swap(inv[a], inv[b]);
    while (!inv.empty() && inv.back().itemId.empty())
        inv.pop_back();
}

// ── Speichern / Laden ─────────────────────────────────────────────────────────

void loadPlayer(player& p) {
    std::string path = assetPath("json/player/player.json");
    std::ifstream f(path);

    if (f.is_open()) {
        std::cout << "[loadPlayer] Lade: " << path << std::endl;
        try {
            json data;
            f >> data;
            p.Set_position(data.value("posX", 0), data.value("posY", 0));
            p.Change_Name(data.value("name", std::string("Spieler")));
            p.setCurrentSlot(data.value("currentSlot", 0));
            p.setBuildMode(data.value("buildMode", false));

            if (data.contains("inventory") && data["inventory"].is_array()) {
                for (const auto& slot : data["inventory"]) {
                    std::string id  = slot.value("id",     "");
                    int         anz = slot.value("amount", 1);
                    if (!id.empty())
                        p.addToInventory(id, anz);
                    std::cout << "[loadPlayer] Slot: " << id << " x" << anz << std::endl;
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "[loadPlayer] JSON Fehler: " << e.what() << " – Fallback auf Standardwerte." << std::endl;
            p.Set_position(0, 0);
            p.Change_Name("Spieler");
        }
    } else {
        std::cout << "[loadPlayer] Nicht gefunden -> Fallback (" << path << ")" << std::endl;
        p.Set_position(0, 0);
        p.Change_Name("Spieler");
    }

    std::cout << "[loadPlayer] Inventar: " << p.getInventory().size() << " slots" << std::endl;
}

void savePlayer(const player& p) {
    json data;
    Vector2 pos           = p.Get_position();
    data["posX"]          = (int)pos.x;
    data["posY"]          = (int)pos.y;
    data["name"]          = p.Get_Name();
    data["currentSlot"] = p.getCurrentSlot();
    data["buildMode"]      = p.isBuildMode();

    json inventoryArray = json::array();
    for (const auto& slot : p.getInventory()) {
        inventoryArray.push_back({
            {"id",     slot.itemId},
            {"amount", slot.amount}
        });
    }
    data["inventory"] = inventoryArray;

    namespace fs = std::filesystem;
    std::string path = assetPath("json/player/player.json");
    fs::create_directories(fs::path(path).parent_path());

    std::ofstream f(path);
    if (f.is_open()) {
        f << data.dump(4);
        std::cout << "[savePlayer] Gespeichert: " << path << std::endl;
    } else {
        std::cerr << "[savePlayer] FEHLER: Kann nicht schreiben: " << path << std::endl;
    }
}

// ── Bewegung ──────────────────────────────────────────────────────────────────

void updatePlayer(player& p) {
    float delta = GetFrameTime();

    if (IsKeyDown(KEY_W)) p.Move(Up,    delta);
    if (IsKeyDown(KEY_S)) p.Move(Down,  delta);
    if (IsKeyDown(KEY_A)) p.Move(Left,  delta);
    if (IsKeyDown(KEY_D)) p.Move(Right, delta);

    // Slot-Auswahl per Tastatur (1-0)
    for (int i = 0; i < 10; i++) {
        if (IsKeyPressed(KEY_ONE + i))
            p.setCurrentSlot(i);
    }

    // Mausrad
    float rad = GetMouseWheelMove();
    if (rad != 0.0f) {
        int newSlot = p.getCurrentSlot() - (int)rad;
        if (newSlot < 0) newSlot = 9;
        if (newSlot > 9) newSlot = 0;
        p.setCurrentSlot(newSlot);
    }

    if (IsKeyPressed(KEY_TAB)) p.toggleInventory();
    if (IsKeyPressed(KEY_B))   p.toggleBuildMode();

    // onHand: jeden Frame fuer das aktive Item
    Item* hand = p.getHandItem();
    if (hand && hand->onHand) hand->onHand();

    // onInventory: jeden Frame fuer alle Items im Inventar
    for (const auto& slot : p.getInventory()) {
        if (slot.itemId.empty()) continue;
        Item* item = g_itemManager.getItem(slot.itemId);
        if (item && item->onInventory) item->onInventory();
    }

    // onClick: kein Ausloesen wenn Maus ueber UI liegt
    if (hand && hand->onClick && !p.IsMouseOnUi()) {
        bool ausloesen = false;

        if (hand->clickKey >= 0)
            ausloesen = IsKeyDown(hand->clickKey);

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) ||
            IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            ausloesen = true;

        if (ausloesen)
            hand->onClick();
    }

    Vector2 pos     = p.Get_position();
    camera.target.x = pos.x;
    camera.target.y = pos.y;
}

// ── Zeichnen ──────────────────────────────────────────────────────────────────

void drawPlayer(player& p) {
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