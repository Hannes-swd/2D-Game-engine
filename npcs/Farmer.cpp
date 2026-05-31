#include "npc_api.h"

// ═══════════════════════════════════════════════════════════════════════════════
//  Farmer.cpp  –  Beispiel-NPC: Bauer
//
//  JSON-Template: assets/json/npcs/farmer.json
//  Dialog-UI:     ui/FarmerDialog.cpp
// ═══════════════════════════════════════════════════════════════════════════════

NPC_BEGIN("farmer", Farmer)

    NPC_VAR(int,   level,    1)
    NPC_VAR(bool,  hostile,  false)

    void onSpawn() {
        NPC_SET(level,   1);
        NPC_SET(hostile, false);
        npcSetWander(npc, true);
    }

    void onUpdate() {
        // Jeden Frame – hier z.B. KI-Logik
    }

    void onProximity() {
        setPendingTooltip("[E] Sprechen");

        if (IsKeyPressed(KEY_E) || leftClickPressed()) {
            openUI("farmer_dialog");
        }
    }

    void onLeaveProximity() {
        // Spieler hat Radius verlassen
    }

NPC_END("farmer")
