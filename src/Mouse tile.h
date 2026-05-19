#pragma once
#include "raylib.h"
#include "Cam.h"
#include "player.h"
#include "Items.h"


inline Vector2 getTileMousePos(int tileSize = 32) {
    // Mausposition im Screen-Space → World-Space (berücksichtigt Zoom + Offset)
    Vector2 screenPos = GetMousePosition();
    Vector2 worldPos  = GetScreenToWorld2D(screenPos, camera);

    // World-Space → Tile-Koordinaten (floor damit negative Koordinaten stimmen)
    Vector2 tilePos;
    tilePos.x = floorf(worldPos.x / (float)tileSize);
    tilePos.y = floorf(worldPos.y / (float)tileSize);
    return tilePos;
}

// ── Baumodus-Raster und Platzierungs-Vorschau ─────────────────────────────────
// Zeichnet das Raster und den Platzierungs-Rahmen wenn der Baumodus aktiv ist.
// Muss im BeginMode2D / EndMode2D Block aufgerufen werden!
inline void DrawBauModusGrid(const player& p, int tileSize = 32,
                              int sichtRadius = 20)
{
    if (!p.isBauModus()) return;

    Vector2 screenPos = GetMousePosition();
    Vector2 worldPos  = GetScreenToWorld2D(screenPos, camera);

    // Tile unter der Maus (ganzzahlig, floor für negative Koordinaten)
    int mausTileX = (int)floorf(worldPos.x / (float)tileSize);
    int mausTileY = (int)floorf(worldPos.y / (float)tileSize);

    // Spielerposition als Mittelpunkt für das sichtbare Raster
    Vector2 spielerPos  = p.Get_position();
    int spielerTileX    = (int)floorf(spielerPos.x / (float)tileSize);
    int spielerTileY    = (int)floorf(spielerPos.y / (float)tileSize);

    // ── Rasterlinien im sichtbaren Bereich ────────────────────────────────────
    Color rasterFarbe = { 100, 140, 255, 45 };
    for (int dy = -sichtRadius; dy <= sichtRadius; dy++) {
        for (int dx = -sichtRadius; dx <= sichtRadius; dx++) {
            int tx = spielerTileX + dx;
            int ty = spielerTileY + dy;
            DrawRectangleLinesEx(
                { (float)(tx * tileSize), (float)(ty * tileSize),
                  (float)tileSize,        (float)tileSize },
                0.5f, rasterFarbe
            );
        }
    }

    // ── Platzierungs-Vorschau für das aktive Item ──────────────────────────────
    // TODO: Für größere platzierbare Items hier die Item-Größe (tileBreite x tileHoehe)
    //       aus den Item-Daten auslesen und den Rahmen entsprechend skalieren.
    //       Aktuell: Boden-Items sind immer 1×1 Tiles groß.

    // Aktives Item holen → Textur für Vorschau nutzen
    Item* hand = p.getHandItem();

    int previewW = 1; // TODO: item->tileBreite (aus JSON oder Item-Struct)
    int previewH = 1; // TODO: item->tileHoehe  (aus JSON oder Item-Struct)

    float px = (float)(mausTileX * tileSize);
    float py = (float)(mausTileY * tileSize);
    float pw = (float)(previewW  * tileSize);
    float ph = (float)(previewH  * tileSize);

    // Halb-transparente Textur-Vorschau (falls Item eine Textur hat)
    if (hand && hand->textur.id != 0) {
        float scaleX = pw / (float)hand->textur.width;
        float scaleY = ph / (float)hand->textur.height;
        float scale  = (scaleX < scaleY) ? scaleX : scaleY;
        DrawTextureEx(hand->textur, { px, py }, 0.0f, scale, { 255, 255, 255, 160 });
    } else {
        // Kein Item → nur blauer Füll-Block
        DrawRectangle((int)px, (int)py, (int)pw, (int)ph, { 80, 140, 255, 60 });
    }

    // Leuchtender Rahmen um den Ziel-Tile
    DrawRectangleLinesEx({ px, py, pw, ph }, 2.0f, { 80, 200, 255, 220 });

    // Koordinaten-Label (World-Space, oberhalb des Tile)
    // Nur anzeigen wenn der Tile in Fensternähe liegt (verhindert Overflow)
    Vector2 labelScreen = GetWorldToScreen2D({ px, py }, camera);
    if (labelScreen.x > 0 && labelScreen.x < GetScreenWidth() &&
        labelScreen.y > 0 && labelScreen.y < GetScreenHeight())
    {
        // Label im Screen-Space zeichnen → kurz aus BeginMode2D heraus
        // Hinweis: Das geht nicht direkt hier. Stattdessen: DrawText in World-Space
        // mit kleiner Schrift (funktioniert im BeginMode2D-Block).
        char koordinaten[32];
        snprintf(koordinaten, sizeof(koordinaten), "%d,%d", mausTileX, mausTileY);
        int fontSize = 6; // klein genug für Zoom-Level 4
        DrawText(koordinaten, (int)px + 2, (int)py + 2, fontSize, { 200, 230, 255, 200 });
    }
}

// ── Baumodus-HUD (Screen-Space, außerhalb BeginMode2D aufrufen!) ──────────────
inline void DrawBauModusHUD(const player& p) {
    if (!p.isBauModus()) return;

    const char* label = "[ BAUMODUS ]";
    int fontSize = 14;
    int tw = MeasureText(label, fontSize);
    int tx = (GetScreenWidth() - tw) / 2;
    int ty = 12;
    DrawText(label, tx + 1, ty + 1, fontSize, { 0, 0, 0, 180 });
    DrawText(label, tx,     ty,     fontSize, { 80, 200, 255, 255 });
}