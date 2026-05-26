#include "building_api.h"

// ─────────────────────────────────────────────────────────────────────────────
//  Haus  –  Klick betritt die house_interior-Dimension
// ─────────────────────────────────────────────────────────────────────────────

BUILDING_BEGIN("House", House)

    void onHover() {
        setPendingTooltip("[Klick] Haus betreten");
    }

    void onClick() {
        enterInterior("house_interior");
    }

    void onPlace()  { }
    void onEnter()  { }
    void onLeave()  { }
    void onUpdate() { }

BUILDING_END("House")
