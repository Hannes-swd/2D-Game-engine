#include "dimension_api.h"

// ─────────────────────────────────────────────────────────────────────────────
//  Haus-Innenraum: 4x10 Tiles, dunkle Bodenfarbe, Betonboden
//  Betreten: Klick aufs Haus (Building onClick)
//  Verlassen: Taste E
// ─────────────────────────────────────────────────────────────────────────────

DIMENSION_BEGIN("house_interior", house_interior)

    void onEnter() {
        // Wird aufgerufen sobald der Spieler den Innenraum betritt.
    }

    void onLeave() {
        // Wird aufgerufen wenn der Spieler den Innenraum verlässt.
    }

    void onUpdate() {
        // Jedes Frame solange der Spieler im Innenraum ist.
        // E drücken → zurück in die Hauptwelt
        if (IsKeyPressed(KEY_E)) {
            leaveCurrentDimension();
        }
    }

    void onDraw() {
        // Optionale Extra-Zeichnung innerhalb BeginMode2D.
    }

DIMENSION_END("house_interior")
