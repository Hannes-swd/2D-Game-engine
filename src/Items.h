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
    std::string itemsOrdner = "items/";  // Ordner wo die Item-Dateien liegen
    
    // Funktions-Pointer für dynamisch geladene Funktionen
    struct FunctionTable {
        void* onKlick   = nullptr;
        void* onInventar = nullptr;
        void* onHand    = nullptr;
        void* onUpdate  = nullptr;
    };
    std::unordered_map<std::string, FunctionTable> geladeneFunktionen;

public:
    ~ItemManager() {
        for (auto& [id, item] : items) {
            item->entladenTextur();
        }
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
        if (it != items.end()) {
            return it->second.get();
        }
        return nullptr;
    }

    void fuehreAus(Item* item, const std::string& aktion) {
        if (!item) return;
        if      (aktion == "onKlick"   && item->onKlick)   item->onKlick();
        else if (aktion == "onInventar"&& item->onInventar) item->onInventar();
        else if (aktion == "onHand"    && item->onHand)     item->onHand();
        else if (aktion == "onUpdate"  && item->onUpdate)   item->onUpdate();
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