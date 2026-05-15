#include "player.h"
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

    // ── onKlick: jede Taste ODER jede Maustaste gehalten → aktives Item ───
    if (hand && hand->onKlick) {
        bool tasteGehalten = false;
        for (int k = 32; k <= 348; k++) {
            if (IsKeyDown(k)) { tasteGehalten = true; break; }
        }

        bool mausGehalten = IsMouseButtonDown(MOUSE_LEFT_BUTTON)  ||
                            IsMouseButtonDown(MOUSE_RIGHT_BUTTON) ||
                            IsMouseButtonDown(MOUSE_MIDDLE_BUTTON);

        if (tasteGehalten || mausGehalten) {
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

// ── Hotbar / Inventar-UI ──────────────────────────────────────────────────────

static void zeichneSlot(int sx, int sy, int size, const InventarSlot* slot,
                         bool aktiv, bool isDragSource)
{
    Color bg = isDragSource ? Color{80, 80, 40, 220}
             : aktiv        ? Color{80, 80, 30, 240}
             : (slot && !slot->itemId.empty()) ? Color{60, 60, 60, 220}
                                               : Color{40, 40, 40, 180};

    Color border = aktiv        ? Color{255, 215, 0, 255}
                 : isDragSource ? Color{255, 180, 0, 200}
                                : Color{120, 120, 120, 200};

    DrawRectangleRounded(
        { (float)sx, (float)sy, (float)size, (float)size },
        0.15f, 6, bg
    );
    DrawRectangleRoundedLines(
        { (float)sx, (float)sy, (float)size, (float)size },
        0.15f, 6, border
    );

    if (aktiv) {
        DrawRectangleRoundedLines(
            { (float)(sx+2), (float)(sy+2), (float)(size-4), (float)(size-4) },
            0.1f, 4, { 255, 215, 0, 100 }
        );
    }

    if (!slot || slot->itemId.empty()) return;

    Item* item = g_itemManager.getItem(slot->itemId);
    if (item && item->textur.id != 0) {
        int   drawSize = size - 8;
        float scaleX   = (float)drawSize / item->textur.width;
        float scaleY   = (float)drawSize / item->textur.height;
        float scale    = (scaleX < scaleY) ? scaleX : scaleY;
        DrawTextureEx(item->textur, { (float)(sx + 4), (float)(sy + 4) }, 0.0f, scale, WHITE);
    } else {
        int fontSize = 9;
        const char* label = slot->itemId.c_str();
        int tw = MeasureText(label, fontSize);
        DrawText(label, sx + (size - tw) / 2,
                 sy + size / 2 - fontSize / 2, fontSize, LIGHTGRAY);
    }

    if (slot->anzahl > 1) {
        std::string anzStr = std::to_string(slot->anzahl);
        int fontSize = 10;
        int tw = MeasureText(anzStr.c_str(), fontSize);
        DrawText(anzStr.c_str(), sx + size - tw - 3 + 1,
                 sy + size - fontSize - 2 + 1, fontSize, BLACK);
        DrawText(anzStr.c_str(), sx + size - tw - 3,
                 sy + size - fontSize - 2,     fontSize, WHITE);
    }
}

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

    auto& inv = p.getInventoryMut();

    DrawRectangleRounded(
        { (float)barX, (float)barY, (float)(BAR_W + 16), (float)BAR_H },
        0.2f, 8, { 20, 20, 20, 200 }
    );
    DrawRectangleRoundedLines(
        { (float)barX, (float)barY, (float)(BAR_W + 16), (float)BAR_H },
        0.2f, 8, { 180, 180, 180, 220 }
    );

    for (int i = 0; i < SLOTS; i++) {
        int sx = barX + 8 + i * (SLOT_SIZE + SLOT_PADDING);
        int sy = barY + 8;

        bool aktiv        = (i == p.getAktuellerSlot());
        bool isDragSource = (i == p.getDragSlot());

        const InventarSlot* slotPtr = (i < (int)inv.size()) ? &inv[i] : nullptr;
        zeichneSlot(sx, sy, SLOT_SIZE, slotPtr, aktiv, isDragSource);
    }

    // Tooltip
    Item* hand = p.getHandItem();
    if (hand) {
        const char* label = hand->name.c_str();
        int fontSize = 12;
        int tw = MeasureText(label, fontSize);
        int tx = (sw - tw) / 2;
        int ty = barY - fontSize - 8;
        DrawText(label, tx + 1, ty + 1, fontSize, BLACK);
        DrawText(label, tx,     ty,     fontSize, { 255, 230, 100, 255 });
    }

    // Drag & Drop
    Vector2 mouse = GetMousePosition();

    auto getSlotUnterMaus = [&]() -> int {
        for (int i = 0; i < SLOTS; i++) {
            int sx = barX + 8 + i * (SLOT_SIZE + SLOT_PADDING);
            int sy = barY + 8;
            if (mouse.x >= sx && mouse.x <= sx + SLOT_SIZE &&
                mouse.y >= sy && mouse.y <= sy + SLOT_SIZE)
                return i;
        }
        return -1;
    };

    int slotUnterMaus = getSlotUnterMaus();

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (slotUnterMaus >= 0) {
            p.setAktuellerSlot(slotUnterMaus);
            if (slotUnterMaus < (int)inv.size() && !inv[slotUnterMaus].itemId.empty()) {
                p.setDragSlot(slotUnterMaus);
            }
            Item* h = p.getHandItem();
            if (h && h->onHand) h->onHand();
        }
    }

    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        int drag = p.getDragSlot();
        if (drag >= 0 && slotUnterMaus >= 0 && slotUnterMaus != drag) {
            p.swapSlots(drag, slotUnterMaus);
        }
        p.setDragSlot(-1);
    }

    // Drag-Vorschau
    if (p.getDragSlot() >= 0 && p.getDragSlot() < (int)inv.size()) {
        const InventarSlot& dragged = inv[p.getDragSlot()];
        Item* dItem = g_itemManager.getItem(dragged.itemId);
        if (dItem && dItem->textur.id != 0) {
            int   drawSize = SLOT_SIZE - 8;
            float scaleX   = (float)drawSize / dItem->textur.width;
            float scaleY   = (float)drawSize / dItem->textur.height;
            float scale    = (scaleX < scaleY) ? scaleX : scaleY;
            DrawTextureEx(dItem->textur,
                { mouse.x - drawSize * scale * 0.5f, mouse.y - drawSize * scale * 0.5f },
                0.0f, scale, { 255, 255, 255, 180 });
        }
    }

    // Rechtsklick auf Slot
    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) && slotUnterMaus >= 0) {
        p.setAktuellerSlot(slotUnterMaus);
        Item* clicked = p.getHandItem();
        if (clicked && clicked->onKlick) clicked->onKlick();
    }

    // Erweitertes Inventar (TAB)
    if (p.isInventarOffen()) {
        const int COLS    = 5;
        const int ROWS    = 4;
        const int WIN_W   = COLS * (SLOT_SIZE + SLOT_PADDING) - SLOT_PADDING + 24;
        const int WIN_H   = ROWS * (SLOT_SIZE + SLOT_PADDING) - SLOT_PADDING + 40;
        int winX = (sw - WIN_W) / 2;
        int winY = (sh - WIN_H) / 2 - 40;

        DrawRectangleRounded(
            { (float)winX, (float)winY, (float)WIN_W, (float)WIN_H },
            0.1f, 8, { 15, 15, 15, 230 }
        );
        DrawRectangleRoundedLines(
            { (float)winX, (float)winY, (float)WIN_W, (float)WIN_H },
            0.1f, 8, { 200, 200, 200, 200 }
        );

        for (int r = 0; r < ROWS; r++) {
            for (int c = 0; c < COLS; c++) {
                int idx = 10 + r * COLS + c;
                int sx  = winX + 12 + c * (SLOT_SIZE + SLOT_PADDING);
                int sy  = winY + 28 + r * (SLOT_SIZE + SLOT_PADDING);
                const InventarSlot* sp = (idx < (int)inv.size()) ? &inv[idx] : nullptr;
                zeichneSlot(sx, sy, SLOT_SIZE, sp, false, idx == p.getDragSlot());

                if (mouse.x >= sx && mouse.x <= sx + SLOT_SIZE &&
                    mouse.y >= sy && mouse.y <= sy + SLOT_SIZE)
                {
                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        if (sp && !sp->itemId.empty()) p.setDragSlot(idx);
                    }
                    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                        int drag = p.getDragSlot();
                        if (drag >= 0 && drag != idx) p.swapSlots(drag, idx);
                        p.setDragSlot(-1);
                    }
                }
            }
        }
    }
}