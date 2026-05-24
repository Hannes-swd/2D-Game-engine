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
    for (const auto& [key, pb] : buildings) {
        ausgabe["buildings"][key]["buildingId"]  = pb.buildingId;
        ausgabe["buildings"][key]["x"]           = pb.x;
        ausgabe["buildings"][key]["y"]           = pb.y;
        ausgabe["buildings"][key]["width"]       = pb.width;
        ausgabe["buildings"][key]["height"]      = pb.height;
        ausgabe["buildings"][key]["instanceId"]  = pb.instanceId;
        ausgabe["buildings"][key]["state"]       = pb.state;
    }

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

        if (input.contains("buildings")) {
            for (auto& [key, bData] : input["buildings"].items()) {
                PlacedBuilding pb;
                pb.buildingId = bData.value("buildingId", "");
                pb.x          = bData.value("x",          0);
                pb.y          = bData.value("y",          0);
                pb.width      = bData.value("width",      1);
                pb.height     = bData.value("height",     1);
                pb.instanceId = bData.value("instanceId", "");
                pb.state      = bData.value("state",      "");
                buildings[key] = pb;
            }
        }
        
        std::cout << "Map geladen: " << datei
                  << " (" << tiles.size() << " tiles)" << std::endl;
        std::cout << "Default Typ: " << defaultType << std::endl;
    } else {
        std::cerr << "Fehler: Kann nicht load " << datei << std::endl;
    }
}

void Map::placeBuilding(const PlacedBuilding& pb) {
    std::string key = std::to_string(pb.x) + "," + std::to_string(pb.y);
    buildings[key] = pb;
}

PlacedBuilding* Map::getBuildingAt(int x, int y) {
    for (auto& [key, pb] : buildings) {
        if (x >= pb.x && x < pb.x + pb.width &&
            y >= pb.y && y < pb.y + pb.height) {
            return &pb;
        }
    }
    return nullptr;
}

void Map::clear() {
    tiles.clear();
    buildings.clear();
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