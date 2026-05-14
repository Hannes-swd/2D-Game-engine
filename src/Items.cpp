#include "Items.h"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

ItemManager g_itemManager;

// ── Hilfsfunktion: verbindet einen JSON-Funktionsnamen mit der Registry ───────
static void bindFunc(std::function<void()>& target,
                     const json& data,
                     const std::string& jsonKey)
{
    if (!data.contains(jsonKey)) return;
    std::string funcName = data[jsonKey].get<std::string>();
    if (funcName.empty()) return;

    auto fn = g_itemManager.sucheFunc(funcName);
    if (fn) {
        target = fn;
        std::cout << "[ItemManager] Callback gebunden: " << jsonKey
                  << " -> " << funcName << std::endl;
    } else {
        std::cerr << "[ItemManager] WARNUNG: Funktion '" << funcName
                  << "' (fuer " << jsonKey << ") nicht in der Registry!" << std::endl;
    }
}

// Wie bindFunc, setzt zusätzlich item->klickTaste aus der Registry
static void bindKlick(Item& item, const json& data, const std::string& jsonKey)
{
    if (!data.contains(jsonKey)) return;
    std::string funcName = data[jsonKey].get<std::string>();
    if (funcName.empty()) return;

    auto fn = g_itemManager.sucheFunc(funcName);
    if (fn) {
        item.onKlick   = fn;
        item.klickTaste = g_itemManager.sucheTaste(funcName);
        std::cout << "[ItemManager] onKlick gebunden: " << funcName
                  << "  Taste=" << item.klickTaste << std::endl;
    } else {
        std::cerr << "[ItemManager] WARNUNG: Funktion '" << funcName
                  << "' (fuer " << jsonKey << ") nicht in der Registry!" << std::endl;
    }
}

// ── Lädt alle Items aus einer einzelnen JSON-Datei ────────────────────────────
static void ladeItemsAusJson(const std::string& pfad,
                              std::unordered_map<std::string, std::unique_ptr<Item>>& items)
{
    std::ifstream f(pfad);
    if (!f.is_open()) {
        std::cerr << "[ItemManager] Kann nicht öffnen: " << pfad << std::endl;
        return;
    }

    json data;
    f >> data;

    for (auto& [id, itemData] : data.items()) {
        auto item        = std::make_unique<Item>();
        item->id         = id;
        item->name       = itemData.value("name",   id);
        item->texturPfad = itemData.value("textur", "");
        item->dateiPfad  = pfad;
        item->ladenTextur();

        // ── Callbacks aus JSON-Namen über die Registry binden ──────────────
        // JSON-Key   → Item-Callback
        // "onKlick"  → item->onKlick     (alter Key-Name in JSON: "onKlick")
        // "inInventar"→ item->onInventar  (JSON nennt es "inInventar")
        // "inHand"   → item->onHand       (JSON nennt es "inHand")
        // "onUpdate" → item->onUpdate
        bindKlick(*item,        itemData, "onKlick");
        bindFunc(item->onInventar, itemData, "inInventar");
        bindFunc(item->onHand,     itemData, "inHand");
        bindFunc(item->onUpdate,   itemData, "onUpdate");

        items[id] = std::move(item);
        std::cout << "[ItemManager] Item geladen: " << id << std::endl;
    }
}

void ItemManager::scanneUndLadeItems() {
    namespace fs = std::filesystem;

    // 1. Neue Struktur: assets/json/items/item.json
    const std::string neuPfad = "assets/json/items/item.json";
    if (fs::exists(neuPfad)) {
        std::cout << "[ItemManager] Lade: " << neuPfad << std::endl;
        ladeItemsAusJson(neuPfad, items);
    }

    // 2. Alter Fallback-Pfad: assets/json/item.json
    const std::string altPfad = "assets/json/item.json";
    if (fs::exists(altPfad)) {
        std::cout << "[ItemManager] Lade (Fallback): " << altPfad << std::endl;
        ladeItemsAusJson(altPfad, items);
    }

    // 3. Zusätzlich alle weiteren .json im items-Ordner scannen
    const std::string ordner = "assets/json/items/";
    if (fs::exists(ordner)) {
        for (const auto& entry : fs::recursive_directory_iterator(ordner)) {
            if (entry.path().extension() == ".json") {
                std::string p = entry.path().string();
                if (p == neuPfad) continue;
                std::cout << "[ItemManager] Zusatz-JSON: " << p << std::endl;
                ladeItemsAusJson(p, items);
            }
        }
    }

    std::cout << "[ItemManager] Gesamt: " << items.size() << " Items geladen." << std::endl;
}

void ItemManager::ladeItemAusDatei(const std::string& dateiPfad) {
    std::cout << "[ItemManager] Neue Item-Datei: " << dateiPfad << std::endl;
    ladeItemsAusJson(dateiPfad, items);
}