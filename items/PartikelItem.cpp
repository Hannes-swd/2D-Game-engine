// Partikel-Demo-Item: zeigt sowohl Farb-Partikel als auch Textur-Partikel.
// Linksklick  = Regenbogen-Burst (Farb-Kreise)   → läuft über onClick()
// Rechtsklick = Gras-Textur-Partikel              → läuft über onHand()
//   Grund: onClick() wird vom Engine nur für Linksklick ausgelöst.
//   Für alle anderen Tasten/Maustasten einfach in onHand() prüfen.
#include "item api.h"
#include "particle_api.h"
#include "sound_api.h"

ITEM_BEGIN("partikelItem", partikelItem)

    void onHand() {
        setBuildMode(true);
        setPendingTooltip("[Links] Farb-Burst   [Rechts] Textur-Burst");

        // Rechtsklick hier prüfen – onHand läuft jeden Frame
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && !IsMouseOnUi()) {
            Vector2 t = getTileMouse();
            float wx  = (t.x + 0.5f) * 32.0f;
            float wy  = (t.y + 0.5f) * 32.0f;

            SpawnParams p;
            p.count         = 12;
            p.color         = WHITE;
            p.speed         = 80.0f;
            p.lifetime      = 1.2f;
            p.size          = 8.0f;
            p.gravity       = 150.0f;
            p.fade          = true;
            p.texture       = getItemTexture("grasItem");
            p.rotationSpeed = 180.0f;

            spawnParticles(wx, wy, p);
            playSound("partikel_pop");
        }
    }

    void onClick() {
        // Linksklick – wird automatisch vom Engine ausgelöst
        Vector2 t = getTileMouse();
        float wx  = (t.x + 0.5f) * 32.0f;
        float wy  = (t.y + 0.5f) * 32.0f;

        if (leftClickPressed()) {
            SpawnParams p;
            p.count    = 15;
            p.speed    = 130.0f;
            p.lifetime = 1.0f;
            p.size     = 5.0f;
            p.gravity  = 200.0f;
            p.fade     = true;

            p.color = RED;     spawnParticles(wx, wy, p);
            p.color = YELLOW;  spawnParticles(wx, wy, p);
            p.color = SKYBLUE; spawnParticles(wx, wy, p);

            playSound("partikel_pop");
        }
    }

    void onInventory() { }

ITEM_END("partikelItem")
