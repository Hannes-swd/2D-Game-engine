#pragma once
#include "raylib.h"
#include "Cam.h"


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