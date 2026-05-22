#pragma once
#include <unordered_map>
#include <string>
#include <nlohmann/json.hpp>

struct GroundDatabase;

struct Map {
    std::unordered_map<std::string, std::string> tiles;
    const GroundDatabase* groundDatabase = nullptr;
    std::string defaultType = "gras"; 
    
    void init(const GroundDatabase& ground);
    void setTile(int x, int y, const std::string& typ);
    std::string getTile(int x, int y) const;
    void save(const std::string& datei) const;
    void load(const std::string& datei);
    void clear();
    int getSize() const;
};

void draw_ground(const Map& world, const GroundDatabase& ground, int tileSize);