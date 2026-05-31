#include "ui_api.h"
#include "npc_api.h"

// ─────────────────────────────────────────────────────────────────────────────
//  FarmerDialog.cpp  –  Beispiel-Dialog für den Bauer-NPC
// ─────────────────────────────────────────────────────────────────────────────

UI_WINDOW_BEGIN("farmer_dialog")
    UI_TITLE("Bauer")
    UI_TEXT("Was kann ich für euch tun?")
    UI_SEPARATOR()
    UI_BUTTON("Tschüss", []() { closeCurrentUI(); })
UI_WINDOW_END("farmer_dialog")
