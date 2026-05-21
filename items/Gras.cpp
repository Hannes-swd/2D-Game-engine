#include "item api.h"

ITEM_BEGIN("grasItem", grasItem)

    void onHand() {
        setBauModus(true);
    }

    void onKlick() {
        if (!isBauModus()) return;
        if (linksklick()) {
            Vector2 t = getTileMaus();
            setTile((int)t.x, (int)t.y, "gras");
        }
    }

    void onInventar() {
        
        // nichts
    }

ITEM_END("grasItem")