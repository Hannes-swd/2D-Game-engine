#include "item api.h"
#include "items.h"

ITEM_BEGIN("grasItem", grasItem)

    void onHand() {
        setBauModus(true);
    }

    void onKlick() {
        if (!isBauModus()) return;
        if (linksklickPressed()) {
            Vector2 t = getTileMaus();
            setTile((int)t.x, (int)t.y, "gras");
            g_spieler->removeFromInventory("grasItem", 1);
        }
    }

    void onInventar() {
        
        // nichts
    }

ITEM_END("grasItem")