#pragma once
#include <unordered_map>
#include <string>
#include <nlohmann/json.hpp>

class BodenDatenbank;

struct Map {
    std::unordered_map<std::string, std::string> tiles;
    const BodenDatenbank* bodenDatenbank = nullptr;
    std::string defaultTyp = "gras"; 
    
    void init(const BodenDatenbank& boden);
    void setTile(int x, int y, const std::string& typ);
    std::string getTile(int x, int y) const;
    void speichern(const std::string& datei) const;
    void laden(const std::string& datei);
    void clear();
    int getSize() const;
};