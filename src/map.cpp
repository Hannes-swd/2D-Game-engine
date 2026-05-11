#include <fstream>
#include <iostream>
#include "map.h"
#include "ground.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

void Map::setTile(int x, int y, const std::string& typ) {
    std::string key = std::to_string(x) + "," + std::to_string(y);
    
    if (!bodenDatenbank || typ == defaultTyp) {
        tiles.erase(key);
    } else {
        tiles[key] = typ;
    }
}

std::string Map::getTile(int x, int y) const {
    std::string key = std::to_string(x) + "," + std::to_string(y);
    
    if (tiles.count(key)) {
        return tiles.at(key);
    }
    return defaultTyp;
}

void Map::init(const BodenDatenbank& boden) {
    bodenDatenbank = &boden;
    
    if (!boden.alle_boden_arten.empty()) {
        defaultTyp = boden.alle_boden_arten.begin().key();
        std::cout << "Default Bodentyp: " << defaultTyp << std::endl;
    }
}

void Map::speichern(const std::string& datei) const {
    json ausgabe;
    
    for (const auto& [key, wert] : tiles) {
        ausgabe["tiles"][key] = wert;
    }
    ausgabe["default_typ"] = defaultTyp;
    
    std::ofstream f(datei);
    if (f.is_open()) {
        f << ausgabe.dump(4);
        std::cout << "Map gespeichert: " << datei << std::endl;
    } else {
        std::cerr << "Fehler: Kann nicht speichern " << datei << std::endl;
    }
}

void Map::laden(const std::string& datei) {
    std::ifstream f(datei);
    
    if (f.is_open()) {
        json input;
        f >> input;
        
        tiles.clear();
        
        // Lade Tiles
        if (input.contains("tiles")) {
            for (auto& [key, wert] : input["tiles"].items()) {
                tiles[key] = wert;
            }
        }
        
        if (input.contains("default_typ")) {
            defaultTyp = input["default_typ"];
        }
        
        std::cout << "Map geladen: " << datei << " (" << tiles.size() << " tiles)" << std::endl;
        std::cout << "Default Typ: " << defaultTyp << std::endl;
    } else {
        std::cerr << "Fehler: Kann nicht laden " << datei << std::endl;
    }
}

void Map::clear() {
    tiles.clear();
}

int Map::getSize() const {
    return tiles.size();
}

void draw_ground(const Map& welt, const BodenDatenbank& boden, int tileSize) {
    for (const auto& [key, typ] : welt.tiles) {
        // Parse x,y from key (format "x,y")
        size_t commaPos = key.find(',');
        if (commaPos != std::string::npos) {
            int x = std::stoi(key.substr(0, commaPos));
            int y = std::stoi(key.substr(commaPos + 1));
            
            // Versuche Textur zu zeichnen
            auto it = boden.texturen.find(typ);
            if (it != boden.texturen.end() && it->second.id != 0) {
                DrawTexture(it->second, x * tileSize, y * tileSize, WHITE);
            } else {
                // Fallback: Violetter Block wenn Textur fehlt
                DrawRectangle(x * tileSize, y * tileSize, tileSize, tileSize, VIOLET);
            }
        }
    }
}