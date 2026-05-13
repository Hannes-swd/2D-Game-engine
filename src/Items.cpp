#include "Items.h"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

ItemManager g_itemManager;

// Lädt alle Items aus einer einzelnen JSON-Datei
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
                std::string pfad = entry.path().string();
                // item.json wurde schon oben geladen
                if (pfad == neuPfad) continue;
                std::cout << "[ItemManager] Zusatz-JSON: " << pfad << std::endl;
                ladeItemsAusJson(pfad, items);
            }
        }
    }

    std::cout << "[ItemManager] Gesamt: " << items.size() << " Items geladen." << std::endl;
}

void ItemManager::ladeItemAusDatei(const std::string& dateiPfad) {
    std::cout << "[ItemManager] Neue Item-Datei: " << dateiPfad << std::endl;
    ladeItemsAusJson(dateiPfad, items);
}