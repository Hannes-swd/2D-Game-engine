// Item zum Platzieren von Truhen.
// Zeigt: Object-System-Integration in ein Item.
#include "item api.h"
#include "object_api.h"

ITEM_BEGIN("truhenItem", truhenItem)

    void onHand() {
        setBuildMode(true);
    }

    void onClick() {
        if (!isBuildMode()) return;
        if (leftClickPressed()) {
            Vector2 t = getTileMouse();
            placeObject("truhe", (int)t.x, (int)t.y);
            g_player->removeFromInventory("truhenItem", 1);
        }
    }

    void onInventory() { }

ITEM_END("truhenItem")
