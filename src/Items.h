#pragma once
#include <string>
#include <functional>
#include <unordered_map>
#include <memory>
#include <vector>
#include "raylib.h"

struct Item {
    std::string id;
    std::string name;
    std::string texturPfad;
    Texture2D textur;
    std::string dateiPfad;

    std::function<void()> onKlick;
    std::function<void()> onInventar;
    std::function<void()> onHand;
    std::function<void()> onUpdate;

    // Taste die onKlick auslöst (Raylib KeyboardKey, z.B. KEY_E)
    // -1 = keine Taste gesetzt
    int klickTaste = -1;

    Item() : textur{0} {}

    void ladenTextur() {
        if (!texturPfad.empty()) {
            textur = LoadTexture(texturPfad.c_str());
        }
    }

    void entladenTextur() {
        if (textur.id != 0) {
            UnloadTexture(textur);
        }
    }
};

class ItemManager {
private:
    std::unordered_map<std::string, std::unique_ptr<Item>> items;

    // ── Funktions-Registry ────────────────────────────────────────────────────
    // Hier werden C++-Funktionen unter ihrem JSON-Namen gespeichert.
    // Eintrag über registriereFunktion() BEVOR scanneUndLadeItems() läuft.
    std::unordered_map<std::string, std::function<void()>> funktionsRegistry;

public:
    ~ItemManager() {
        for (auto& [id, item] : items) {
            item->entladenTextur();
        }
    }

    // Registriert eine C++-Funktion unter einem Namen (wie er in der JSON steht)
    void registriereFunktion(const std::string& name, std::function<void()> fn) {
        funktionsRegistry[name] = fn;
    }

    // Wie registriereFunktion, setzt zusätzlich die Taste für onKlick.
    // taste: ein Raylib KeyboardKey, z.B. KEY_E, KEY_F, KEY_SPACE usw.
    // Wird in Items.cpp beim Binden von "onKlick" automatisch übernommen.
    struct FuncEintrag {
        std::function<void()> fn;
        int taste = -1;   // -1 = keine Taste
    };
    std::unordered_map<std::string, FuncEintrag> funktionsRegistryEx;

    void registriereFunktionMitTaste(const std::string& name,
                                      std::function<void()> fn,
                                      int taste) {
        funktionsRegistryEx[name] = { fn, taste };
        funktionsRegistry[name]   = fn;   // auch im alten Registry für onHand/onInventar
    }

    // Sucht eine Funktion in der Registry; gibt nullptr zurück wenn nicht gefunden
    std::function<void()> sucheFunc(const std::string& name) const {
        auto it = funktionsRegistry.find(name);
        if (it != funktionsRegistry.end()) return it->second;
        return nullptr;
    }

    // Gibt die registrierte Taste zurück (-1 wenn keine)
    int sucheTaste(const std::string& name) const {
        auto it = funktionsRegistryEx.find(name);
        if (it != funktionsRegistryEx.end()) return it->second.taste;
        return -1;
    }

    // Fügt ein fertiges Item direkt ein (für manuelles Laden aus main.cpp)
    void registriereItem(std::unique_ptr<Item> item) {
        if (item) {
            items[item->id] = std::move(item);
        }
    }

    void scanneUndLadeItems();
    void ladeItemAusDatei(const std::string& dateiPfad);

    Item* getItem(const std::string& id) {
        auto it = items.find(id);
        if (it != items.end()) return it->second.get();
        return nullptr;
    }

    void fuehreAus(Item* item, const std::string& aktion) {
        if (!item) return;
        if      (aktion == "onKlick"    && item->onKlick)    item->onKlick();
        else if (aktion == "onInventar" && item->onInventar)  item->onInventar();
        else if (aktion == "onHand"     && item->onHand)      item->onHand();
        else if (aktion == "onUpdate"   && item->onUpdate)    item->onUpdate();
    }

    void zeichne(Item* item, int x, int y) {
        if (item && item->textur.id != 0) {
            DrawTexture(item->textur, x, y, WHITE);
        }
    }

    void updateAlleItems() {
        for (auto& [id, item] : items) {
            if (item->onUpdate) item->onUpdate();
        }
    }
};

extern ItemManager g_itemManager;