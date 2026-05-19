#include "item_registry.h"
#include "map.h"
#include "Mouse tile.h"
#include "player.h"

extern Map    welt;
extern player* g_spieler;

REGISTER_ITEM_FUNC_MIT_TASTE(grasItemklick,    KEY_E);
REGISTER_ITEM_FUNC(grasItemInInventar);
REGISTER_ITEM_FUNC(grasItemInHand);

void grasItemInHand() {
    if (g_spieler && !g_spieler->isBauModus())
        g_spieler->setBauModus(true);
}

void grasItemklick() {
    if (!g_spieler || !g_spieler->isBauModus()) return;
    if (g_spieler->isMausAufUI()) return; 

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        Vector2 mausPos = getTileMousePos(32);
        welt.setTile((int)mausPos.x, (int)mausPos.y, "gras");
    }
}

void grasItemInInventar() {

}