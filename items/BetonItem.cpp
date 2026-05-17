#include "item_registry.h"
#include "map.h"
#include "Mouse tile.h"
extern Map welt;

// ── Automatische Registrierung ────────────────────────────────────────────────
REGISTER_ITEM_FUNC_MIT_TASTE(BetonItemklick,    KEY_E);
REGISTER_ITEM_FUNC(BetonItemInInventar);
REGISTER_ITEM_FUNC(BetonItemInHand);

// ── Implementierungen ─────────────────────────────────────────────────────────
void BetonItemklick() {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 Mousepos = getTileMousePos(32);
        welt.setTile((int)Mousepos.x, (int)Mousepos.y, "concrete");
    }
}

void BetonItemInInventar() {
}

void BetonItemInHand() {
}