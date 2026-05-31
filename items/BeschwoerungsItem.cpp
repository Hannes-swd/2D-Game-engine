#include "npc_api.h"

ITEM_BEGIN("beschwItem", beschwItem)

    void onHand() {
        setBuildMode(true);
    }

    void onClick() {
        if (!isBuildMode()) return;
        if (!leftClickPressed()) return;

        Vector2 t = getTileMouse();
        float wx = t.x * 32.0f + 16.0f;
        float wy = t.y * 32.0f + 16.0f;

        PlacedNPC& npc = spawnNPC("beschworen", wx, wy);

        // Kleidung des Spielers übernehmen
        npc.bodyId  = g_player->getBodyId();
        npc.shirtId = g_player->getShirtId();
        npc.pantsId = g_player->getPantsId();
        npc.hairId  = g_player->getHairId();
    }

    void onInventory() { }

ITEM_END("beschwItem")
