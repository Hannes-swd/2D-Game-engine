// Musik-Demo-Item: Startet und stoppt Hintergrundmusik.
// Zeigt das Sound-System (Musik-Teil).
// Musik-Dateien in assets/music/ ablegen – Dateiname = ID.
#include "item api.h"
#include "sound_api.h"

namespace _musikitem_state {
    static bool playing = false;
}

ITEM_BEGIN("musikItem", musikItem)

    void onHand() {
        using namespace _musikitem_state;
        if (playing)
            setPendingTooltip("[Klick] Musik stoppen");
        else
            setPendingTooltip("[Klick] Musik starten  (placeholder)");
    }

    void onClick() {
        using namespace _musikitem_state;
        if (!leftClickPressed()) return;

        if (!playing) {
            playMusic("placeholder");
            playing = true;
        } else {
            stopMusic();
            playing = false;
        }
    }

    void onInventory() { }

ITEM_END("musikItem")
