#include "building_api.h"

ITEM_BEGIN("Beton", Beton)

    void onHand() {
        setBuildMode(true);
    }

    void onClick() {
        if (!isBuildMode()) return;
        if (leftClickPressed()) {
            Vector2 t = getTileMouse();
            placeBuilding("House", (int)t.x, (int)t.y);
        }
    }

    void onInventory() {
    }

ITEM_END("Beton")
