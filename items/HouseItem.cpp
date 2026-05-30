#include "building_api.h"

ITEM_BEGIN("HouseItem", Beton)

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

ITEM_END("HouseItem")
