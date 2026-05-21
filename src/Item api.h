#pragma once
// ═══════════════════════════════════════════════════════════════════════════════
//  item_api.h  –  Das einzige Header das ein Modder braucht
//
//  Verwendung in items/gras.cpp:
//
//      #include "item_api.h"
//      ITEM_BEGIN("gras", gras)
//
//          void onHand() {
//              setBauModus(true);
//          }
//
//          void onKlick() {
//              Vector2 t = getTileMaus();
//              setTile(t.x, t.y, "gras");
//          }
//
//          void onInventar() {
//              // leer lassen wenn nicht gebraucht
//          }
//
//      ITEM_END("gras")
//
//  WICHTIG: Das zweite Argument von ITEM_BEGIN muss ein gültiger C++-Bezeichner
//  sein (kein String, keine Leerzeichen, keine Bindestriche).
//  Beispiele:
//      ITEM_BEGIN("gras",       gras)
//      ITEM_BEGIN("Beton",      Beton)
//      ITEM_BEGIN("iron_sword", iron_sword)
//
//  Das war's. Kein extern, kein REGISTER_..., kein g_spieler.
// ═══════════════════════════════════════════════════════════════════════════════

#include "Items.h"
#include "map.h"
#include "player.h"
#include "Mouse tile.h"
#include "raylib.h"
#include <string>
#include <functional>

// ── Globale Variablen (einmalig in main.cpp definiert) ───────────────────────
extern Map     welt;
extern player* g_spieler;

// ── Hilfsfunktionen für Modder ───────────────────────────────────────────────

// Setzt einen Tile auf der Karte
inline void setTile(int x, int y, const std::string& typ) {
    welt.setTile(x, y, typ);
}

// Tile-Position unter der Maus
inline Vector2 getTileMaus(int tileGroesse = 32) {
    return getTileMousePos(tileGroesse);
}

// Baumodus (Raster + Vorschau)
inline void  setBauModus(bool aktiv)  { if (g_spieler) g_spieler->setBauModus(aktiv); }
inline bool  isBauModus()             { return g_spieler && g_spieler->isBauModus(); }

// Maus-Checks
inline bool  isMausAufUI()            { return g_spieler && g_spieler->isMausAufUI(); }
inline bool  linksklick()             { return IsMouseButtonDown(MOUSE_BUTTON_LEFT);  }
inline bool  rechtsklick()            { return IsMouseButtonDown(MOUSE_BUTTON_RIGHT); }
inline bool  linksklickPressed()      { return IsMouseButtonPressed(MOUSE_BUTTON_LEFT);  }

// ── Interner Auto-Registrar ───────────────────────────────────────────────────
struct _ItemAutoRegistrar {
    _ItemAutoRegistrar(const char* itemId,
                       std::function<void()> fnHand,
                       std::function<void()> fnKlick,
                       std::function<void()> fnInventar)
    {
        std::string id(itemId);

        // Wrapper für onKlick: automatisch UI-Check eingebaut
        auto klickWrapper = [fnKlick]() {
            if (!g_spieler || g_spieler->isMausAufUI()) return;
            fnKlick();
        };

        // In der Registry speichern (für JSON-Binding)
        getItemManager().registriereFunktion(id + "_onHand",     fnHand);
        getItemManager().registriereFunktion(id + "_onKlick",    klickWrapper);
        getItemManager().registriereFunktion(id + "_onInventar", fnInventar);

        // Sofort ans Item binden falls es bereits aus JSON geladen wurde
        Item* item = getItemManager().getItem(id);
        if (item) {
            item->onHand     = fnHand;
            item->onKlick    = klickWrapper;
            item->onInventar = fnInventar;
        }
    }
};

// ── ITEM_BEGIN / ITEM_END ─────────────────────────────────────────────────────
//
//  FIX LNK2005: Jedes Item bekommt einen eigenen Namespace (_item_impl_<tag>),
//  damit onHand/onKlick/onInventar nicht über Translation Units hinweg
//  kollidieren.
//
//  tag  = gültiger C++-Bezeichner, z.B. gras, Beton, iron_sword
//  id   = Item-String, z.B. "gras", "Beton", "iron_sword"
//
//  Alle drei Funktionen müssen definiert werden (leerer Body ist OK).

#define ITEM_BEGIN(itemId, tag)                                 \
    namespace _item_impl_##tag {                                \
        static const char* _ITEM_ID = itemId;

#define ITEM_END(itemId)                                        \
        static _ItemAutoRegistrar _reg(                         \
            itemId, onHand, onKlick, onInventar                 \
        );                                                      \
    } // end namespace