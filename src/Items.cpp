#include "Items.h"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <regex>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

ItemManager g_itemManager;

std::vector<std::string> findeFunktionenInDatei(const std::string& dateiPfad) {
    std::vector<std::string> gefundeneFunktionen;
    
    std::ifstream file(dateiPfad);
    if (!file.is_open()) return gefundeneFunktionen;
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    std::regex funktionsRegex(R"(void\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*\()");
    std::smatch match;
    std::string::const_iterator searchStart(content.cbegin());
    
    while (std::regex_search(searchStart, content.cend(), match, funktionsRegex)) {
        gefundeneFunktionen.push_back(match[1].str());
        searchStart = match.suffix().first;
    }
    
    return gefundeneFunktionen;
}

// Lädt Items aus einer einzelnen JSON-Datei (z.B. assets/json/item.json)
static void ladeItemsAusJson(const std::string& pfad,
                              std::unordered_map<std::string, std::unique_ptr<Item>>& items)
{
    std::ifstream f(pfad);
    if (!f.is_open()) {
        std::cerr << "Fehler: Kann Item-JSON nicht öffnen: " << pfad << std::endl;
        return;
    }

    json data;
    f >> data;

    for (auto& [id, itemData] : data.items()) {
        auto item         = std::make_unique<Item>();
        item->id          = id;
        item->name        = itemData.value("name",    id);
        item->texturPfad  = itemData.value("textur",  "");
        item->dateiPfad   = pfad;
        item->ladenTextur();

        items[id] = std::move(item);
        std::cout << "Item geladen: " << id << std::endl;
    }
}

void ItemManager::scanneUndLadeItems() {
    namespace fs = std::filesystem;

    // 1. Zuerst die zentrale item.json laden (dein aktuelles Format)
    const std::string einzelDatei = "assets/json/item.json";
    if (fs::exists(einzelDatei)) {
        std::cout << "Lade Items aus: " << einzelDatei << std::endl;
        ladeItemsAusJson(einzelDatei, items);
    }

    // 2. Zusätzlich den Unterordner assets/json/items/ scannen (falls vorhanden)
    if (fs::exists(itemsOrdner)) {
        for (const auto& entry : fs::recursive_directory_iterator(itemsOrdner)) {
            if (entry.path().extension() == ".json") {
                std::string pfad = entry.path().string();
                std::cout << "Gefunden: " << pfad << std::endl;
                ladeItemsAusJson(pfad, items);
            }
        }
    }

    std::cout << "ItemManager: " << items.size() << " Items geladen." << std::endl;
}

void ItemManager::ladeItemAusDatei(const std::string& dateiPfad) {
    std::cout << "Neue Item-Datei entdeckt: " << dateiPfad << std::endl;
    ladeItemsAusJson(dateiPfad, items);
}