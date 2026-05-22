#include "item api.h"

ITEM_BEGIN("grasItem", grasItem)

    void onHand() {
        setBuildMode(true);
    }

    void onClick() {
        if (!isBuildMode()) return;
        if (leftClickPressed()) {
            Vector2 t = getTileMouse();
            setTile((int)t.x, (int)t.y, "gras");
            g_player->removeFromInventory("grasItem", 1);
        }
    }

    void onInventory() {
        // nichts
    }

ITEM_END("grasItem")