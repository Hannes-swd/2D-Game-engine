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

// Sucht Funktionsnamen in einer .cpp Datei per Regex (nur STL, kein WinAPI nötig)
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

void ladeFunktionenAusDatei(const std::string& itemId, const std::string& dateiPfad) {
    std::cout << "Hinweis: Dynamisches Laden von " << dateiPfad << " wird zur Compile-Zeit benötigt." << std::endl;
    std::cout << "Die Funktionen für Item '" << itemId << "' müssen in einer zentralen Datei registriert werden." << std::endl;
}

void ItemManager::scanneUndLadeItems() {
    namespace fs = std::filesystem;
    
    if (!fs::exists(itemsOrdner)) {
        std::cout << "Items-Ordner nicht gefunden: " << itemsOrdner << std::endl;
        return;
    }
    
    for (const auto& entry : fs::recursive_directory_iterator(itemsOrdner)) {
        if (entry.path().extension() == ".cpp" || entry.path().extension() == ".json") {
            std::string pfad = entry.path().string();
            std::cout << "Gefunden: " << pfad << std::endl;
            
            if (entry.path().extension() == ".json") {
                std::ifstream f(pfad);
                if (f.is_open()) {
                    json data;
                    f >> data;
                    
                    for (auto& [id, itemData] : data.items()) {
                        auto item = std::make_unique<Item>();
                        item->id = id;
                        item->name = itemData.value("name", id);
                        item->texturPfad = itemData.value("textur", "");
                        item->dateiPfad = itemData.value("sourceFile", "");
                        item->ladenTextur();
                        
                        items[id] = std::move(item);
                        std::cout << "Item geladen: " << id << std::endl;
                    }
                }
            }
        }
    }
}

void ItemManager::ladeItemAusDatei(const std::string& dateiPfad) {
    std::cout << "Neue Item-Datei entdeckt: " << dateiPfad << std::endl;
}