#include <fstream>
#include <iostream>
#include <filesystem>
#include "map.h"
#include "ground.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

void Map::setTile(int x, int y, const std::string& typ) {
    std::string key = std::to_string(x) + "," + std::to_string(y);
    tiles[key] = typ;
}

std::string Map::getTile(int x, int y) const {
    std::string key = std::to_string(x) + "," + std::to_string(y);
    
    if (tiles.count(key)) {
        return tiles.at(key);
    }
    return defaultType;
}

void Map::init(const GroundDatabase& ground) {
    groundDatabase = &ground;
    
    if (!ground.allGroundTypes.empty()) {
        defaultType = ground.allGroundTypes.begin().key();
        std::cout << "Default Bodentyp: " << defaultType << std::endl;
    }
}

void Map::save(const std::string& datei) const {
    namespace fs = std::filesystem;
    fs::path p(datei);
    if (p.has_parent_path()) {
        fs::create_directories(p.parent_path());
    }

    json ausgabe;
    ausgabe["name"] = "Meine Welt";
    for (const auto& [key, wert] : tiles) {
        ausgabe["tiles"][key] = wert;
    }
    ausgabe["default_typ"] = defaultType;

    std::ofstream f(datei);
    if (f.is_open()) {
        f << ausgabe.dump(4);
        f.flush();
        f.close();
        std::cout << "Map gespeichert: " << datei
                  << " (" << tiles.size() << " tiles)" << std::endl;
    } else {
        std::cerr << "Fehler: Kann nicht save " << datei << std::endl;
    }
}

void Map::load(const std::string& datei) {
    std::ifstream f(datei);
    
    if (f.is_open()) {
        json input;
        f >> input;
        
        tiles.clear();
        
        if (input.contains("tiles")) {
            for (auto& [key, wert] : input["tiles"].items()) {
                tiles[key] = wert;
            }
        }
        
        if (input.contains("default_typ")) {
            defaultType = input["default_typ"];
        }
        
        std::cout << "Map geladen: " << datei
                  << " (" << tiles.size() << " tiles)" << std::endl;
        std::cout << "Default Typ: " << defaultType << std::endl;
    } else {
        std::cerr << "Fehler: Kann nicht load " << datei << std::endl;
    }
}

void Map::clear() {
    tiles.clear();
}

int Map::getSize() const {
    return tiles.size();
}

void draw_ground(const Map& world, const GroundDatabase& ground, int tileSize) {
    for (const auto& [key, typ] : world.tiles) {
        size_t commaPos = key.find(',');
        if (commaPos != std::string::npos) {
            int x = std::stoi(key.substr(0, commaPos));
            int y = std::stoi(key.substr(commaPos + 1));
            
            auto it = ground.texturen.find(typ);
            if (it != ground.texturen.end() && it->second.id != 0) {
                DrawTexture(it->second, x * tileSize, y * tileSize, WHITE);
            } else {
                // Fallback: Violetter Block wenn Textur fehlt
                DrawRectangle(x * tileSize, y * tileSize, tileSize, tileSize, VIOLET);
            }
        }
    }
}