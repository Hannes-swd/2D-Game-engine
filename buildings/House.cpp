#include <iostream>
#include "building_api.h"

BUILDING_BEGIN("House", House)

    void onHover() {
        // Standard: Engine zeigt automatisch Outline + "[Klick] Betreten"
        
    }

    void onClick() {
        // Standard: Engine wechselt automatisch zur Innenwelt (wenn interior_size gesetzt)
    }

    void onPlace() {
        std::cout << "auf haus";

    }

    void onEnter() {
    }

    void onLeave() {
    }

    void onUpdate() {
    }

BUILDING_END("House")
