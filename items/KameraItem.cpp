// Kamera-Demo-Item: Wechselt per Klick zwischen drei Kamera-Modi.
// Zeigt: setCameraFollowPlayer, setCameraFixed, cameraMoveToSmooth, cameraShake.
#include "item api.h"
#include "cam_api.h"

namespace _kameraitem_state {
    static int mode = 0; // 0=follow 1=fixed 2=cinematic
}

ITEM_BEGIN("kameraItem", kameraItem)

    void onHand() {
        // Modus-Anzeige als Tooltip
        using namespace _kameraitem_state;
        if (mode == 0) setPendingTooltip("[Klick] Modus: Folgt Spieler  → Einfrieren");
        if (mode == 1) setPendingTooltip("[Klick] Modus: Eingefroren    → Cinematic");
        if (mode == 2) setPendingTooltip("[Klick] Modus: Cinematic      → Folgt Spieler");
    }

    void onClick() {
        using namespace _kameraitem_state;
        if (!leftClickPressed()) return;

        if (mode == 0) {
            // Kamera einfrieren wo sie gerade ist
            setCameraFixed(camera.target.x, camera.target.y);
            mode = 1;

        } else if (mode == 1) {
            // Cinematic: gleitet 3 Sekunden zu Spieler-Position + Offset
            Vector2 pos = g_player->Get_position();
            cameraMoveToSmooth(pos.x + 80.0f, pos.y - 60.0f, 3.0f);
            mode = 2;

        } else {
            // Zurück zum Spieler folgen + Shake-Demo
            setCameraFollowPlayer();
            cameraShake(6.0f, 0.5f);
            mode = 0;
        }
    }

    void onInventory() { }

ITEM_END("kameraItem")
