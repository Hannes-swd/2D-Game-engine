// Beispiel-Objekt: Truhe
// Platzierbar mit dem TruhenItem.
// Zeigt: Objekt-System + Sound + Partikel kombiniert.
#include "object_api.h"
#include "sound_api.h"
#include "particle_api.h"

OBJECT_BEGIN("truhe", Truhe)

    void onUpdate() {
        // Tooltip anzeigen wenn Spieler in der Nähe ist
        if (isPlayerNear(obj, 40.0f))
            setPendingTooltip("[E] Truhe oeffnen");
    }

    void onInteract() {
        // Goldenen Partikel-Burst
        SpawnParams p;
        p.count    = 20;
        p.color    = GOLD;
        p.speed    = 90.0f;
        p.lifetime = 0.9f;
        p.size     = 4.0f;
        p.gravity  = 250.0f;
        p.fade     = true;
        float wx = (obj.tileX + 0.5f) * 32.0f;
        float wy = (obj.tileY + 0.5f) * 32.0f;
        spawnParticles(wx, wy, p);

        // Öffnungs-Sound
        playSound("truhe_open");

        setPendingTooltip("Truhe geoeffnet!");
    }

    void onDestroy() {
        // Holzsplitter-Partikel beim Zerstören
        SpawnParams p;
        p.count    = 10;
        p.color    = BROWN;
        p.speed    = 60.0f;
        p.lifetime = 0.5f;
        p.size     = 3.0f;
        p.gravity  = 150.0f;
        p.fade     = true;
        float wx = (obj.tileX + 0.5f) * 32.0f;
        float wy = (obj.tileY + 0.5f) * 32.0f;
        spawnParticles(wx, wy, p);
    }

OBJECT_END("truhe")
