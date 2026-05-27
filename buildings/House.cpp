#include "building_api.h"
#include "ui_api.h"

BUILDING_BEGIN("House", House)

    void onHover() {
        setPendingTooltip("[Klick] Haus betreten");
    }

    void onClick() {
        openUI("house_entrance");
    }

    void onPlace()  { }
    void onEnter()  { }
    void onLeave()  { }
    void onUpdate() { }

BUILDING_END("House")
