#pragma once
#include "Items.h"

// ── Automatische Registrierung per Static-Initializer ────────────────────────
// Wird beim Programmstart ausgeführt, BEVOR main() läuft.
// Nutzt getItemManager() Singleton → kein Static-Init-Order-Problem.

struct AutoRegister {
    AutoRegister(const char* name, std::function<void()> fn) {
        getItemManager().registriereFunktion(name, fn);
    }
    AutoRegister(const char* name, std::function<void()> fn, int taste) {
        getItemManager().registriereFunktionMitTaste(name, fn, taste);
    }
};

// Registriert eine Funktion OHNE Taste (onHand, onInventar, onUpdate)
// Verwendung oben in der .cpp des Items:
//   REGISTER_ITEM_FUNC(grasItemInHand)
#define REGISTER_ITEM_FUNC(funcName) \
    void funcName(); \
    static AutoRegister _reg_##funcName(#funcName, funcName)

// Registriert eine Funktion MIT Taste (onKlick)
// Verwendung oben in der .cpp des Items:
//   REGISTER_ITEM_FUNC_MIT_TASTE(grasItemklick, KEY_E)
#define REGISTER_ITEM_FUNC_MIT_TASTE(funcName, taste) \
    void funcName(); \
    static AutoRegister _reg_##funcName(#funcName, funcName, taste)