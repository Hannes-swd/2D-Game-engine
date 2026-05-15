#pragma once
#include "player.h"

// Zeichnet einen einzelnen Inventar-Slot (intern + extern verwendbar)
void zeichneSlot(int sx, int sy, int size, const InventarSlot* slot,
                 bool aktiv, bool isDragSource);

// Zeichnet die gesamte Inventar-UI (Hotbar + erweitertes Inventar)
void DrawInventar(player& p);