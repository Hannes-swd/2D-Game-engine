#include "hud_api.h"

// ── Beispiel-Button ───────────────────────────────────────────────────────────
HUD_BUTTON_BEGIN(example_button)
    HUD_POS(10, 10)
    HUD_SIZE(80, 40)
    HUD_TEXT("Klick mich")

    HUD_ON_PRESS([]{
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
    })

    HUD_ON_HOLD([]{
        // jeden Frame solange gehalten
    })
HUD_BUTTON_END
